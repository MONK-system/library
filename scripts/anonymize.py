#!/usr/bin/env python3

import os
import sys

def modify_binary_file(input_path, output_path):
    try:
        # Read the entire content of the input file
        with open(input_path, 'rb') as file:
            data = file.read()

        # Create a sequence of '-' characters and convert it to bytes
        id_replacement_bytes = ('-' * 27).encode()
        name_replacement_bytes = ('-' * 66).encode()
        birth_date_replacement_bytes = bytes([0xFF] * 7)
        gender_replacement_bytes = bytes([0x00])

        # Modify patient's ID
        modified_data = data[:86] + id_replacement_bytes + data[86+27:]

        # Modify patient's Name
        modified_data = modified_data[:132] + name_replacement_bytes + modified_data[132+66:]

        # Modify patient's Date of birth
        modified_data = modified_data[:212] + birth_date_replacement_bytes + modified_data[212+7:]

        # Modify patient's Gender
        modified_data = modified_data[:221] + gender_replacement_bytes + modified_data[221+1:]

        # Write the modified data to the output file
        with open(output_path, 'wb') as file:
            file.write(modified_data)

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
