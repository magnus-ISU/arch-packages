#!/usr/bin/python3

import sys
import glob

import helper

csvnames = []
csvdata  = []
calcscores = []

if len(sys.argv) < 3:
    print ('Big 5 calculator (helper) program!\n./program input_folder output_folder\n')
    exit(0)

input_folder  = sys.argv[1]
output_folder = sys.argv[2]

for fname in glob.glob(input_folder + '/*'):
    with open(fname) as fdata:
        qdata = fdata.read().splitlines()
    csvnames.append(helper.slicer(fname, '/'))
    csvdata.append(qdata)

# indexes are the nth element, counting from 1 NOT 0
def average (data, center, indexes):
    retval = 0
    count = 0
    for index in indexes:
        neg = False
        if index < 0:
            index = -index
            neg = True
        to_add = data[index - 1]
        if to_add == 'N':
            continue
        to_add = int(to_add)
        if neg:
            to_add = 2 * center - to_add
        retval += to_add
        count += 1
    return retval / count

for i in range(len(csvnames)):
    scores = {}
    scores['BFAS_E'] = average(csvdata[i], 3, [4, 9, -14, 19, -24, -29, -34, 39, 44, -49, -54, 59, -64, 69, 74, -79, 84, 89, 94, -99])
    scores['BFAS_A'] = average(csvdata[i], 3, [-2, 7, 12, -17, 22, 27, -32, -37, 42, 47, -52, 57, -62, -67, 72, -77, -82, -87, 92, -97])
    scores['BFAS_C'] = average(csvdata[i], 3, [3, -8, -13, 18, -23, 28, -33, 38, 43, -48, -53, 58, 63, -68, 73, -78, -83, 88, -93, 98])
    scores['BFAS_N'] = average(csvdata[i], 3, [1, -6, -11, 16, 21, -26, -31, 36, 41, -46, -51, 56, -61, -66, 71, 76, -81, -86, -91, -96])
    scores['BFAS_O'] = average(csvdata[i], 3, [5, 10, -15, 20, 25, 30, 35, 40, -45, -50, -55, -60, 65, 70, 75, -80, -85, -90, 95, 100])

    calcscores.append(scores)

for i in range(len(csvnames)):
    ofile = output_folder.replace('/', '') + '/' + csvnames[i]
    f = open(ofile, 'w')
    for key in calcscores[i]:
        f.write('%f,%s\n' % (calcscores[i][key],key))
