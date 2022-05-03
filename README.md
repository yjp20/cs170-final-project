- Main build: `make`
- Run main approximation: `./a.out {problem}` like `./a.out medium/109`
- Run improvement on main approximation: `./a.out {problem} i` like `./a.out medium/109 i`

## Files

- `main.cpp`: Main ACO algorithm that does most of the heavy lifting
- `gen.cpp`: Generates an cplex lp format ilp problem for minimum geometric covering (helped in solving a small fraction of problems)
- `quad.zpl`: Quadratic-integer formulation of the problem in ZIMPL (useful for finding optimal outputs in some small test cases)
- `problem.zpl`: MINLP formulation of the problem in ZIMPL which can be parsed by SCIP
- `ipopt.py`: Experimental Ipopt implementation of the NLP formulation of this program
- `cvx.py`: Another experimental implementation in cvxpy

- `vis`: svelte web application to visualize and diagnose problems
- `tools`: directory which contains some scripts to help run things (notably `find_improvable`, `runner_todo`)
