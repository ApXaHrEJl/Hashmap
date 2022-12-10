set(target_name csc_test)

add_executable(
  ${target_name}
  libcsc/libcsc/HashMap.cpp
)

include(CompileOptions)
set_compile_options(${target_name})

target_link_libraries(
  ${target_name}
  PRIVATE
    libcsc
    gtest_main
)

include(GoogleTest)
gtest_discover_tests(${target_name})
