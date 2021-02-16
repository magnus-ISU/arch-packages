#!/usr/bin/python3

import sys
import math

# Takes in standard input as a csv file and prints the same file with each column, except the first, standardized. Ignores a row if the first cell is empty

def avg(arr):
    retval = 0
    for val in arr:
        retval += float(val)
    return retval / len(arr)

def stddev(arr, mean):
    retval = 0
    for i in range(len(arr)):
        retval += (float(arr[i]) - mean) ** 2
    return math.sqrt(retval / (len(arr) - 1))

data = sys.stdin.read().splitlines()

# Find out how many rows are in the file

headings = data[0].split(',')

variables = {}
averages = {}
stddevs = {}

for heading in headings:
    variables[heading] = []

for line in data[1:]:
    arr = line.split(',')
    if len(arr[0]) == 0:
        continue
    for i in range(len(arr)):
        variables[headings[i]].append(arr[i])

for heading in headings[1:]:
    averages[heading] = avg(variables[heading])
    stddevs[heading] = stddev(variables[heading], averages[heading])

print (data[0])
for line in data[1:]:
    arr = line.split(',')
    print(arr[0], end='')
    for i in range(len(arr) - 1):
        if len(arr[0]) == 0:
            print(',', end='')
        else:
            heading = headings[1 + i]
            val = float(arr[1 + i])
            zscore = (val - averages[heading]) / stddevs[heading]
            print(',%f' % (zscore), end='')
    print()
