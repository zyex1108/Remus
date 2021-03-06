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

#ifndef remus_server_WorkeryFactory_h
#define remus_server_WorkeryFactory_h

//included for export symbols
#include <remus/server/ServerExports.h>
#include <remus/server/WorkerFactoryBase.h>

namespace remus{
namespace server{

//forward declare FactoryFileParser, a helper class that can be over-ridden
//to allow users to parse different file formats
class FactoryFileParser;

struct FactoryWorkerSpecification;

//The Worker Factory.
//First it locates all files that match a given extension of the default extension
//of .rw. These files are than parsed to determine what type of local Remus workers
//we can launch.
class REMUSSERVER_EXPORT WorkerFactory : public WorkerFactoryBase
{
public:
  //Work Factory defaults to creating a maximum of 1 workers at once,
  //with a default extension of ".rw" to search for
  WorkerFactory();

  //Create a worker factory with a different file extension to
  //search for. The extension must start with a period.
  WorkerFactory(const std::string& ext);

  //Create a worker factory with a different file extension, and a custom
  //file parsing algorithm. The extension must start with a period.
  WorkerFactory(const std::string& ext,
                boost::shared_ptr<FactoryFileParser>& parser);

  virtual ~WorkerFactory();

  //add a path to search for workers.
  //by default we only search the current working directory
  void addWorkerSearchDirectory(const std::string& directory);

  //return all the MeshIOTypes that the factory can possibly make
  //this allows the client to discover workers types that can be constructed
  virtual remus::common::MeshIOTypeSet supportedIOTypes() const;

  //return all the JobRequirementsSet for all workers that match a give
  //MeshIOType
  virtual remus::proto::JobRequirementsSet workerRequirements(
                                       remus::common::MeshIOType type) const;

  //return if the worker factory has a worker that matches the given requirements
  virtual bool haveSupport(const remus::proto::JobRequirements& reqs) const;

  //request the factory to construct a worker given a requirements and a lifespan
  //can return false if the factory doesn't support these requirements, or
  //if the factory already has too many workers in existence already.
  virtual bool createWorker(const remus::proto::JobRequirements& type,
                            WorkerFactoryBase::FactoryDeletionBehavior lifespan);

  //checks all current processes and removes any that have
  //shutdown
  virtual void updateWorkerCount();

  virtual unsigned int currentWorkerCount() const;

  //return the worker file extension we have
  std::string workerExtension() const { return this->WorkerExtension;  }

private:
  //this method only handles constructing the worker
  //it is expected that all checks to make sure that the worker type
  //have been done, and that we have room for the worker already
  virtual bool addWorker(
    const FactoryWorkerSpecification& worker,
    WorkerFactoryBase::FactoryDeletionBehavior lifespan);

  std::string WorkerExtension;

  boost::shared_ptr<FactoryFileParser> Parser;

  struct WorkerTracker;
  boost::shared_ptr<WorkerTracker> Tracker;
};

}
}

#endif
