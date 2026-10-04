FROM ubuntu:latest AS deps

RUN apt update && apt install -y git gdb cmake build-essential libgmp-dev libgtest-dev lcov \
    && apt clean && rm -rf /var/lib/apt/lists/*

FROM deps AS librbp

ARG RELIC_GIT_TAG
COPY . /LibRBP
WORKDIR /LibRBP
RUN cmake -B build -S . -DCMAKE_BUILD_TYPE=Release ${RELIC_GIT_TAG:+-DRBP_RELIC_GIT_TAG=$RELIC_GIT_TAG} \
    && cmake --build build --parallel \
    && ctest --test-dir build --output-on-failure \
    && cmake --install build && ldconfig

WORKDIR /LibRBP/demo
RUN cmake -B build -S . && cmake --build build --parallel

CMD ["./build/demo"]
