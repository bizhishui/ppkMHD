#
# Kokkos test example : query device information
#
project(test_kokkos)

set(ppk_LIBRARIES kokkos)

set(test_kokkos_targets query_device_kokkos)

foreach(curr_target ${test_kokkos_targets})
	add_executable(${curr_target} ${curr_target}.cpp)
	foreach(CURRL ${ppk_LIBRARIES})
		target_include_directories(${curr_target} PRIVATE $<TARGET_PROPERTY:${CURRL},INTERFACE_INCLUDE_DIRECTORIES>)
		target_link_libraries(${curr_target} PUBLIC ${CURRL})
	endforeach(CURRL)
  if (USE_MPI)
    target_link_libraries(${curr_target} PUBLIC MPI::MPI_CXX)
  endif(USE_MPI)

endforeach(curr_target)

# installing
install(TARGETS  ${test_kokkos_targets} DESTINATION ppkMHD/bin/test/kokkos/)

