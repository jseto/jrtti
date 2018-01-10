add_executable( Test test_jrtti.cpp sample.cpp )
target_link_libraries(Test gtest_main)
add_executable( Sample run_sample.cpp sample.cpp )
# Now simply link against gtest or gtest_main as needed. Eg
add_test(NAME test_jrtti COMMAND example)
