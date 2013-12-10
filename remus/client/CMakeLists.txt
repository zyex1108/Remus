project(Remus_Client)

set(headers
    Client.h
    Job.h
    JobDataRequest.h
    JobFileRequest.h
    JobResult.h
    JobStatus.h
    ServerConnection.h
    )

set(client_srcs
    Client.cxx
    JobFileRequest.cxx
    )

add_library(RemusClient ${client_srcs} ${headers})
target_link_libraries(RemusClient
                      LINK_PUBLIC RemusCommon
                      )

#we need to use kwStat to get basic file size information
#since it is only used inside implementation files no need to install it
target_include_directories(RemusClient
                          PRIVATE
                          ${Remus_SOURCE_DIR}/thirdparty/kwStat)

#create the export header symbol exporting
remus_export_header(RemusClient ClientExports.h)

#install the library
remus_install_library(RemusClient)

#install the headers
remus_public_headers(${headers})

#setup the exports for the library when used from a build tree
export(PACKAGE RemusClient)
export(TARGETS RemusClient RemusCommon FILE RemusClient-exports.cmake)
