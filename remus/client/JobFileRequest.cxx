//=============================================================================
//
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//
//=============================================================================
#include <remus/client/JobFileRequest.h>

#include "kwStat.h"

#include <sstream>
#include <fstream>

namespace remus{
namespace client{


struct JobFileRequest::InternalFileRep
{
  InternalFileRep():
    FilePath(),
    FileContents(),
    HasRemoteServer(false)
    {

    }

  InternalFileRep(const std::string& path):
    FilePath(path),
    FileContents(),
    HasRemoteServer(false)
    {
    }

  std::string FilePath;
  std::string FileContents;
  bool HasRemoteServer;
};

//------------------------------------------------------------------------------
JobFileRequest::JobFileRequest(remus::MESH_INPUT_TYPE inputFileType,
           remus::MESH_OUTPUT_TYPE outputMeshIOType):
  superclass(inputFileType,outputMeshIOType),
  CommandArgs(),
  FileRepresentation(new InternalFileRep())
  {
  }

//------------------------------------------------------------------------------
JobFileRequest::JobFileRequest(remus::MESH_INPUT_TYPE inputFileType,
           remus::MESH_OUTPUT_TYPE outputMeshIOType,
           const std::string& path,
           const std::string& args):
  superclass(inputFileType,outputMeshIOType),
  CommandArgs(args),
  FileRepresentation(new InternalFileRep(path))
  {
  }

//------------------------------------------------------------------------------
JobFileRequest::JobFileRequest(remus::common::MeshIOType combinedType):
  superclass(combinedType),
  CommandArgs(),
  FileRepresentation(new InternalFileRep())
  {
  }

//------------------------------------------------------------------------------
JobFileRequest::JobFileRequest(remus::common::MeshIOType combinedType,
               const std::string& path,
               const std::string& args):
  superclass(combinedType),
  CommandArgs(args),
  FileRepresentation(new InternalFileRep(path))
  {

  }

//------------------------------------------------------------------------------
void JobFileRequest::serialize(std::stringstream& buffer) const
{
  //label the type of job file
  buffer << this->requestClassType() << std::endl;

  //send the input and output type
  buffer << this->type() << std::endl;

  //next are command line args
  buffer << this->CommandArgs.size() << std::endl;
  remus::internal::writeString(buffer, this->CommandArgs);

  //next is if we are remote or local
  buffer << this->FileRepresentation->HasRemoteServer << std::endl;

  //the last bit of common info to transmit is the file name.
  //We want to transmit the filename as it is useful data for the worker to
  //know, It might need to do different operations based on the extension
  buffer << this->FileRepresentation->FilePath.size() << std::endl;
  remus::internal::writeString(buffer, this->FileRepresentation->FilePath);

  //now if the server is remote we transfer the contents of the file
  if(this->FileRepresentation->HasRemoteServer)
    {
    std::ifstream fileContents(this->FileRepresentation->FilePath.c_str(),
                               std::ios_base::in | std::ios::binary);
    //the contents of the file prefixed with length
    if(fileContents.is_open())
      {
      //we don't want to seak the entire file to get the length, so
      //use kwStat to get us the proper size of the file by using stat
      //this will return the valid size

      //todo move to boost::iostreams since windows 2008,2010 fstream
      //can't support files larger than 4GB
      kw::stat s(this->FileRepresentation->FilePath);
      buffer << s.fileLength() << std::endl;
      buffer << fileContents.rdbuf() << std::endl;
      }
    }
}

//------------------------------------------------------------------------------
remus::client::JobFileRequest JobFileRequest::deserialize(
                                                    std::stringstream& buffer)
{
  int tempJobClassType;
  remus::common::MeshIOType jobRequirements;
  std::string commandArgs;

  buffer >> tempJobClassType;
  buffer >> jobRequirements;
  buffer >> commandArgs;

  const remus::client::AbstractJobRequest::ClassType jobClassType=
    static_cast<remus::client::AbstractJobRequest::ClassType>(tempJobClassType);
  (void)jobClassType; //currently not used, just documenting the type

  //construct the basic file request
  remus::client::JobFileRequest jfr(jobRequirements);

  //set the command args
  jfr.CommandArgs = commandArgs;

  //set internal state of the file, are we still a file name or do we have data
  buffer >> jfr.FileRepresentation->HasRemoteServer;

  //read the file name data
  int fpathLen;
  buffer >> fpathLen;
  jfr.FileRepresentation->FilePath =
                          remus::internal::extractString(buffer,fpathLen);

  //read the contents of the file if needed
  if(jfr.FileRepresentation->HasRemoteServer)
    {
    int fcontentsLen;
    buffer >> fcontentsLen;
    jfr.FileRepresentation->FileContents =
                remus::internal::extractString(buffer,fcontentsLen);
    }
  return jfr;

}

}
}
