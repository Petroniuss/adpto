import pycosat
import random

import matplotlib.pyplot as plt

def satPhaseTransition(n: int, k: int, T: int):
    S = [1,-1]            
    V = range(1, n + 1)

    def randVar():
        return random.choice(V) * random.choice(S)  

    def genFormula(clausesNumber: int):
        return [[ randVar() for _ in range(k) ] for _ in range(clausesNumber) ]

    inverse_step = 10
    xs, ys = [], []
    for i in range(1, 10 * inverse_step):
        a = (1 + i / inverse_step)
        clausesNumber = int(a * n)

        satisfiable = 0
        for _ in range(T):
            formula = genFormula(clausesNumber)
            if pycosat.solve(formula) != u'UNSAT':
                satisfiable += 1

        res = satisfiable / T

        xs.append(a)
        ys.append(res)
        print(a, res)
    
    plt.plot(xs, ys)
    plt.show()

if __name__=='__main__':
    satPhaseTransition(10, 3, 100)