# Pre-requisites

Install & build the boost C++ library.

# Build

    mkdir build
    cd build
    cmake ..
    make

This will generate an executable called `myo-server`.

# Execute

In the build directory call

    ./myo-server

This starts the myo server on localhost:2000.

# Test

To start a simple client for the grab server that just prints whatever it receives call

    ruby test-client.rb
