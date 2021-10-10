project(backward-cpp)

set(test_backward_targets test_segfault)

foreach(curr_target ${test_backward_targets})
	add_executable(${curr_target} ${curr_target}.cpp ${BACKWARD_ENABLE})
	add_backward(${curr_target})
endforeach(curr_target)

# installing
install(TARGETS  ${test_backward_targets} DESTINATION ppkMHD/bin/test/backward-cpp/)