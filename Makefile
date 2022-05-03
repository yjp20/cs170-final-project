default:
	g++-11 -std=c++17 -fopenmp -O3 main.cpp
gen:
	g++-11 -std=c++17 -O3 gen.cpp -o gen
opt_sol:
	./gen $(problem) > tmp/problem.lp
	~/SCIPOptSuite-8.0.0-Darwin/bin/scip -c "read tmp/problem.lp" -c "optimize" -c "write solution tmp/sol" -c "exit"
	cat tmp/sol | grep '^x' | D="$(D)" python3 tools/extract.py > tmp/out
	mv tmp/out outputs/$(problem).out
	./a.out $(problem) i
