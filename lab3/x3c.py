import pycosat
import random
from dimacs import *

def x3c(n, subsets):
    # todo reduction x3c => sat
    pass
    

if __name__=='__main__':
    test_name = '10.no.x3c'
    tests_dir = 'x3c-tests'
    test = f'{tests_dir}/{test_name}'

    example = loadX3C(test)
    n = example[0]
    subsets = example[1][1:]

    x3c(n, subsets)
    