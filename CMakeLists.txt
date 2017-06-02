cmake_minimum_required(VERSION 3.0)
project(drafter)
set(CMAKE_BINARY_DIR build)

set(CMAKE_CXX_STANDARD 11)

include_directories(
  "ext/snowcrash/ext/markdown-parser/ext/sundown/src"
  "ext/snowcrash/ext/markdown-parser/src"
  "ext/snowcrash/src"
  "ext/sos/src"
  "ext/cmdline"
  "src/refract"
  "src"
)

# Catch
set(CATCH_INCLUDE_DIR test/vendor/catch/include)
add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE ${CATCH_INCLUDE_DIR})

# DTL
set(DTL_INCLUDE_DIR test/vendor/dtl/dtl)
add_library(dtl INTERFACE)
target_include_directories(dtl INTERFACE ${DTL_INCLUDE_DIR})

# Sundown
file(GLOB SUNDOWN_SRCS
  "ext/snowcrash/ext/markdown-parser/ext/sundown/src/*.c"
  "ext/snowcrash/ext/markdown-parser/ext/sundown/src/*.h")
add_library(sundown ${SUNDOWN_SRCS})

# Markdown Parser
file(GLOB MARKDOWN_SRCS
  "ext/snowcrash/ext/markdown-parser/src/*.cc"
  "ext/snowcrash/ext/markdown-parser/src/*.h")
add_library(markdown-parser ${MARKDOWN_SRCS})
add_dependencies(markdown-parser sundown)
target_link_libraries(markdown-parser sundown)

# Snowcrash
file(GLOB SNOWCRASH_SRCS
  "ext/snowcrash/src/*.cc"
  "ext/snowcrash/src/*.h")

if(WIN32)
  list(APPEND SNOWCRASH_SRCS ext/snowcrash/src/win/RegexMatch.cc)
else()
  list(APPEND SNOWCRASH_SRCS ext/snowcrash/src/posix/RegexMatch.cc)
endif()

add_library(snowcrash ${SNOWCRASH_SRCS})
add_dependencies(snowcrash markdown-parser)
target_link_libraries(snowcrash markdown-parser)

# SOS
add_library(sos
  ext/sos/src/sos.cc
  ext/sos/src/sos.h
  ext/sos/src/sosYAML.h
  ext/sos/src/sosJSON.h)

# Drafter
file(GLOB DRAFTER_SRCS
  "src/*.cc"
  "src/*.h"
  "src/refract/*.cc"
  "src/refract/*.h")
list(REMOVE_ITEM DRAFTER_SRCS src/main.cc)
add_library(libdrafter ${DRAFTER_SRCS})
add_dependencies(libdrafter snowcrash sos)
target_link_libraries(libdrafter snowcrash sos)

# Drafter Tests
file(GLOB TEST_SRCS "test/*.cc")
add_executable(test-libdrafter ${TEST_SRCS})
target_link_libraries(test-libdrafter libdrafter Catch dtl)

# Drafter C Tests
add_executable(test-capi "test/test-CAPI.c")
target_link_libraries(test-capi libdrafter)

enable_testing(true)
add_test(ctests ctests)
add_test(tests tests)

# Drafter CLI
add_executable(drafter src/main.cc)
target_link_libraries(drafter libdrafter snowcrash sos markdown-parser sundown)

install(TARGETS drafter DESTINATION bin)
install(FILES src/drafter.h DESTINATION include/drafter)
