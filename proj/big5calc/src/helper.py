#!/usr/bin/python3

import os
import sys

def slicer(s, delete_to):
    index=s.find(delete_to)
    if index != -1:
        return s[index + 1:]
    return s
