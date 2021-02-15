#!/usr/bin/python3

# Takes in input from stdin as a csv file, reads it according to a config file, and prints to standard out a file with only data numbers, one per line, or an N if no data
# If there is an error in parsing the file, such as an out-of-bounds number, warn the user and exit.

import sys

import helper

def get_score(s, line, fileid, mi, ma):
    global error
    s = s.split(',', 1)[0]
    if len(s) == 0:
        warnings[fileid].append('Warning at line %d: No answer (expected between %d and %d)' % (line, mi, ma))
        return 'N'
    if s in 'afF':
        s = '0'
    if s in 'btT':
        s = '1'
    if not s.isnumeric():
        errors[fileid].append('Error   at line %d: malformatted answer %s (expected between %d and %d)' % (line, s, mi, ma))
        error = True
        return 'E'
    if int(s) < mi:
        errors[fileid].append('Error   at line %d: Value %s is less than minimum %d' % (line, s, mi))
        error = True
        return 'm'
    if int(s) > ma:
        errors[fileid].append('Error   at line %d: Value %s is greater than maximum %d' % (line, s, ma))
        error = True
        return 'M'

    return s

if len(sys.argv) < 4:
    print ('Big 5 Calculator - Changes .csv files to lists of numbers - Magnus Anderson\nUsage: ./fix.py output_folder format_file [.csv files]')
    exit(0)

file_template = sys.argv[2]
output_folder = sys.argv[1]

csvfiles = []
csvnames = []
scores = []
warnings = []
errors = []
error = False

with open (file_template) as f:
    template = f.read().splitlines()

for arg in sys.argv[3:]:
    with open (arg) as csv:
        qdata = csv.read().splitlines()
    csvfiles.append(qdata)
    csvnames.append(helper.slicer(arg, '/'))
    scores.append([])
    warnings.append([])
    errors.append([])

linenumb = 0
for line in template:
    conf = line.split(',')
    mi = 0
    ma = 0
    nlines = 0
    ignore = False
    for option in conf:
        key, val = option.split(':', 1)
        if key == 'min':
            mi = int(val)
        if key == 'max':
            ma = int(val)
        if key == 'lines':
            nlines = int(val)
        if key == 'ignore':
            ignore = True
    if ignore:
        linenumb += nlines
        continue
    for i in range(nlines):
        for j in range(len(csvfiles)):
            f = csvfiles[j]
            fname = csvnames[j]
            arr = scores[j]
            val = get_score(f[linenumb], linenumb + 1, j, mi, ma)
            arr.append(val)
        linenumb += 1

sep = False
for i in range(len(csvnames)):
    fname = csvnames[i]
    warn = warnings[i]
    err = errors[i]
    if len(err) > 0:
        if not sep:
            sep = True
            print()
        print('Errors in file %s' % (fname))
        for w in warn:
            print (w)
        for e in err:
            print (e)
        print()
    elif len(warn) > 0:
        if not sep:
            sep = True
            print()
        print('Warnings in file %s' % (fname))
        for w in warn:
            print (w)
        print()

if error:
    exit(1)

i = 0
for student in csvnames:
    filename = output_folder.replace('/', '') + '/' + student
    f = open(filename, "w")
    for val in scores[i]:
        f.write("%s\n" % (val))
    i += 1
