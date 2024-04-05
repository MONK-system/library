# Print header information from a MFER file
from monklib import print_header
import argparse

# Initialize the parser
parser = argparse.ArgumentParser(description='Print MFER header')
# Add the input file as a positional argument
parser.add_argument('input', help='Input binary data file path')
# Parse the command line arguments
args = parser.parse_args()

print_header(args.input)