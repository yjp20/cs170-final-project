import cvxpy as cp
import numpy as np
import sys

name = "small/128"

with open(f'inputs/{name}.in', "r") as f:
    lines = [line for line in f.readlines() if line[0] != '#']
    N = int(lines[0])
    D = int(lines[1])
    Rs = int(lines[2])
    Rp = int(lines[3])

    _grid = [0.0] * (D*D)
    _mask = [0.0] * (D*D*D*D)

    for i in range(N):
        a, b = [int(x) for x in lines[i+4].split(" ")]
        _grid[a*D+b] = 1

    for i in range(D):
        for j in range(D):
            for a in range(D):
                for b in range(D):
                    _mask[i*D*D*D+j*D*D+a*D+b] = (i-a)*(i-a) + (j-b)*(j-b)

    grid = np.array(_grid)
    mask = np.array(_mask).reshape(D*D,D*D)
    mask_rs = (mask <= Rs * Rs)[grid==1.,]
    mask_rp = (mask <= Rp * Rp) + np.identity(D*D) * 5.8827

with open(f'outputs/{name}.out', "r") as f:
    lines = [line for line in f.readlines() if line[0] != '#']
    M = int(lines[0])
    _x0 = [0] * (D*D)
    for i in range(M):
        a, b = [int(x) for x in lines[i+1].split(" ")]
        _x0[a*D+b] = 1.0
    x0 = np.array(_x0)

x = cp.Variable(D*D, boolean=True)
constraints = [mask_rs @ x >= 1]
objective = cp.sum(cp.quad_form(x, mask_rp))
prob = cp.Problem(cp.Minimize(objective), constraints)
result = prob.solve()
print(result)
print(x.value)
