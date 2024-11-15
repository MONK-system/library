import pytest
import monklib
import os

@pytest.fixture
def test_file_path():
    file_path = os.getenv("TEST_FILE_PATH")
    if not file_path:
        pytest.fail("The TEST_FILE_PATH environment variable must be provided.")
    return file_path

def test_file_path_argument(test_file_path):
    assert test_file_path == "./resources/test-file.MWF"

# Sample helper function to strip null characters
def trim_nulls(string):
    return string.replace('\x00', '')

def test_initialization_with_filename(test_file_path):
    # Initialize NihonKohdenData with a filename
    data = monklib.Data(test_file_path)
    header = data.getHeader()
    
    # Assertions for header properties
    assert trim_nulls(header.preamble) == "MFR Monitoring Waveform"
    assert header.byteOrder.name == "ENDIAN_LITTLE"
    assert trim_nulls(header.modelInfo) == "NIHON KOHDEN^CNS6000^0, 5, 0, 9"
    assert header.measurementTimeISO == "2019-06-19T13:20:00"
    assert trim_nulls(header.patientID) == "12345"
    assert trim_nulls(header.patientName) == "TRWRU"
    assert header.birthDateISO == "N/A"
    assert header.patientSex == "Unknown"
    assert header.samplingIntervalString == "1x10^-3 (s)"
    assert len(header.events) == 0
    assert header.sequenceCount == 12
    assert header.channelCount == 6

def test_anonymize(test_file_path, tmp_path):
    # Load data, anonymize, and save
    data = monklib.Data(test_file_path)
    data.anonymize()
    anonymized_path = tmp_path / "anonymized.MWF"
    data.writeToBinary(str(anonymized_path))

    # Reload anonymized data
    anonymized_data = monklib.Data(str(anonymized_path))
    header = anonymized_data.getHeader()
    assert trim_nulls(header.patientID) == ""
    assert trim_nulls(header.patientName) == ""

def test_set_channel_selection(test_file_path):
    # Load data and manipulate channel selection
    data = monklib.Data(test_file_path)
    data.setChannelSelection(0, True)  # Activate a channel
    with pytest.raises(RuntimeError):
        data.setChannelSelection(17, True)  # Invalid index

def test_set_interval_selection(test_file_path):
    # Load data and manipulate interval selection
    data = monklib.Data(test_file_path)
    data.setIntervalSelection(0, 10)  # Valid interval
    with pytest.raises(RuntimeError):
        data.setIntervalSelection(10, 5)  # Invalid (start > end)
    with pytest.raises(RuntimeError):
        data.setIntervalSelection(-1, 10)  # Invalid (negative start)
    with pytest.raises(RuntimeError):
        data.setIntervalSelection(0, -1)  # Invalid (negative end)

def test_write_to_binary(test_file_path, tmp_path):
    # Load data and write to a binary file
    data = monklib.Data(test_file_path)
    output_path = tmp_path / "output.MWF"
    data.writeToBinary(str(output_path))

    # Reload and compare data
    original_data = monklib.Data(test_file_path)
    written_data = monklib.Data(str(output_path))
    assert str(written_data.getHeader()) == str(original_data.getHeader())

def test_write_to_csv(test_file_path, tmp_path):
    # Load data and write to a CSV file
    data = monklib.Data(test_file_path)
    csv_path = tmp_path / "output.csv"
    data.writeToCsv(str(csv_path))

    # Verify CSV content
    with open(csv_path, "r") as f:
        first_line = f.readline().strip()
    expected_header = "Time: (s), ECG II: 2x10^-6 (V), ECG V5: 2x10^-6 (V), ART: 125x10^-3 (mmHg), PAP: 125x10^-3 (mmHg), CVP: 125x10^-3 (mmHg), Pacing Status: N/A"
    assert first_line == expected_header
