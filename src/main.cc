#include "xml_printer.hh"

#include <iostream>

int main(int argc, char *argv[]) {

  if (argc <= 1) {
    std::cerr << "Expected filename." << std::endl;
    exit(-1);
  }

  XMLPrinter xmlp{argv[1]};
  std::cout << xmlp.xml() << std::endl;
}
