#!/bin/bash

# List all files in the folder input/ and run fix on them. If all pass
p=True
for filename in input/*; do
	echo $filename | lolcat
	cat $filename | src/fix.py conf/fixconf > /dev/null || p=False
done

if [ p == 'False' ]; then
	exit 1
fi

# Pipe all files through fix and then calc to get the individual scores. Add them to a master spreadsheet
echo No errors, adding to master.csv
src/calc.py --fake-data | head -n 1 >> master.csv
for filename in input/*; do
	cat $filename | src/fix.py conf/fixconf 2> /dev/null | src/calc.py | tail -n 1 >> master.csv
done

day=$(date '+%Y-%m-%d__%H-%M-%S')

# Calculate the Z-scores of each student from the last spreadsheet and add them to a new spreadsheet
# Pipe that spreadsheet to a new program that creates a pdf for each interesting collection of data (big 5, affect..., etc) and stores the pdfs in a folder
echo creating directory "output/$day"
mkdir -p "output/$day"
echo generating pdfs...
cat master.csv | src/zscore.py | src/zscore2pdf.py "output/$day"
