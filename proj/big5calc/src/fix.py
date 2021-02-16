#!/usr/bin/python3

# Takes in input from stdin as a csv file, reads it according to a config file, and prints to standard out a file with only data numbers, one per line, or an N if no data
# The first line in output is the name
# If there is an error in parsing the file, such as an out-of-bounds number, warn the user and exit.

import sys

import helper

def get_score(s, line, mi, ma):
    s = s.split(',', 1)[0]
    if len(s) == 0:
        warnings.append('Warning at line %d: No answer (expected between %d and %d)' % (line, mi, ma))
        return 'N'
    if s in 'afF':
        s = '0'
    if s in 'btT':
        s = '1'
    if not s.isnumeric():
        errors.append('Error   at line %d: malformatted answer %s (expected between %d and %d)' % (line, s, mi, ma))
        return 'E'
    if int(s) < mi:
        errors.append('Error   at line %d: Value %s is less than minimum %d' % (line, s, mi))
        return 'E'
    if int(s) > ma:
        errors.append('Error   at line %d: Value %s is greater than maximum %d' % (line, s, ma))
        return 'E'

    return s

if len(sys.argv) < 2:
    print ('Big 5 Calculator - Changes .csv stdin to list of numbers - Magnus Anderson\nUsage: ./fix.py format_file', file=sys.stderr)
    exit(0)

file_template = sys.argv[1]

with open (file_template) as f:
    template = f.read().splitlines()

csvdata = sys.stdin.read().splitlines()

scores = []
warnings = []
errors = []
name = ''

linenumb = 0
for line in template:
    conf = line.split(',')
    mi = 0
    ma = 0
    nlines = 0
    ignore = False
    name = False
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
        if key == 'name':
            name = True
    if ignore:
        linenumb += nlines
        continue
    if len(csvdata) < linenumb + 1:
        errors.append('File ended abruptly')
        break
    if name:
        if csvdata[linenumb].find(',') == -1:
            errors.append('No name or incorrectly formatted name')
            break
        name = csvdata[linenumb].split(',')[0]
        linenumb += 1
        continue
    for i in range(nlines):
        val = get_score(csvdata[linenumb], linenumb + 1, mi, ma)
        scores.append(val)
        linenumb += 1

if len(errors) > 0:
    print('Errors in file')
    for w in warnings:
        print (w, file=sys.stderr)
    for e in errors:
        print (e, file=sys.stderr)
    exit(1)
elif len(warnings) > 0:
    print('Warnings in file')
    for w in warn:
        print (w, file=sys.stderr)
    print()

print(name, end='')
for score in scores:
    print(',', end='')
    print(score, end='')
    comma=True
print()
