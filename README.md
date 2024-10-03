# Pixel Language Compiler and Playground

This is an implementation of a compiler for the Pixel Language undertaken for the [CPS2000 Compilers course](https://www.um.edu.mt/courses/studyunit/CPS2000) at the University of Malta.

A specification for the assignment (and partial specification of the langauge implemented) can be found [here](./report/specification.pdf). A report on the work carried out for the assignment can be found [here](./report/report.pdf).

## Building the compiler

THe compiler can be built using CMake. Run:
``` sh
cmake .
```
to obtain a build file for the project.

Then run
``` sh
make
```
to actually build the compiler. A binary called `pixelc` will be produced.

## Using the compiler

``` text
./pixelc [-o <outfile>] [-xml <outfile>] {<options>} [src]
Options:
    -o                  Specify output file. By default stdout is used.
    -xml                Generate XML from the AST produced. An output 
                        file for the XML must also be specified.
    -frotate-loops      Rotates while/for loops when generating code.
    -felim-dead-code    Eliminate dead code.
    -fpeephole-optimize Enable the peephole optimizer.
    -h                  Print this help message and exit immediately.
Args:
    src                 Specifies source file to compile.
```
