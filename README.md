# MONK - MFER Conversion Library

A python library for management of Nihon-Kohden MFER data using C++ & python bindings.

As of this version, the library handles:

- Data extraction of MFER formatted binary files (.MWF)
  - Conversion of waveform channels to .csv
    - Channel selection
    - Interval selection
  - Header information (optionally anonymized)
- Anonymization of sensitive fields

Some scripts that provide basic utilities can be found in [scripts/](scripts/)

## Installation

### Prerequisites

- **Python**: Python 3.7 or newer.
- **CMake**: Version 3.8 or newer. CMake is used to configure the build process.
- **g++** or **MSVC**: A C++ compiler that supports C++17 standard. For g++, version 7.3 or newer is recommended. For MSVC, Visual Studio 2019 or newer is required.

### CLI

Clone and install the repository:

```
git clone --recursive https://github.com/MONK-system/library.git
pip install ./library
```

This will install the library as "monklib". (as of now)

Alternatively, you can install the package directly by using:

```
pip install git+https://github.com/MONK-system/library.git
```
