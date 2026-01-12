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

## Running the playground locally

If you want to run the playground locally, you can do so by running
``` bash
./scripts/run.bash
```
This will build and start the Docker image for the playground, which will be available at http://localhost:8080.

## Hosting your own copy of the playground

Hosting a copy of the playground is more work intensive unless you own the domain `pixel.markmizzi.dev` (which at the time of writing belongs to me).

Change the hardcoded references to `pixel.markmizzi.dev` in `nginx.conf` and `scripts/deploy.bash` to a domain name you own (a simple search and replace will do). Also remember to change my email `mizzimark2001@gmail.com` to one of your own in the call to `certbot` within `scripts/deploy.bash`.

Once all this is done, simply run
``` bash
./scripts/deploy.bash
```
on the machine where you want to host the playground.

## Showcase

The Pixel playgrond has a code editor with syntax highlighting, and is also capable of displaying the compiled bytecode of a program:
<img width="1798" height="917" alt="image" src="https://github.com/user-attachments/assets/afe60c93-e421-4adc-acec-c8d7263c9425" />

In addition, you can also view the parsed syntax tree for a program:
<img width="1798" height="917" alt="image" src="https://github.com/user-attachments/assets/4b3e1404-6cc1-4de3-9162-205e652b1d63" />

A compiled program can be executed in the virtual machine:
<img width="1791" height="929" alt="Screenshot from 2026-01-12 22-56-43" src="https://github.com/user-attachments/assets/43b859ff-0d90-41b2-be1c-58cf4317973e" />

Below you can see execution of some sample programs.

### Double Pendulum

![double-pendulum](https://github.com/user-attachments/assets/eb4d01fb-46b9-4f50-bab1-d21ea999bc6c)

This one was quite challenging to implement as it required programming $\sin(x)$ and $\cos(x)$ functions from scratch:
<img width="1791" height="929" alt="Screenshot from 2026-01-12 22-56-02" src="https://github.com/user-attachments/assets/d0a63c26-926e-4673-b56a-173e60735555" />

### Quicksort

![quicksort](https://github.com/user-attachments/assets/d46528ae-2824-4cdb-b156-2b75a60c7eef)

### Game of Life

![gameoflife](https://github.com/user-attachments/assets/5e84a75c-6ae8-4cb7-995f-6543c61f21f9)

### Lorentz Attractor

![lorentz](https://github.com/user-attachments/assets/343c0413-d780-4e61-8c15-a63fba2cc14b)
