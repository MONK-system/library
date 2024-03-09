import argparse
import pandas as pd
import plotly.graph_objects as go
from plotly.subplots import make_subplots
import numpy as np

# Initialize the parser
parser = argparse.ArgumentParser(description='Plot CSV data')

# Add the input file as a positional argument
parser.add_argument('input', help='Input CSV file path')

# Add the '--sub' optional flag
parser.add_argument(
    '--sub', help='Generate subplot graphs', action='store_true')

# Parse the command line arguments
args = parser.parse_args()

# Use the input file path provided from the command line
csv_file_path = args.input

# Define the number of rows to read (optional, remove or adjust as needed)
number_of_rows = 10000

# Read the CSV file, specifying the number of rows to read
df = pd.read_csv(csv_file_path, nrows=number_of_rows)

# Convert all columns to numeric, coercing errors to NaN (non-numeric values will become NaN)
df = df.apply(pd.to_numeric, errors='coerce')

# Interpolate missing data
df.interpolate(method='linear', limit_direction='forward',
               axis=0, inplace=True)

# Drop any remaining NaNs or infs
df.replace([np.inf, -np.inf], np.nan, inplace=True)
df.dropna(inplace=True)

# If the '--sub' flag is provided, generate subplots
if args.sub:
    # Create subplots: one row for each measurement
    fig = make_subplots(rows=len(df.columns[1:]), cols=1, shared_xaxes=True)

    # Add a trace for each measurement, in a separate subplot
    for i, column in enumerate(df.columns[1:], start=1):
        fig.add_trace(
            go.Scatter(x=df['Time: (s)'],
                       y=df[column],
                       mode='lines',
                       name=column),
            row=i, col=1
        )

        # Set the title for each subplot's y-axis to the column name
        axis_name = f'yaxis{i}' if i > 1 else 'yaxis'
        fig['layout'][axis_name].update(title=column)

    # Update layout with titles and adjust other layout elements
    fig.update_layout(
        title='Measurements Over Time',
        xaxis_title='Time (s)',
        yaxis_title='',
        legend_title='Measurements',
        hovermode='closest',
        height=300 * len(df.columns[1:])
    )

    # Update xaxis properties
    fig.update_xaxes(title_text='Time (s)', row=len(df.columns[1:]), col=1)

    # Update yaxis titles
    for i, column in enumerate(df.columns[1:], start=1):
        fig.update_yaxes(title_text=column, row=i, col=1)

    # Show the plot
    fig.show()

else:  # Otherwise, generate a combined graph
    # Create a Plotly Graph Objects figure
    fig = go.Figure()

    # Add a line for each measurement column, excluding 'Time (s)'
    for column in df.columns[1:]:
        fig.add_trace(go.Scatter(x=df['Time: (s)'],
                                 y=df[column], mode='lines', name=column))

    # Update layout with titles and adjust other layout elements
    fig.update_layout(title='Measurements Over Time (Limited Data)',
                      xaxis_title='Time (s)',
                      yaxis_title='Measurement Value',
                      legend_title='Measurements')

    # Show the plot
    fig.show()
