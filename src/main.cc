#include "compiler.hh"
#include "util.hh"

#include <fstream>
#include <iostream>

void print_usage() {

  const std::string helpMessage =
      "./pixarc [-o <outfile>] [-xml <outfile>] {<options>} [src]\n"
      "Options:\n"
      "  -o                  Specify output file. By default stdout is used.\n"
      "  -xml                Generate XML from the AST produced. An output "
      "file for the XML must also be specified.\n"
      "  -frotate-loops      Rotates while/for loops when generating code.\n"
      "  -felim-dead-code    Eliminate dead code.\n"
      "  -fpeephole-optimize Enable the peephole optimizer.\n"
      "  -h                  Print this help message and exit immediately.\n"
      "Args:\n"
      "  src                 Specifies source file to compile.\n";

  std::cout << helpMessage;
  exit(0);
}

CompilerOptions parseArgs(int argc, char *argv[]) {
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
    } else if (arg == "-frotate-loops") {
      options.rotateLoops = true;
    } else if (arg == "-felim-dead-code") {
      options.eliminateDeadCode = true;
    } else if (arg == "-fpeephole-optimize") {
      options.peepholeOptimize = true;
    } else if (gotSource) {
      std::cerr << "Cannot process more than one source file at a time."
                << std::endl;
      exit(-1);
    } else {
      options.infile = arg;
      gotSource = true;
    }
  }

  return options;
}

int main(int argc, char *argv[]) {

  CompilerOptions options = parseArgs(argc, argv);

  try {
    Compiler compiler{options};
    compiler.compile();
  } catch (CompilationError &e) {
    std::cerr << e.what() << std::endl;
    exit(-1);
  }
}
