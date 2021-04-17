#!/usr/bin/python3

import sys

for s in sys.argv[1:]:
    val = 0
    for letter in s:
        val = val * 26
        num = ord(letter) - ord('a') + 1
        val += num
    print (val)
