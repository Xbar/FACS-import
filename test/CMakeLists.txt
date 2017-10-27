FILE(GLOB unit_test_src *.cpp)
ADD_EXECUTABLE(unit_test ${unit_test_src})
target_link_libraries(unit_test gtest_main gtest facs_io)

add_custom_target (tests
  COMMAND ${CMAKE_CURRENT_BINARY_DIR}/unit_test
  DEPENDS libfacs_io)

add_test (NAME unit_tests COMMAND unit_test)
