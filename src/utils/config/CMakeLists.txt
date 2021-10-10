project(config)

add_library(${PROJECT_NAME} "")

target_sources(${PROJECT_NAME}
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/ConfigMap.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/inih/ini.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/inih/INIReader.cpp
  )

target_include_directories(${PROJECT_NAME}
  PUBLIC
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/src/utils
  ${CMAKE_CURRENT_SOURCE_DIR}/inih ) 

if (USE_MPI)
  target_link_libraries(${PROJECT_NAME} PUBLIC MPI::MPI_CXX)
endif(USE_MPI)

#Add an alias so that library can be used inside the build tree
add_library(ppkMHD::${PROJECT_NAME} ALIAS ${PROJECT_NAME})


# installing
FILE(GLOB currHeaders1 *.h)
FILE(GLOB currHeaders2 inih/*.h)
set(currHeaders ${currHeaders1} ${currHeaders2})
target_include_directories(${PROJECT_NAME} PUBLIC $$<INSTALL_INTERFACE:ppkMHD/include)
install(TARGETS ${PROJECT_NAME} 
    RUNTIME DESTINATION ppkMHD/bin 
    ARCHIVE DESTINATION ppkMHD/lib
    LIBRARY DESTINATION ppkMHD/lib
    INCLUDES DESTINATION ppkMHD/include
    )

install(FILES  ${currHeaders} DESTINATION ppkMHD/include)
