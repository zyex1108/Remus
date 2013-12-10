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

#ifndef remus_client_JobFileRequest_h
#define remus_client_JobFileRequest_h

#include <string>
#include <boost/shared_ptr.hpp>

#include <remus/client/AbstractJobRequest.h>


//JobFileRequest is a efficient way to send a file and collection of arguments
//to a worker.
//If the client, server, and worker are all on the same machine we will
//send only the file name and arguments to the worker.
//at any point if the client, server, or worker are on different machines
//we will read the file and store the contents and send that as a binary
//blob
//So if the client and server are on the same machine, when the server is
//about to dispatch the job to a remote worker it will read the file and
//send the contents of the file to the server
//
//
//Note: the current issues with this behavior is that we expect
//that the server will have read privileges on the file that we are sending
//
//Note: We need to extend the FileRequest to be able to handle multiple
//files in a nice way
//
//A job request has two purposes. First it is sent to the server to determine
//if the mesh type and data model that the job request has is supported.
//Secondly it is used to transport the actual job to the server
//
//Note: Currently no server supports the ability to restrict a mesh request
//to a single named mesher. This option is expected to be supported in the future.
//For now we replicate this feature by making <MeshIOType,inputMeshDataType> be
//unique for each mesher
namespace remus{
namespace client{

class Client;

class JobFileRequest : public remus::client::AbstractJobRequest
{
public:
  typedef remus::client::AbstractJobRequest superclass;

  //Construct a job request with only a mesher type given. This is used to
  //when asking a server if it supports a type of mesh
  JobFileRequest(remus::MESH_INPUT_TYPE inputFileType,
             remus::MESH_OUTPUT_TYPE outputMeshIOType);

  //Construct a job request with a mesh type and a file and arguments
  //to run the job. This is used when submitting a job from the client to the server.
  JobFileRequest(remus::MESH_INPUT_TYPE inputFileType,
             remus::MESH_OUTPUT_TYPE outputMeshIOType,
             const std::string& path,
             const std::string& args);

  //Construct a job request with the given types held inside the remus::common::MeshIOType object
  explicit JobFileRequest(remus::common::MeshIOType combinedType);

  //Construct a job request with the given types held inside the remus::common::MeshIOType object
  //and a path and args for a file
  JobFileRequest(remus::common::MeshIOType combinedType,
                 const std::string& path,
                 const std::string& args);

  const std::string& commandArgs() const { return this->CommandArgs; }

  virtual remus::client::AbstractJobRequest::ClassType requestClassType() const
    { return remus::client::AbstractJobRequest::FILE; }

private:
  friend class remus::client::Client;
  friend std::string to_string(const remus::client::JobFileRequest& request);
  friend remus::client::JobFileRequest to_JobFileRequest(const std::string& msg);

  //state to mark if the file should be read when we serialize
  void setServerToBeRemote(bool isRemote) const;

  //deserialize function
  static remus::client::JobFileRequest deserialize(std::stringstream& buffer);

  //serialize function
  void serialize(std::stringstream& buffer) const;

  std::string CommandArgs;

  struct InternalFileRep;
  boost::shared_ptr<InternalFileRep> FileRepresentation;
};

//------------------------------------------------------------------------------
inline std::string to_string(const remus::client::JobFileRequest& request)
{
  //ask the data request to place the contents into a string, this will allow
  //us to encode the file contents or the file name based if the server
  //is local or remote
  std::stringstream buffer;
  request.serialize(buffer);
  return buffer.str();
}


//------------------------------------------------------------------------------
inline remus::client::JobFileRequest to_JobFileRequest(const std::string& msg)
{
  //decode the string into a data request this will allow
  //us to decode the string based on if the server is local or remote
  std::stringstream buffer(msg);
  return remus::client::JobFileRequest::deserialize(buffer);
}


//------------------------------------------------------------------------------
inline remus::client::JobFileRequest to_JobFileRequest(const char* data, int size)
{
  //convert a job request from a string, used as a hack to serialize
  std::string temp(size,char());
  memcpy(const_cast<char*>(temp.c_str()),data,size);
  return to_JobFileRequest( temp );
}


}
}

#endif
