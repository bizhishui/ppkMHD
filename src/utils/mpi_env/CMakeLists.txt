project(mpi_env)

set (mpi_env_SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/mpi_env.h
  ${CMAKE_CURRENT_SOURCE_DIR}/mpi_env.cpp
  )

add_library(${PROJECT_NAME} OBJECT
  ${mpi_env_SRCS}
  )

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