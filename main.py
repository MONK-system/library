import monkLib
import argparse

# Initialize the parser
parser = argparse.ArgumentParser(description='Plot CSV data')
# Add the input file as a positional argument
parser.add_argument('input', help='Input CSV file path')
# Add the output file as an positional argument
parser.add_argument('output', help='Output CSV file path')
# Parse the command line arguments
args = parser.parse_args()

data = monkLib.NihonKohdenData(args.input)

data.printHeader()
data.writeToCsv(args.output)