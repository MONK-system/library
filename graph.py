import pandas as pd
import plotly.express as px

# Path to your CSV file
csv_file_path = 'output.csv'

# Read the CSV file. Assuming it's a single column of values with no header.
df = pd.read_csv(csv_file_path, header=None)

# Name the column for clarity
df.columns = ['Value']

# Reset the index to use it as the X-axis in the plot
df.reset_index(inplace=True)
df.columns = ['Index', 'Value']

# Create an interactive line plot with Plotly Express
fig = px.line(df, x='Index', y='Value', title='Interactive Line Graph')

# Show the plot
fig.show()
