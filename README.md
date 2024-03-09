# MONK - MFER Library

A library for management of Nihon-Kohden MFER data, eventually with python bindings.

As of this commit, running the program will take an input MWF file, print the data & header, and write the formatted data in a csv, categorized by channel & ordered by time.

![CSV output](img/csv-output.png)

## Building & running the application with CMake

### Prerequisites

- g++
- CMake

#### Compile

Create a directory called build in the project root directory

Inside the build directory, run cmake on the project directory:

```
cmake ..
```

Then build the project with:

```
make
```

Compiles the application to Library.app.

#### Running the executable

```
./Library.app -i {input-file} -o {output-file}
```

Runs the application with a given file. (`./Library.app -i ./test-file.MWF -o ./output.csv` for example)

### Other CMake rules

```
rm -rf build/
```

Then repeat earlier steps to make a new build.

## Building & running the application with CMake

### Prerequisites

- g++
- make

#### Compile

```
make
```

Compiles the application to an "out" executable (out.exe on Windows).

#### Running the executable

```
./out -i {input-file} -o {output-file}
```

Runs the application with a given file. (`./out -i ./test-file.MWF -o ./output.csv` for example)

#### Other make rules

```
make test [in={input-file}] [out={output-file}]
make all [in={input-file}] [out={output-file}]
```

For example: `make test in=./data.MWF out=./out.csv`

`test` runs the application with a given input and output.
`all` compiles and runs the application with a given file, given changes to source code. These rules are mostly used for development.

If left empty, the variables will default to `in=./test-file.MWF` & `out=./output.csv`

```
make clean
```

Cleans compilation files (out and .o files).

## Graphing the output

### Prerequisites

- python
  - pandas
  - plotly
  - pyarrow

### From command-line

```
python ./graph.py {data} [--sub]
```

For example: `python ./graph.py ./output.csv --sub`. Creates subplot figure. Without the `--sub` flag, a combined plot is created.

Creates a combined plot or subplots of the csv data. Currently, the plots are limited to 10 000 lines (in graph.py), for optimization.
