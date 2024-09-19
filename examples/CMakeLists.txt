add_executable(wincpp_test)
set_target_properties(wincpp_test PROPERTIES OUTPUT_NAME "test")
target_sources(wincpp_test PRIVATE "test.cpp")
target_link_libraries(wincpp_test PRIVATE wincpp)