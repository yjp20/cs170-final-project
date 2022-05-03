import jax.numpy as np
from jax import jit, grad, jacfwd, jacrev
from cyipopt import minimize_ipopt
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
    mask_rp = mask <= Rp * Rp

with open(f'outputs/{name}.out', "r") as f:
    lines = [line for line in f.readlines() if line[0] != '#']
    M = int(lines[0])
    _x0 = [0] * (D*D)
    for i in range(M):
        a, b = [int(x) for x in lines[i+1].split(" ")]
        _x0[a*D+b] = 1.0
    x0 = np.array(_x0)

def objective(x):
    return 143.42301882139 * np.sum(np.exp(0.170 * np.dot(mask_rp, x)) * x)

def ineq_constraints(x, u=None):
    return np.dot(mask_rs, x) - 1

obj_jit = jit(objective)
con_ineq_jit = jit(ineq_constraints)

# build the derivatives and jit them
obj_grad = jit(grad(obj_jit))  # objective gradient
obj_hess = jit(jacrev(jacfwd(obj_jit))) # objective hessian
con_ineq_jac = jit(jacfwd(con_ineq_jit))  # jacobian
con_ineq_hess = jacrev(jacfwd(con_ineq_jit))  # hessian
con_ineq_hessvp = jit(lambda x, v: con_ineq_hess(x) * v[0])

cons = [{'type': 'ineq', 'fun': con_ineq_jit, 'jac': con_ineq_jac, 'hess': con_ineq_hess}]
bnds = [(0, 1) for _ in range(x0.size)]

res = minimize_ipopt(obj_jit, jac=obj_grad, hess=obj_hess, x0=x0, bounds=bnds, constraints=cons, options={'disp': 5})

# print(objective(x0))
# print(ineq_constraints(x0))

