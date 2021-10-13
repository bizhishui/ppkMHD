configure_file(test_io_2d.ini test_io_2d.ini COPYONLY)
configure_file(test_io_3d.ini test_io_3d.ini COPYONLY)

################################
# VTK test
################################
project(test_io_vtk)
add_executable(${PROJECT_NAME} "")

target_sources(${PROJECT_NAME}
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/test_io_vtk.cpp)

# target_include_directories(test_io_vtk
#   PUBLIC
#   ${CMAKE_SOURCE_DIR}/src
#   ${CMAKE_SOURCE_DIR}/src/utils/io
#   ${CMAKE_SOURCE_DIR}/src/utils/mpiUtils
#   )
target_link_libraries(${PROJECT_NAME}
  PUBLIC
  kokkos 
  ppkMHD::shared 
  ppkMHD::config 
  ppkMHD::monitoring 
  ppkMHD::io)

if(USE_MPI)
  target_link_libraries(${PROJECT_NAME}
    PUBLIC
    ppkMHD::mpiUtils)
endif(USE_MPI)

# installing
install(TARGETS  ${PROJECT_NAME} DESTINATION ppkMHD/bin/test/io/)
###############################
# HDF5 test
###############################
project(test_io_hdf5)
if(USE_HDF5)
  add_executable(${PROJECT_NAME}
    test_io_hdf5.cpp
    #${BACKWARD_ENABLE}
    )
  
  target_link_libraries(${PROJECT_NAME}
    PUBLIC
    kokkos 
    ppkMHD::shared 
    ppkMHD::config 
    ppkMHD::monitoring 
    ppkMHD::io 
    ${HDF5_LIBRARIES})
  
  if(USE_MPI)
    target_link_libraries(${PROJECT_NAME}
      PUBLIC
      ppkMHD::mpiUtils)
  endif(USE_MPI)

  #add_backward(test_io_hdf5)

  # installing
  install(TARGETS  ${PROJECT_NAME} DESTINATION ppkMHD/bin/test/io/)
  
endif(USE_HDF5)

###############################
# PNETCDF test
###############################
project(test_io_pnetcdf)
if(USE_MPI)
  if(USE_PNETCDF)

    add_executable(${PROJECT_NAME}
      test_io_pnetcdf.cpp
      #${BACKWARD_ENABLE}
      )
    
    target_link_libraries(${PROJECT_NAME}
      PUBLIC
      kokkos
      ppkMHD::shared
      ppkMHD::config
      ppkMHD::monitoring
      ppkMHD::io
      ppkMHD::mpiUtils
      ${PNETCDF_LIBRARIES})
    
    #add_backward(test_io_pnetcdf)

    # installing
    install(TARGETS  ${PROJECT_NAME} DESTINATION ppkMHD/bin/test/io/)
    
  endif(USE_PNETCDF)
endif(USE_MPI)