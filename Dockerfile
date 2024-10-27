FROM ubuntu:latest

# Update libraries.
RUN apt update && apt upgrade -y
# Install needed libraries.
RUN apt install -y git build-essential libgmp-dev libmpfr-dev libssl-dev gdb cmake
# Clean up.
RUN apt clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

# Download library and extract. (One could use git clone but this is easier for swithing to published versions.
RUN git clone https://github.com/relic-toolkit/relic.git

## Relic library installation.
RUN mkdir /relic/build
WORKDIR "/relic/build"
RUN ../preset/gmp-pbc-bls381.sh ../ && \
    make && \
    make install

# Copy the files over to working directory.
RUN mkdir -p /home/project/build
COPY . /home/project

# Build the project.
WORKDIR "/home/project/build"
RUN cmake ..
RUN make

# Execute the test.
RUN ctest

RUN make install