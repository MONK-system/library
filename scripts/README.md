# Scripts

These scripts provide some basic utilities using the library, and shows some basic implementation.

## write.py

This script will write the data of a file to csv.

```
python scripts/write.py {input} {output}
```

For example: `python write.py data.MWF output.csv` will print the header information and write the data to output.csv.

## print.py

This script will print the header information of a file.

```
python scripts/print.py {input}
```

## anonymize.py

This script will anonymize and write the data to a binary file.

```
python scripts/anonymize.py {input} {output}
```

## graph.py

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
