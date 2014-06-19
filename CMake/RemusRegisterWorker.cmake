#=========================================================================
#
#  This software is distributed WITHOUT ANY WARRANTY; without even
#  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE.  See the above copyright notice for more information.
#
#=========================================================================


#we create a text file that list this mesh worker and its type
function(Register_Mesh_Worker WorkerExecutableName InputMeshFileType OutputMeshType)
  message(STATUS "Register_Mesh_Worker called ${EXECUTABLE_OUTPUT_PATH}/${WorkerExecutableName}.rw")
  configure_file(${Remus_SOURCE_DIR}/CMake/RemusWorker.rw.in
           ${EXECUTABLE_OUTPUT_PATH}/${WorkerExecutableName}.rw
            @ONLY)
endfunction()
