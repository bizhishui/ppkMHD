project(mood)

add_library(${PROJECT_NAME} "")

target_sources(${PROJECT_NAME}
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/monomials_ordering.h
  ${CMAKE_CURRENT_SOURCE_DIR}/monomials_print_utils.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Polynomial.h
  ${CMAKE_CURRENT_SOURCE_DIR}/polynomials_utils.h
  ${CMAKE_CURRENT_SOURCE_DIR}/polynomials_utils.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/MonomialMap.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Stencil.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Stencil.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/StencilUtils.h
  ${CMAKE_CURRENT_SOURCE_DIR}/StencilUtils.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/GeometricTerms.h
  ${CMAKE_CURRENT_SOURCE_DIR}/GeometricTerms.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Matrix.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Matrix.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/mood_utils.h
  ${CMAKE_CURRENT_SOURCE_DIR}/mood_utils.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/MoodBaseFunctor.h
  ${CMAKE_CURRENT_SOURCE_DIR}/MoodBaseQuad.h
  ${CMAKE_CURRENT_SOURCE_DIR}/MoodBaseQuad.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/MoodPolynomialReconstructionFunctors.h
  ${CMAKE_CURRENT_SOURCE_DIR}/MoodFluxesFunctors.h
  ${CMAKE_CURRENT_SOURCE_DIR}/MoodInitFunctors.h
  ${CMAKE_CURRENT_SOURCE_DIR}/MoodUpdateFunctors.h
  ${CMAKE_CURRENT_SOURCE_DIR}/SolverHydroMood.h
  )

target_compile_options(${PROJECT_NAME}
  PUBLIC ${KOKKOS_CXX_FLAGS}
  )
target_include_directories(${PROJECT_NAME}
  PUBLIC
  ${CMAKE_SOURCE_DIR}/src
  ${LAPACKE_INCLUDE_DIR}
  )
target_link_libraries(${PROJECT_NAME}
  PUBLIC
  kokkos
  ${LAPACKE_LIBRARIES}
  ${OpenBLAS_LIB}
  )

  add_library(ppkMHD::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# # installing
install(TARGETS ${PROJECT_NAME} 
    RUNTIME DESTINATION ppkMHD/bin 
    ARCHIVE DESTINATION ppkMHD/lib
    LIBRARY DESTINATION ppkMHD/lib
    )