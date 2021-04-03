#!/usr/bin/python3

from functools import lru_cache
import time

@lru_cache(maxsize=3)
def fib(n):
    if n <= 1:
        return 1

    return fib(n - 1) + fib(n - 2)

def main():
    timer = time.time()


    f = open("fib.txt", "a")
    i = 0
    while True:
        if i % 1000000 == 0:
            print(str(i) + "\t" + str(round(time.time() - timer, 1)) + "s\t", end="")
            timer = time.time()

            f.write(str(i) + ":" + str(fib(i)) + "\n")
            i += 1
            f.write(str(i) + ":" + str(fib(i)) + "\n")
            i += 1

            print(str(round(time.time() - timer, 1)) + "s")
            timer = time.time()
        else:
            fib(i)
            i += 1
    f.close()

main()
