set(target_name libcsc)

add_library(
  ${target_name} INTERFACE
  libcsc/hashmap/HashMap.hpp
)

target_include_directories(
  ${target_name}
  INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)
