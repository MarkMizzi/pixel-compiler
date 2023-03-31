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
    for (const std::unique_ptr<codegen::PixIRFunction> &func :
         compiler.compile()) {
      std::cout << func->funcName << std::endl;
      for (const std::unique_ptr<codegen::BasicBlock> &block : func->blocks) {
        for (const codegen::PixIRInstruction &instr : block->instrs) {
          std::cout << instr.to_string() << std::endl;
        }
      }
    }
  } catch (CompilationError &e) {
    std::cerr << e.what() << std::endl;
    exit(-1);
  }
}
