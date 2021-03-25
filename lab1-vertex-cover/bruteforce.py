from dimacs import *
from itertools import *
from grademe import isVC

def VC_bruteforce(G, k):
    edges = edgeList(G)
    n = len(G)
    for cover in combinations(range(n), k):
        if isVC(edges, cover):
            return True, cover

    return False, {}


def solve(name):
    print('Attempting to solve ' + name + '...')
    G = loadGraph("graph/" + name)
    V = len(G)
    for k in range(1, V):
        success, cover = VC_bruteforce(G, k)
        if success:
            saveSolution("graph/" + name + ".sol", cover)
            print('Solved ' + name + '!')
            break


solve("e20")

