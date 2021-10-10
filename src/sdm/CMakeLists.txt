# sdm is a header only library
project(sdm)

add_library(${PROJECT_NAME} INTERFACE)

# INTERFACE library requires no source arguments
# target_sources(sdm
#   INTERFACE
#   ${CMAKE_CURRENT_SOURCE_DIR}/sdm_shared.h
#   ${CMAKE_CURRENT_SOURCE_DIR}/SDM_Geometry.h
#   ${CMAKE_CURRENT_SOURCE_DIR}/SDMBaseFunctor.h
#   ${CMAKE_CURRENT_SOURCE_DIR}/SDM_Boundaries_Functors.h
#   ${CMAKE_CURRENT_SOURCE_DIR}/SDM_Boundaries_Functors_Wedge.h
#   ${CMAKE_CURRENT_SOURCE_DIR}/SDM_Boundaries_Functors_Jet.h
#   ${CMAKE_CURRENT_SOURCE_DIR}/SDM_Compute_error.h
#   ${CMAKE_CURRENT_SOURCE_DIR}/SDM_Dt_Functors.h
#   ${CMAKE_CURRENT_SOURCE_DIR}/SDM_Flux_Functors.h
#   ${CMAKE_CURRENT_SOURCE_DIR}/SDM_Flux_with_Limiter_Functors.h
#   ${CMAKE_CURRENT_SOURCE_DIR}/SDM_Interpolate_Functors.h
#   ${CMAKE_CURRENT_SOURCE_DIR}/SDM_Limiter_Functors.h
#   ${CMAKE_CURRENT_SOURCE_DIR}/SDM_Positivity_preserving.h
#   ${CMAKE_CURRENT_SOURCE_DIR}/SDM_Run_Functors.h
#   ${CMAKE_CURRENT_SOURCE_DIR}/HydroInitFunctors.h
#   ${CMAKE_CURRENT_SOURCE_DIR}/SolverHydroSDM.h
#   )

target_include_directories(${PROJECT_NAME}
  INTERFACE
  ${CMAKE_SOURCE_DIR}/src
  )
target_link_libraries(${PROJECT_NAME}
  INTERFACE
  kokkos
  )

add_library(ppkMHD::${PROJECT_NAME} ALIAS ${PROJECT_NAME})


# installing
FILE(GLOB currHeaders1 *.h)
FILE(GLOB currHeaders2 init/*.h)
install(TARGETS ${PROJECT_NAME} 
    RUNTIME DESTINATION ppkMHD/bin 
    ARCHIVE DESTINATION ppkMHD/lib
    LIBRARY DESTINATION ppkMHD/lib
    INCLUDES DESTINATION ppkMHD/include
    )

install(FILES  ${currHeaders1} DESTINATION ppkMHD/include)
install(FILES  ${currHeaders2} DESTINATION ppkMHD/include/init)
