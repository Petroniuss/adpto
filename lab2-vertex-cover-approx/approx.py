from dimacs import *
from itertools import *
import random

def two_approx(G):
    V = len(G) - 2 # 1...V
    E = edgeList(G)
    cover = set()
    for u, v in E:
        if u not in cover and v not in cover:
            cover.add(u)
            cover.add(v)
        
    return cover


def logn_approx(G):
    V = len(G) 
    buckets = [ set() for _ in range(V) ]
    cover = set()
    for v in range(V):
        buckets[len(G[v])].add(v)

    for degree in range(V - 1, 0, -1):
        while len(buckets[degree]) > 0:
            v = buckets[degree].pop()
            for u in G[v]:
                u_deg = len(G[u])
                # move u down
                G[u].remove(v)
                buckets[u_deg].remove(u)
                if u_deg - 1 > 0:
                    buckets[u_deg - 1].add(u)
            
            cover.add(v)

    return cover


def something_random(G):
    V = len(G) - 2 # 1...V
    E = len(edgeList(G))

    covered = [ len(G[i]) == 0 for i in range(len(G)) ]
    cover = set()
    while E > 0:
        u = random.randint(1, V)
        while covered[u] is True:
            u = random.randint(1, V)

        E -= len(G[u])
        covered[u] = True        
        cover.add(u)

        for v in G[u]:
            G[v].remove(u)
            if len(G[v]) == 0:
                covered[v] = True

    return cover


def solve(name):
    print('Attempting to solve ' + name + '...')
    G = loadGraph("graph/" + name)
    V = len(G)
    cover = logn_approx(G)
    saveSolution("graph/" + name + ".sol", cover)
    print('Solved ' + name + '!')

solve("k330_a")