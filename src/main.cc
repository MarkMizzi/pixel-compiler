#include "compiler.hh"
#include "util.hh"
#include "xml_printer.hh"

#include <exception>
#include <iostream>

int main(int argc, char *argv[]) {

  if (argc <= 1) {
    std::cerr << "Expected filename." << std::endl;
    exit(-1);
  }

  try {
    Compiler compiler{argv[1]};
    codegen::PixIRCode code = compiler.compile();
    codegen::dumpCode(code, std::cout);
  } catch (CompilationError &e) {
    std::cerr << e.what() << std::endl;
    exit(-1);
  }
}
