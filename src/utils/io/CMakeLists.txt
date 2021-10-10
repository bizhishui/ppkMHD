project(io)

add_library(${PROJECT_NAME} "")

target_sources (${PROJECT_NAME}
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/IO_common.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/IO_ReadWrite.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/IO_VTK.cpp
  )

if(USE_SDM)
  target_sources (${PROJECT_NAME}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/IO_ReadWrite_SDM.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/IO_VTK_SDM_shared.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/IO_VTK_SDM.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/IO_VTK_SDM_Flux.cpp
    )
endif(USE_SDM)

if(USE_HDF5)
  target_sources (${PROJECT_NAME}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/IO_HDF5.cpp
    )
endif(USE_HDF5)

if(USE_MPI)
  if(USE_PNETCDF)
    target_sources (${PROJECT_NAME}
      PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}/IO_PNETCDF.h
      )    
  endif(USE_PNETCDF)
endif(USE_MPI)

target_include_directories(${PROJECT_NAME}
  PUBLIC
  ${CMAKE_SOURCE_DIR}/src
  )

target_link_libraries(${PROJECT_NAME}
  PUBLIC
  kokkos shared
  )

if (USE_HDF5)
  target_link_libraries(${PROJECT_NAME}
    PUBLIC
    ${HDF5_LIBRARIES}
    )
endif(USE_HDF5)

if (USE_PNETCDF)
  target_link_libraries(${PROJECT_NAME}
    PUBLIC
    ${PNETCDF_LIBRARIES}
    )
endif(USE_PNETCDF)

if (USE_MPI)
  target_link_libraries(${PROJECT_NAME} PUBLIC MPI::MPI_CXX)
endif(USE_MPI)

#Add an alias so that library can be used inside the build tree
add_library(ppkMHD::${PROJECT_NAME} ALIAS ${PROJECT_NAME})


# installing
FILE(GLOB currHeaders *.h)
target_include_directories(${PROJECT_NAME} PUBLIC $$<INSTALL_INTERFACE:ppkMHD/include)
install(TARGETS ${PROJECT_NAME} 
    RUNTIME DESTINATION ppkMHD/bin 
    ARCHIVE DESTINATION ppkMHD/lib
    LIBRARY DESTINATION ppkMHD/lib
    INCLUDES DESTINATION ppkMHD/include
    )

install(FILES  ${currHeaders} DESTINATION ppkMHD/include)
