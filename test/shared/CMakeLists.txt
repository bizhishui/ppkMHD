#
# SHARED related tests (src/shared)
#



project(test_shared)

set(ppk_LIBRARIES kokkos dl)

set(test_shared_targets test_euler_eigen_decomposition)

foreach(curr_target ${test_shared_targets})
	add_executable(${curr_target} ${curr_target}.cpp)
	target_include_directories(${curr_target}  PUBLIC  ${CMAKE_SOURCE_DIR}/src)   #to verify
	foreach(CURRL ${ppk_LIBRARIES})
		target_link_libraries(${curr_target} PUBLIC ${CURRL})
	endforeach(CURRL)
  if (USE_MPI)
    target_link_libraries(${curr_target} PUBLIC MPI::MPI_CXX)
  endif(USE_MPI)
endforeach(curr_target)

# installing
install(TARGETS  ${test_shared_targets} DESTINATION ppkMHD/bin/test/shared/)
