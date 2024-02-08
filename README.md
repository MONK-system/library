# MONK - MFER Library

A library for management of Nihon-Kohden MFER data.

At this current commit, the program parses binary files into collections of tags, length and content according to the specification, and prints it to the console:

![image](https://github.com/MONK-system/library/assets/102857059/007b2ade-048f-485f-a822-d97401aa0c80)

## Building & running the application
### Prerequisites
- g++
- make

### From command-line
#### Compile
```
make
```

Compiles the application to an "out" executable (out.exe on Windows).

#### Running the executable
```
./out -i {file}
```

Runs the application with a given file. (```./out -i ./test-file.MWF``` for example)

#### Other make rules
```
make test
```

```
make test FILE={file}
```

Runs the application with a given file. By default, FILE = ./test-file.MWF.

```
make all
```

```
make all FILE={file}
```

Compiles and runs the application with a given file, given changes to source code. By default, FILE = ./test-file.MWF. Mainly used for development.

```
make clean
```

Cleans compilation files (out and .o files).
