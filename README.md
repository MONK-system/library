# MONK - MFER Library

A library for management of Nihon-Kohden MFER data with python bindings.

As of this commit, the library can take an input MWF file, print the data & header, and write the formatted data in a csv, categorized by channel & ordered by time.

## Installing the library

### Prerequisites

- **Python**: Python 3.7 or newer.
- **CMake**: Version 3.8 or newer. CMake is used to configure the build process.
- **g++** or **MSVC**: A C++ compiler that supports C++17 standard. For g++, version 7.3 or newer is recommended. For MSVC, Visual Studio 2019 or newer is required.

### CLI

Clone and initialize the repository:

```
git clone https://github.com/MONK-system/library.git
cd ./library
[branch {branch}]
git submodule update --init --recursive
```

(Optionally switch to branch)

Install the library:

```
pip install .
```

This will install the library as "monkLib". (as of now)

Alternatively, you can install the package directly by using:

```
pip install git+https://github.com/MONK-system/library.git[@branch]
```

@branch to optionally select which branch.

## Script: write.py

This script will print the header information of a file and write to csv.

```
python write.py {input} {output}
```

For example: `python write.py data.MWF output.csv` will print the header information and write the data to output.csv.

## Script: graph.py

### Prerequisites

- pandas
- plotly

### Usage

```
python graph.py {data} [--sub]
```

For example: `python graph.py output.csv --sub`. Creates subplot figure. Without the `--sub` flag, a combined plot is created.

Creates a combined plot or subplots of the csv data. Currently, the plots are limited to 10 000 lines (in graph.py), for optimization.
