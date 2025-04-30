# A sample file to show how to build and install the LibRBP and use it in another project.
FROM ubuntu:latest

# Perform apt update and upgrade.
RUN apt update && apt upgrade -y
# Reindex and install needed libraries.
RUN apt update && apt install -y git gdb cmake build-essential libgmp-dev libmpfr-dev libssl-dev libgtest-dev
# Clean up.
RUN apt clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

# Download the RELIC library and the RBP library.
RUN git clone https://github.com/relic-toolkit/relic.git
RUN git clone https://github.com/WeiqiNs/LibRBP.git

# RELIC library installation.
RUN mkdir /relic/build
WORKDIR "/relic/build"
RUN ../preset/gmp-pbc-bls381.sh .. && cmake --build . --parallel && cmake --install .

# RBP library installation.
WORKDIR "/LibRBP"
RUN cmake -B build -S . && cmake --build build --parallel && cmake --install build

# Build the demo to make sure it works.
WORKDIR "/LibRBP/demo"
RUN cmake -B build -S . && cmake --build build --parallel

# Run the demo and keep container running.
CMD ["/bin/sh", "-c", "./build/demo && tail -f /dev/null"]
