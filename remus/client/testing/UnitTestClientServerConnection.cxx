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

#include <remus/client/ServerConnection.h>
#include <remus/common/remusGlobals.h>
#include <remus/testing/Testing.h>

#include <string>

namespace {

zmq::socketInfo<zmq::proto::tcp> make_tcp_socket(std::string host, int port)
{
  return zmq::socketInfo<zmq::proto::tcp>(host,port);
}

zmq::socketInfo<zmq::proto::ipc> make_ipc_socket(std::string host)
{
  return zmq::socketInfo<zmq::proto::ipc>(host);
}

zmq::socketInfo<zmq::proto::inproc> make_inproc_socket(std::string host)
{
  return zmq::socketInfo<zmq::proto::inproc>(host);
}

} //namespace


int UnitTestClientServerConnection(int, char *[])
{

  remus::client::ServerConnection sc;
  REMUS_ASSERT( (sc.endpoint().size() > 0) );
  const std::string default_endpoint = sc.endpoint();

  zmq::socketInfo<zmq::proto::tcp> default_socket("127.0.0.1",
                                                  remus::SERVER_CLIENT_PORT);


  REMUS_ASSERT( (sc.endpoint() == default_socket.endpoint()) );

  remus::client::ServerConnection test_socket_sc(default_socket);
  remus::client::ServerConnection test_socket_sc2(
                                                make_tcp_socket("127.0.0.1",1));

  REMUS_ASSERT( (test_socket_sc.endpoint() == default_endpoint) );
  REMUS_ASSERT( (test_socket_sc2.endpoint() != default_endpoint) );

  remus::client::ServerConnection test_full_sc("74.125.30.106",82);
  remus::client::ServerConnection test_full_sc2 =
              remus::client::make_ServerConnection("tcp://74.125.30.106:82");

  REMUS_ASSERT( (test_full_sc.endpoint() ==
                        make_tcp_socket("74.125.30.106",82).endpoint()) );
  REMUS_ASSERT( (test_full_sc.endpoint() == test_full_sc2.endpoint()) );
  REMUS_ASSERT( (test_full_sc.endpoint() != default_endpoint) );

  //test local host bool with tcp ip
  REMUS_ASSERT( (sc.isLocalEndpoint()==true) );
  REMUS_ASSERT( (test_socket_sc.isLocalEndpoint()==true) );
  REMUS_ASSERT( (test_socket_sc2.isLocalEndpoint()==true) );
  REMUS_ASSERT( (test_full_sc.isLocalEndpoint()==false) );

  //test inproc
  remus::client::ServerConnection sc_inproc( make_inproc_socket("client") );
  REMUS_ASSERT( (sc_inproc.isLocalEndpoint()==true) );
  REMUS_ASSERT( (sc_inproc.endpoint() == std::string("inproc://client")) );

  remus::client::ServerConnection sc_inproc2 =
                      remus::client::make_ServerConnection("inproc://client");
  REMUS_ASSERT( (sc_inproc2.isLocalEndpoint()==true) );
  REMUS_ASSERT( (sc_inproc2.endpoint() == std::string("inproc://client")) );

  //test ipc
  remus::client::ServerConnection sc_ipc( make_ipc_socket("task_pool") );
  REMUS_ASSERT( (sc_ipc.isLocalEndpoint()==true) );
  REMUS_ASSERT( (sc_ipc.endpoint() == std::string("ipc://task_pool")) );

  remus::client::ServerConnection sc_ipc2 =
                      remus::client::make_ServerConnection("ipc://task_pool");
  REMUS_ASSERT( (sc_ipc.isLocalEndpoint()==true) );
  REMUS_ASSERT( (sc_ipc.endpoint() == std::string("ipc://task_pool")) );


  //test sharing a context.
  remus::client::ServerConnection share_context;
  share_context.context(sc.context());
  REMUS_ASSERT( (share_context.context() == sc.context()) )

  remus::client::ServerConnection share_context2;
  share_context2.context( remus::client::make_ServerContext(4) );
  REMUS_ASSERT( (share_context.context() != share_context2.context()) );

  boost::shared_ptr<zmq::context_t> old_context = share_context2.context();
  share_context2.context( remus::client::make_ServerContext(4) );
  REMUS_ASSERT( (old_context != share_context2.context()) );

  share_context.context( share_context2.context() );
  REMUS_ASSERT( (share_context.context() == share_context2.context()) );


  return 0;
}
