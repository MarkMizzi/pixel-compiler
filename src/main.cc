#include "compiler.hh"
#include "util.hh"

#include <fstream>
#include <iostream>

void print_usage() {
  std::cout << "./pixarc [-o <outfile>] [-xml <outfile>] {<options>} [src]"
            << std::endl;
  std::cout << "Options:" << std::endl;
  std::cout << "  -o    Specify output file. By default stdout is used."
            << std::endl;
  std::cout << "  -xml  Generate XML from the AST produced. An output file for "
               "the XML must also be specified."
            << std::endl;
  std::cout << "  -felim-dead-code  Eliminate dead code." << std::endl;
  std::cout << "  -h    Print this help message and exit immediately."
            << std::endl;
  std::cout << "Args:" << std::endl;
  std::cout << "  src   Specifies source file to compile." << std::endl;
  exit(0);
}

int main(int argc, char *argv[]) {

  CompilerOptions options;

  // arg processing
  bool gotSource = false;
  for (int i = 1; i < argc; i++) {
    std::string arg{std::move(argv[i])};
    if (arg == "-h") {
      print_usage();
    } else if (arg == "-o") {
      i++;
      if (i >= argc) {
        std::cerr << "Expected filename for output." << std::endl;
        exit(-1);
      }
      options.outfile = std::string(std::move(argv[i]));
    } else if (arg == "-xml") {
      options.generateXml = true;
      i++;
      if (i >= argc) {
        std::cerr << "Expected filename for XML output." << std::endl;
        exit(-1);
      }
      options.xmlOutfile = std::string(std::move(argv[i]));
    } else if (arg == "-felim-dead-code") {
      options.eliminateDeadCode = true;
    } else if (gotSource) {
      std::cerr << "Cannot process more than one source file at a time."
                << std::endl;
      exit(-1);
    } else {
      options.infile = arg;
      gotSource = true;
    }
  }

  try {
    Compiler compiler{options};
    compiler.compile();
  } catch (CompilationError &e) {
    std::cerr << e.what() << std::endl;
    exit(-1);
  }
}
