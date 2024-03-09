
import monkLib

data = monkLib.NihonKohdenData("test-file.MWF")

header = data.getHeader()
print(header)
