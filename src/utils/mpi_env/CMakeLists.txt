project(mpi_env)

set (mpi_env_SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/mpi_env.h
  ${CMAKE_CURRENT_SOURCE_DIR}/mpi_env.cpp
  )

add_library(${PROJECT_NAME} OBJECT
  ${mpi_env_SRCS}
  )

# installing
install(TARGETS ${PROJECT_NAME} 
    RUNTIME DESTINATION ppkMHD/bin 
    ARCHIVE DESTINATION ppkMHD/lib
    LIBRARY DESTINATION ppkMHD/lib
    )
