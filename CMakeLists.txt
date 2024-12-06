cmake_minimum_required(VERSION 3.22)
project(PixARLang)

set(CMAKE_CXX_STANDARD 17)
Include(FetchContent)

# otherwise clangd complains
set(CMAKE_CXX_FLAGS "-std=c++17")

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.0.1 # or a later release
)
FetchContent_MakeAvailable(Catch2)

include_directories(src)

set(SRC_FILES
  src/util.hh
  src/location.hh
  src/lexer.hh
  src/parser.hh
  src/ast.hh
  src/visitor.hh
  src/xml_visitor.hh
  src/semantic_visitor.hh
  src/codegen.hh
  src/deadcode.hh
  src/peephole.hh
  src/compiler.hh
  src/lexer.cc
  src/parser.cc
  src/visitor.cc
  src/xml_visitor.cc
  src/semantic_visitor.cc
  src/codegen.cc
  src/deadcode.cc
  src/peephole.cc
)

add_executable(pixelc
  ${SRC_FILES}
  src/main.cc)

add_executable(pixelc_tests
  ${SRC_FILES}
  tests/lexer_tests.cc
  tests/semantic_visitor_tests.cc)

target_link_libraries(pixelc_tests PRIVATE Catch2::Catch2WithMain)
