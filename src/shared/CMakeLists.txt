#######################
### SHARED ############
#######################
project(shared)

add_library(${PROJECT_NAME} "")
target_sources (${PROJECT_NAME}
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/problems/BlastParams.h
  ${CMAKE_CURRENT_SOURCE_DIR}/problems/DiskParams.h
  ${CMAKE_CURRENT_SOURCE_DIR}/problems/FieldLoopParams.h
  ${CMAKE_CURRENT_SOURCE_DIR}/problems/GreshoParams.h
  ${CMAKE_CURRENT_SOURCE_DIR}/problems/ImplodeParams.h
  ${CMAKE_CURRENT_SOURCE_DIR}/problems/initRiemannConfig2d.h
  ${CMAKE_CURRENT_SOURCE_DIR}/problems/IsentropicVortexParams.h
  ${CMAKE_CURRENT_SOURCE_DIR}/problems/JetParams.h
  ${CMAKE_CURRENT_SOURCE_DIR}/problems/KHParams.h
  ${CMAKE_CURRENT_SOURCE_DIR}/problems/PointSourceGravity.h
  ${CMAKE_CURRENT_SOURCE_DIR}/problems/RayleighTaylorInstabilityParams.h
  ${CMAKE_CURRENT_SOURCE_DIR}/problems/RisingBubbleParams.h
  ${CMAKE_CURRENT_SOURCE_DIR}/problems/RotorParams.h
  ${CMAKE_CURRENT_SOURCE_DIR}/problems/WaveParams.h
  ${CMAKE_CURRENT_SOURCE_DIR}/problems/WedgeParams.h
  ${CMAKE_CURRENT_SOURCE_DIR}/BoundariesFunctors.h
  ${CMAKE_CURRENT_SOURCE_DIR}/BoundariesFunctorsWedge.h
  ${CMAKE_CURRENT_SOURCE_DIR}/HydroParams.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/HydroParams.h
  ${CMAKE_CURRENT_SOURCE_DIR}/HydroState.h
  ${CMAKE_CURRENT_SOURCE_DIR}/kokkos_shared.h
  ${CMAKE_CURRENT_SOURCE_DIR}/real_type.h
  ${CMAKE_CURRENT_SOURCE_DIR}/enums.h
  ${CMAKE_CURRENT_SOURCE_DIR}/SolverBase.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/SolverBase.h
  ${CMAKE_CURRENT_SOURCE_DIR}/RiemannSolvers.h
  ${CMAKE_CURRENT_SOURCE_DIR}/RiemannSolvers_MHD.h
  ${CMAKE_CURRENT_SOURCE_DIR}/utils.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/utils.h
  ${CMAKE_CURRENT_SOURCE_DIR}/mhd_utils.h
  ${CMAKE_CURRENT_SOURCE_DIR}/solver_utils.h
  )

target_link_libraries(${PROJECT_NAME}
  PUBLIC
  kokkos
  ppkMHD::config
  ppkMHD::io
  )
if(USE_MPI)
  target_link_libraries(${PROJECT_NAME}
  PUBLIC
  ppkMHD::mpiUtils)
endif(USE_MPI)

#Add an alias so that library can be used inside the build tree
add_library(ppkMHD::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# installing
FILE(GLOB currHeaders1 *.h)
FILE(GLOB currHeaders2 problems/*.h)
set(currHeaders ${currHeaders1} ${currHeaders2})
target_include_directories(${PROJECT_NAME} PUBLIC $$<INSTALL_INTERFACE:ppkMHD/include)
install(TARGETS ${PROJECT_NAME} 
    RUNTIME DESTINATION ppkMHD/bin 
    ARCHIVE DESTINATION ppkMHD/lib
    LIBRARY DESTINATION ppkMHD/lib
    INCLUDES DESTINATION ppkMHD/include
    )

install(FILES  ${currHeaders} DESTINATION ppkMHD/include)


#######################
### SOLVER FACTORY ####
#######################
project(solver_factory)
add_library(${PROJECT_NAME} "")
target_sources (${PROJECT_NAME}
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/SolverFactory.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/SolverFactory.h
  )
target_include_directories(${PROJECT_NAME}
  PUBLIC
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/src/muscl
)
target_link_libraries(${PROJECT_NAME}
  PUBLIC
  kokkos
  )
if(USE_MPI)
  target_link_libraries(${PROJECT_NAME}
  PUBLIC
  ppkMHD::mpiUtils)
endif(USE_MPI)

#Add an alias so that library can be used inside the build tree
add_library(ppkMHD::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# installing
FILE(GLOB currHeaders0 *.h)
target_include_directories(${PROJECT_NAME} PUBLIC $$<INSTALL_INTERFACE:ppkMHD/include)
install(TARGETS ${PROJECT_NAME} 
    RUNTIME DESTINATION ppkMHD/bin 
    ARCHIVE DESTINATION ppkMHD/lib
    LIBRARY DESTINATION ppkMHD/lib
    INCLUDES DESTINATION ppkMHD/include
    )

install(FILES  ${currHeaders0} DESTINATION ppkMHD/include)