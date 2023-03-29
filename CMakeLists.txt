cmake_minimum_required(VERSION 3.22)
project(PixARLang)

set(CMAKE_CXX_STANDARD 17)
Include(FetchContent)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.0.1 # or a later release
)
FetchContent_MakeAvailable(Catch2)

include_directories(src)

add_executable(pixarc
  src/location.hh
  src/lexer.hh
  src/ast.hh
  src/parser.hh
  src/visitor.hh
  src/xml_visitor.hh
  src/xml_printer.hh
  src/lexer.cc
  src/ast.cc
  src/parser.cc
  src/visitor.cc
  src/xml_visitor.cc
  src/main.cc)

add_executable(pixarc_tests
  src/location.hh
  src/lexer.hh
  src/ast.hh
  src/parser.hh
  src/visitor.hh
  src/xml_visitor.hh
  src/xml_printer.hh
  src/lexer.cc
  src/ast.cc
  src/parser.cc
  src/visitor.cc
  src/xml_visitor.cc
  tests/lexer_tests.cc)

target_link_libraries(pixarc_tests PRIVATE Catch2::Catch2WithMain)
