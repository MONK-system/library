import os
import sys

# This function assumes standard Nihon Kohdnen file structure,
# variances might not be accounted for (encoding differences, custom name length, etc.)
# Script anonomizes patient ID, name, date of birth & sex

def modify_binary_file(input_path, output_path):
    try:
        # Read the entire content of the input file
        with open(input_path, 'rb') as file:
            data = file.read()

        # Declare indexes for the fields to be modified [index, length, replacement bytes]
        replacement_bytes = [
            [101, 17, bytes([0x00] * 17)], # Patient ID
            [132, 66, bytes([0x00] * 66)], # Patient Name
            [212, 7, bytes([0xFF] * 7)], # Patient Date of Birth
            [221, 1, bytes([0x00])] # Patient Sex
        ]

        # Replace the bytes in the data
        for field in replacement_bytes:
            index, length, replacement = field
            data = data[:index] + replacement + data[index+length:]

        # Write the modified data to the output file
        with open(output_path, 'wb') as file:
            file.write(data)

        print("File has been modified and saved successfully.")

    except Exception as e:
        print(f"An error occurred: {e}")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python script.py <input_file> <output_file>")
    else:
        input_file_path = sys.argv[1]
        output_file_path = sys.argv[2]
        modify_binary_file(input_file_path, output_file_path)
