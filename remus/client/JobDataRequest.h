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

#ifndef remus_client_JobDataRequest_h
#define remus_client_JobDataRequest_h

#include <algorithm>
#include <string>
#include <sstream>

#include <remus/client/AbstractJobRequest.h>

//JobDataRequest is the easiest way to send raw binary data streams
//to a worker. Unlike its sibling JobFileRequest it doesn't have command
//arguments or has any concept of locality

//A job request has two purposes. First it is sent to the server to determine
//if the mesh type and data model that the job request has is supported.
//Secondly it is used to transport the actual job to the server

//Note: Currently no server supports the ability to restrict a mesh request
//to a single named mesher. This option is expected to be supported in the future.
//For now we replicate this feature by making <MeshIOType,inputMeshDataType> be
//unique for each mesher
namespace remus{
namespace client{
class JobDataRequest : public remus::client::AbstractJobRequest
{
public:
  typedef remus::client::AbstractJobRequest superclass;

  //Construct a job request with only a mesher type given. This is used to
  //when asking a server if it supports a type of mesh
  JobDataRequest(remus::MESH_INPUT_TYPE inputFileType,
             remus::MESH_OUTPUT_TYPE outputMeshIOType):
    superclass(inputFileType,outputMeshIOType),
    JobInfo()
    {
    }

  //Construct a job request with a mesh type and the info required by the worker
  //to run the job. This is used when submitting a job from the client to the server.
  JobDataRequest(remus::MESH_INPUT_TYPE inputFileType,
             remus::MESH_OUTPUT_TYPE outputMeshIOType,
             const std::string& info):
    superclass(inputFileType,outputMeshIOType),
    JobInfo(info)
    {
    }

  //Construct a job request with the given types held inside the remus::common::MeshIOType object
  explicit JobDataRequest(remus::common::MeshIOType combinedType):
    superclass(combinedType),
    JobInfo()
    {
    }

  //Construct a job request with the given types held inside the remus::common::MeshIOType object
  JobDataRequest(remus::common::MeshIOType combinedType, const std::string& info):
    superclass(combinedType),
    JobInfo(info)
    {
    }

  const std::string& jobInfo() const { return JobInfo; }

  virtual remus::client::AbstractJobRequest::ClassType requestClassType() const
    { return remus::client::AbstractJobRequest::DATA; }

private:
  std::string JobInfo;
};

//------------------------------------------------------------------------------
inline std::string to_string(const remus::client::JobDataRequest& request)
{
  //convert a request to a string, used as a hack to serialize
  //encoding is simple, contents newline separated

  std::stringstream buffer;
  buffer << request.requestClassType() << std::endl;
  buffer << request.type() << std::endl;
  buffer << request.jobInfo().length() << std::endl;
  remus::internal::writeString(buffer, request.jobInfo());
  return buffer.str();
}


//------------------------------------------------------------------------------
inline remus::client::JobDataRequest to_JobDataRequest(const std::string& msg)
{
  //convert a job detail from a string, used as a hack to serialize

  //de-serializing is tricky since we need to be able to fetch the type of the
  //class before we convert it
  std::stringstream buffer(msg);

  int tempJobClassType;
  remus::common::MeshIOType jobRequirements;

  buffer >> tempJobClassType; //not used
  buffer >> jobRequirements;

  const remus::client::AbstractJobRequest::ClassType jobClassType=
    static_cast<remus::client::AbstractJobRequest::ClassType>(tempJobClassType);
  (void)jobClassType; //currently not used, just documenting the type

  int dataLen;
  std::string data;

  buffer >> dataLen;
  data = remus::internal::extractString(buffer,dataLen);
  return remus::client::JobDataRequest(jobRequirements,data);
}


//------------------------------------------------------------------------------
inline remus::client::JobDataRequest to_JobDataRequest(const char* data, int size)
{
  //convert a job request from a string, used as a hack to serialize
  std::string temp(size,char());
  std::copy( data, data+size, temp.begin() );
  return to_JobRequest( temp );
}


}
}

#endif
