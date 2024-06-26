# Write the data from a MFER file to another CSV file
from monklib import convert_to_csv
import argparse

# Initialize the parser
parser = argparse.ArgumentParser(description='Write CSV data')
# Add the input file as a positional argument
parser.add_argument('input', help='Input binary data file path')
# Add the output file as an positional argument
parser.add_argument('output', help='Output CSV file path')
# Parse the command line arguments
args = parser.parse_args()

convert_to_csv(args.input, args.output)