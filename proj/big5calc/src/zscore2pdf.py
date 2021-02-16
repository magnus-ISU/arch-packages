#!/usr/bin/python3

# reads the spreadsheet generated in the last section (from stdin) and creates a folder for each student (who is in the last section added, so all students after the last blank line) with pdfs inside for their scores on different personality measures, such as Big 5, etc
# The folders for each student are created inside a folder passed in as a command line argument. That folder must already exist and not end with a /

import genpdf
import sys
import os
import random

data = sys.stdin.read().splitlines()
headers = data[0].split(',')

output_dir = sys.argv[1]

last_empty_line = 0
for i in range(len(data)):
    if len(data[i].split(',')[0]) == 0:
        last_empty_line = i

# We are only concerned with lines between the last empty one (or the header, which is on line 0, so we are good there) and the end
data = data[last_empty_line + 1:]

big5 = genpdf.PDF(orientation='P', unit='mm', format='A4')
big5ids = [1, 2, 3, 4, 5]
random.shuffle(big5ids)
big5.add_page()
for index in big5ids:
    big5.add_title(headers[index])

# Generate a new page in each pdf with the appropriate data
for line in data:
    scores = line.split(',')
    for i in range(len(scores)):
        if i > 0:
            scores[i] = float(scores[i])

    big5.add_page()
    big5.add_title("Big 5 Scores for %s" % (scores[0]))
    for index in big5ids:
        big5.add_row(scores[index])

big5.output(output_dir + '/big5.pdf', 'F')
