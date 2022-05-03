import sys
import os

D = int(os.environ["D"])

lines = sys.stdin.readlines()
print(int(len(lines)))
for line in lines:
    var = int(line.split()[0][1:])
    print(f"{var // D} {var % D}")
