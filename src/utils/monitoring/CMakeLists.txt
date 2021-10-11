project(monitoring)
find_package(OpenMP)

add_library(${PROJECT_NAME} "")

target_sources(${PROJECT_NAME}
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/OpenMPTimer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/SimpleTimer.cpp
  )

if (PAPI_FOUND)
  target_sources(${PROJECT_NAME}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/PapiInfo.cpp
    )
endif(PAPI_FOUND)

target_include_directories(${PROJECT_NAME}
  PUBLIC
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/src
)


if (PAPI_FOUND)
  target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${PAPI_INCLUDE_DIR})
  target_link_libraries(${PROJECT_NAME}
    PUBLIC
    ${PAPI_LIBRARY})
endif(PAPI_FOUND)

if(OpenMP_CXX_FOUND)
  target_link_libraries(${PROJECT_NAME} PUBLIC OpenMP::OpenMP_CXX)
endif(OpenMP_CXX_FOUND)

#Add an alias so that library can be used inside the build tree
add_library(ppkMHD::${PROJECT_NAME} ALIAS ${PROJECT_NAME})


# installing
install(TARGETS ${PROJECT_NAME} 
    RUNTIME DESTINATION ppkMHD/bin 
    ARCHIVE DESTINATION ppkMHD/lib
    LIBRARY DESTINATION ppkMHD/lib
    )
