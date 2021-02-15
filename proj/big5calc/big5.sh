#!/bin/bash

src/fix.py input_fixed/ conf/fixconf input/* || exit 1

src/calc.py input_fixed input_calc
