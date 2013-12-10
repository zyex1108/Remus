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

#ifndef remus_client_AbstractJobRequest_h
#define remus_client_AbstractJobRequest_h

#include <remus/common/MeshIOType.h>


namespace remus{
namespace client{

class AbstractJobRequest
{
public:
  enum ClassType
    {
    DATA=1,
    FILE=2
    };

  //Set the separate mesh input and output type
  AbstractJobRequest(remus::MESH_INPUT_TYPE inputFileType,
                     remus::MESH_OUTPUT_TYPE outputMeshIOType):
    CombinedType(inputFileType,outputMeshIOType)
    {
    }

  //Set the combined mesh input and output type
  explicit AbstractJobRequest(remus::common::MeshIOType combinedType):
    CombinedType(combinedType)
    {
    }

  virtual ~AbstractJobRequest(){}
  AbstractJobRequest(const AbstractJobRequest&){}

  //allow people to ask what type of dataType we are, currently used
  //to encode our serialization
  virtual remus::client::AbstractJobRequest::ClassType requestClassType() const=0;

  //constructs a variable that represents the combination of the input
  //and output type as a single integer
  remus::common::MeshIOType type() const { return this->CombinedType; }

  remus::MESH_OUTPUT_TYPE outputType() const { return CombinedType.outputType(); }
  remus::MESH_INPUT_TYPE inputType() const { return CombinedType.inputType(); }

private:
  remus::common::MeshIOType CombinedType;
};


}
}

#endif
