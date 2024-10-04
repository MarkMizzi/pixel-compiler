FROM ubuntu:24.04

# Install system deps
RUN apt-get update
RUN apt-get install -y nodejs npm cmake

### Setting up compiler binaries

# Copy compiler source to the docker container
WORKDIR /
COPY src src
COPY tests tests
COPY CMakeLists.txt CMakeLists.txt

# Build the compiler
RUN cmake -DCMAKE_BUILD_TYPE=Release .
RUN make pixelc

# Remove detritus from the build.
RUN rm -rf src tests CMakeLists.txt \
    Makefile CMakeCache.txt cmake_install.cmake CMakeFiles generated-includes _deps

# Remove cmake as it is no longer needed.
RUN apt-get remove -y cmake
RUN apt-get autoremove -y

### Setting up playground

# Copy playground src to container
COPY playground playground
COPY vm vm

WORKDIR /playground

# Install deps and build playground app
RUN npm install
RUN npm run build

# Clean up typescript files as these are not needed in build.
WORKDIR /
RUN rm **/*.ts

# Remove npm as it is no longer needed.
RUN apt-get remove -y npm
RUN apt-get autoremove -y

# Command to run the playground app, appropriate port is also exposed.
WORKDIR /playground
EXPOSE 8080
CMD ["node", "server/main.js"]