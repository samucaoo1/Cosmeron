cmake_minimum_required(VERSION 3.16)
project(CosmeronOHOSPortability LANGUAGES C)

if(NOT CMAKE_SYSTEM_NAME STREQUAL "OHOS")
  message(FATAL_ERROR "Use the OpenHarmony native SDK ohos.toolchain.cmake")
endif()

# The C11 manifest is the only suite inventory. Resolve it at configure time
# so this target follows the same selected tests as the native CI runners.
set(COSMERON_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../..")
set(COSMERON_MANIFEST "${COSMERON_ROOT}/.github/tests.txt")
if(NOT EXISTS "${COSMERON_MANIFEST}")
  message(FATAL_ERROR "Missing portable test manifest: ${COSMERON_MANIFEST}")
endif()

file(STRINGS "${COSMERON_MANIFEST}" COSMERON_TEST_ENTRIES)
set(COSMERON_TEST_SOURCES)
foreach(entry IN LISTS COSMERON_TEST_ENTRIES)
  string(STRIP "${entry}" entry)
  if(entry STREQUAL "" OR entry MATCHES "^#")
    continue()
  endif()
  if(NOT entry MATCHES "^([^ \t]+)[ \t]+([^ \t]+)$")
    message(FATAL_ERROR "Invalid .github/tests.txt entry: '${entry}'")
  endif()
  set(module "${CMAKE_MATCH_1}")
  set(test "${CMAKE_MATCH_2}")
  set(source "${COSMERON_ROOT}/Codespace/Tests/${module}/${test}.c")
  if(NOT EXISTS "${source}")
    message(FATAL_ERROR "Manifest source does not exist: ${source}")
  endif()
  list(APPEND COSMERON_TEST_SOURCES "${source}")
endforeach()

add_library(cosmeron_ohos_manifest OBJECT ${COSMERON_TEST_SOURCES})
add_executable(cosmeron_ohos_multi_tu MultiTU_A.c MultiTU_B.c MultiTU_Main.c)

foreach(target cosmeron_ohos_manifest cosmeron_ohos_multi_tu)
  set_target_properties(${target} PROPERTIES
    C_STANDARD 11 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
  target_include_directories(${target} PRIVATE "${COSMERON_ROOT}/Codespace")
  target_compile_options(${target} PRIVATE
    -Wall -Wextra -Wpedantic -Werror -Qunused-arguments -UNDEBUG)
endforeach()

message(STATUS "OHOS target ABI: ${OHOS_ARCH}")
message(STATUS "Portable manifest sources: ${COSMERON_TEST_SOURCES}")
message(STATUS "Compile/link verification only; runtime and device permissions unverified")
