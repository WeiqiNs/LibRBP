FROM ubuntu:latest

# Update libraries.
RUN apt update && apt upgrade -y
# Install needed libraries.
RUN apt install -y git build-essential libgmp-dev libmpfr-dev libssl-dev libgtest-dev gdb cmake
# Clean up.
RUN apt clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

# Download library and extract. (One could use git clone but this is easier for swithing to published versions.
RUN git clone https://github.com/relic-toolkit/relic.git

## Relic library installation.
RUN mkdir /relic/build
WORKDIR "/relic/build"
RUN ../preset/gmp-pbc-bls381.sh .. && make && make install

# Copy the files over to working directory.
RUN mkdir -p /home/LibRBP/build
COPY . /home/LibRBP

# Build the project, run tests and install.
WORKDIR "/home/LibRBP/build"
RUN cmake .. && make && ctest && make install

# Build the demo to make sure it works.
RUN mkdir -p /home/LibRBP/demo/build
WORKDIR "/home/LibRBP/demo/build"
RUN cmake .. && make

# Run the demo and keep container running.
CMD ["/bin/sh", "-c", "./demo && tail -f /dev/null"]
