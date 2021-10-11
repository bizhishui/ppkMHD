project(muscl)

add_library(${PROJECT_NAME} "")

target_sources (${PROJECT_NAME}
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/hydro_shared.h
  ${CMAKE_CURRENT_SOURCE_DIR}/HydroBaseFunctor2D.h
  ${CMAKE_CURRENT_SOURCE_DIR}/HydroBaseFunctor3D.h
  ${CMAKE_CURRENT_SOURCE_DIR}/HydroRunFunctors2D.h
  ${CMAKE_CURRENT_SOURCE_DIR}/HydroRunFunctors3D.h
  ${CMAKE_CURRENT_SOURCE_DIR}/HydroInitFunctors2D.h
  ${CMAKE_CURRENT_SOURCE_DIR}/HydroInitFunctors3D.h
  ${CMAKE_CURRENT_SOURCE_DIR}/MHDBaseFunctor2D.h
  ${CMAKE_CURRENT_SOURCE_DIR}/MHDBaseFunctor3D.h
  ${CMAKE_CURRENT_SOURCE_DIR}/MHDRunFunctors2D.h
  ${CMAKE_CURRENT_SOURCE_DIR}/MHDRunFunctors3D.h
  ${CMAKE_CURRENT_SOURCE_DIR}/MHDInitFunctors2D.h
  ${CMAKE_CURRENT_SOURCE_DIR}/MHDInitFunctors3D.h
  ${CMAKE_CURRENT_SOURCE_DIR}/SolverHydroMuscl.h
  ${CMAKE_CURRENT_SOURCE_DIR}/SolverHydroMuscl.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/SolverMHDMuscl.h
  ${CMAKE_CURRENT_SOURCE_DIR}/SolverMHDMuscl.cpp
  )

target_include_directories(${PROJECT_NAME}
  PUBLIC
  ${CMAKE_SOURCE_DIR}/src
  )

target_link_libraries(${PROJECT_NAME}
  PUBLIC
  kokkos
  )

if(USE_MPI)
  target_link_libraries(${PROJECT_NAME} PUBLIC ppkMHD::mpiUtils)
endif(USE_MPI)

#Add an alias so that library can be used inside the build tree
add_library(ppkMHD::${PROJECT_NAME} ALIAS ${PROJECT_NAME})


# installing
install(TARGETS ${PROJECT_NAME} 
    RUNTIME DESTINATION ppkMHD/bin 
    ARCHIVE DESTINATION ppkMHD/lib
    LIBRARY DESTINATION ppkMHD/lib
    )
