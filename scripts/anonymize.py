# Anonymize and write the data from a MFER file to a new file
from monklib import Data
import argparse

# Initialize the parser
parser = argparse.ArgumentParser(description='Anonymize MFER data')
# Add the input file as a positional argument
parser.add_argument('input', help='Input binary data file path')
# Add the output file as an positional argument
parser.add_argument('output', help='Output binary data file path')
# Parse the command line arguments
args = parser.parse_args()

data = Data(args.input)
data.anonymize()
data.writeToBinary(args.output)