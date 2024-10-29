# RELIC based Bilinear Pairing Library (RBP)
[![Asymmetric Curve CI](https://github.com/Weiqi97/RBP/actions/workflows/asym_ci.yml/badge.svg)](https://github.com/Weiqi97/RBP/actions/workflows/asym_ci.yml) [![Symmetric Curve CI](https://github.com/Weiqi97/RBP/actions/workflows/sym_ci.yml/badge.svg)](https://github.com/Weiqi97/RBP/actions/workflows/sym_ci.yml)

RBP is a wrapper designed for constructing bilinear pairing groups using the [RELIC](https://github.com/relic-toolkit/relic) library. It offers a set of slightly simplified APIs to facilitate the implementation and prototyping of pairing-based cryptographic schemes, leveraging the capabilities of the RELIC library.

To build and install this library, you must first install RELIC and its dependencies. For a detailed installation guide, please refer to their [wiki page](https://github.com/relic-toolkit/relic/wiki/Building). Once RELIC is installed and configured with the desired curve, you can simply run the following command in the root directory of this project:
```bash
mkdir build
cd build
cmake ..
make

# Optionally, you could run ctest to ensure the compile was successful.
ctest

# Optionally, you could choose to install this library to the default path.
make install
```
You could also refer to the [Dockerfile](Dockerfile) for detailed steps on setting up the environment. You could also just use that file and build a docker image as your development environment.

A simple example of how to include and use this library is provided in the [demo](demo) folder.