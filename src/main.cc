#include "xml_printer.hh"

#include <exception>
#include <iostream>

int main(int argc, char *argv[]) {

  if (argc <= 1) {
    std::cerr << "Expected filename." << std::endl;
    exit(-1);
  }

  try {
    XMLPrinter xmlp{argv[1]};
    std::cout << xmlp.xml() << std::endl;
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
    exit(-1);
  }
}
