# MONK - MFER Library

A library for management of Nihon-Kohden MFER data with python bindings.

As of this commit, the library can take an input MWF file, print the data & header, and write the formatted data in a csv, categorized by channel & ordered by time.

## Installing the library

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
pip install git+https://github.com/MONK-system/library.git[@branch]
```

@branch to optionally select which branch.

## Script: write.py

This script will write the data of a file to csv.

```
python scripts/write.py {input} {output}
```

For example: `python write.py data.MWF output.csv` will print the header information and write the data to output.csv.

## Script: print.py

This script will print the header information of a file.

```
python scripts/print.py {input}
```

## Script: anonymize.py

This script will anonymize and write the data to a binary file.

```
python scripts/anonymize.py {input} {output}
```

## Script: graph.py

### Prerequisites

- numpy
- pandas
- plotly

### Usage

```
python scripts/graph.py {data} [--combined]
```

For example: `python graph.py output.csv --combined`. Creates combined figure. Without the `--combined` flag, subplot figures are created.

Creates subplots or a combined plot of the csv data. Currently by default, the plots are limited to a maximum of 10 000 lines (in graph.py), for optimization.
