
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(am 64)
else()
  set(am 32)
endif()

#with xcode 5 we don't have gcc anymore on the mac
if(APPLE AND NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(boost_toolset toolset=clang-osx)
endif()

set(boost_with_args
  --with-date_time
  --with-filesystem
  --with-system
  --with-thread
  )

#since we don't specify a prefix for the superbuild,
#we can determine where the buld directory will be. This
#is needed as we need to wrap the build directory in quotes to
#and escape spaces in the path for boost to properly build.
string(REPLACE " " "\\ " boost_build_dir ${CMAKE_CURRENT_BINARY_DIR}/boost/src/boost)
string(REPLACE " " "\\ " boost_install_dir ${install_location})

set(boost_cmds
  CONFIGURE_COMMAND ./bootstrap.sh ${boost_toolset} --prefix=${boost_install_dir}
  BUILD_COMMAND ./b2 ${boost_toolset} --build-dir=${boost_build_dir} address-model=${am} ${boost_with_args}
  INSTALL_COMMAND ./b2 ${boost_toolset} address-model=${am} ${boost_with_args}
    install
  )

add_external_project(boost
  ${boost_cmds}
  BUILD_IN_SOURCE 1
  )

ExternalProject_Get_Property(boost install_dir)
add_project_property(boost BOOST_INCLUDEDIR "${install_dir}/include/boost")
add_project_property(boost BOOST_LIBRARYDIR "${install_dir}/lib")