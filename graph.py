import argparse
import pandas as pd
import plotly.graph_objects as go

# Initialize the parser
parser = argparse.ArgumentParser(description='Plot CSV data')

# Add the '-i' argument for the input file path
parser.add_argument('-i', '--input', help='Input CSV file path', required=True)

# Parse the command line arguments
args = parser.parse_args()

# Use the input file path provided from the command line
csv_file_path = args.input

# Define the number of rows to read (optional, remove or adjust as needed)
number_of_rows = 10000

# Read the CSV file, specifying the number of rows to read
df = pd.read_csv(csv_file_path, nrows=number_of_rows)

# Create a Plotly Graph Objects figure
fig = go.Figure()

# Add a line for each measurement column, excluding 'Time (s)'
# Skip 'Time (s)' which is assumed to be the first column
for column in df.columns[1:]:
    fig.add_trace(go.Scatter(x=df['Time (s)'],
                  y=df[column], mode='lines', name=column))

# Update layout with titles and adjust other layout elements
fig.update_layout(title='Measurements Over Time (Limited Data)',
                  xaxis_title='Time (s)',
                  yaxis_title='Measurement Value',
                  legend_title='Measurements')

# Show the plot
fig.show()
