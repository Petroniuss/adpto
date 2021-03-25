from dimacs import *
from itertools import *
from grademe import isVC

def VC_2_k(edges, k, cover, edge_index):
    while edge_index < len(edges):
        u, v = edges[edge_index]
        if not ((u in cover) or (v in cover)):
            break
        else:
            edge_index += 1
    
    if edge_index >= len(edges):
        return True, cover

    if k == 0:
        return False, set()

    u, v = edges[edge_index]

    success, other_cover = VC_2_k(edges, k - 1, cover | {u}, edge_index + 1)
    if success:
        return success, other_cover

    return VC_2_k(edges, k - 1, cover | {v}, edge_index + 1)


def solve(name):
    print('Attempting to solve ' + name + '...')
    G = loadGraph("graph/" + name)
    edges = edgeList(G)
    V = len(G)
    for k in range(1, V):
        print(f'\t Trying k: {k}')
        success, cover = VC_2_k(edges, k, set(), 0)
        if isVC(edges, cover):
            saveSolution("graph/" + name + ".sol", cover)
            print('Solved ' + name + '!')
            break

solve("e5")
