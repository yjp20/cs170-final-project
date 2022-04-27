default:
	g++-11 -std=c++17 -fopenmp main.cpp
small:
	./a.out < project-sp22-skeleton/inputs/small/001.in
medium:
	./a.out < inputs/medium.in
plot_progression:
	python3 plot_prog.py
plot_gradient:
	python3 plot.py
plot_solution:
	python3 project-sp22-skeleton/python/visualize.py $(input) --with-solution sol.out out.svg
	open -a "Google Chrome" out.svg
