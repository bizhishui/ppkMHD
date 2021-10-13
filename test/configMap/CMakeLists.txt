#
# ConfigMap class test (src/util/config)
#

##############################################
project(configMap)

set(test_configMap_target ConfigMapTest)

add_executable(${test_configMap_target}
  ConfigMapTest.cpp)

target_link_libraries(${test_configMap_target}
  PRIVATE
  ppkMHD::config)

##############################################
if(USE_MPI)
  
  configure_file(test_mpi.ini test_mpi.ini COPYONLY)

  add_executable(ConfigMapTest_mpi
    ConfigMapTest_mpi.cpp)

  target_link_libraries(ConfigMapTest_mpi
    PRIVATE
    ppkMHD::config
    ppkMHD::mpiUtils)
  
endif(USE_MPI)


# installing
install(TARGETS  ${test_configMap_target} DESTINATION ppkMHD/bin/test/configMap/)
