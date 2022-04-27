// yjp20
// 2022 04

#include<omp.h>
#include<stdio.h>
#include<math.h>
#include<string.h>
#include<algorithm>
#include<iostream>
#include<queue>
#include<stack>
#include<vector>
#include<utility>
#include<random>

#define d(...) " [" << #__VA_ARGS__ ": " << (__VA_ARGS__) << "] "
using namespace std;
typedef long long ll;

const int ITER = 1e3;
const int BATCH_SIZE = 256;
const int MAX = 2e2;
const int MAX_ELITE = 8;
const double DECAY = 0.950;
const double SPREAD = 0.01;

char *path;
int N, D, Rs, Rp;

pair<int,int> points[BATCH_SIZE][220];
int grid[MAX][MAX];
int sol[MAX][MAX];

int gen[BATCH_SIZE][MAX][MAX];
int cover[BATCH_SIZE][MAX][MAX];
int res[BATCH_SIZE][MAX][MAX];

double best = 1e50;
double gradient[MAX][MAX];
vector<double> progression_best;
vector<double> progression_average;

random_device rd;
default_random_engine eng(rd());
std::mt19937 g(rd());

#define cmap(x, y, r) \
	for (int i=max(0,x-r); i<=min(D-1,x+r); i++) \
		for (int j=max(0,y-r); j<=min(D-1,y+r); j++) \
			if ( (x-i)*(x-i) + (y-j)*(y-j) <= r * r)

#define cmap2(x, y, r) \
	for (int ii=max(0,x-r); ii<=min(D-1,x+r); ii++) \
		for (int jj=max(0,y-r); jj<=min(D-1,y+r); jj++) \
			if ( (x-ii)*(x-ii) + (y-jj)*(y-jj) <= r * r)

double score(int ans[MAX][MAX]) {
	double sum = 0;
	for (int x=0; x<D; x++) {
		for (int y=0; y<D; y++) {
			if (ans[x][y]) {
				int resonance = 0;
				cmap(x, y, Rp) {
					resonance += ans[i][j];
				}
				sum += 170 * exp(0.170 * (resonance - 1));
			}
		}
	}
	return sum;
}

double score_previous() {
	char filepath[100];
	sprintf(filepath, "outputs/%s.out", path);
	FILE *f = fopen(filepath, "r");

	char c = fgetc(f);
	if (c == '#') fscanf(f, "%*[^\n]");
	else ungetc(c, f);

	int prev_sol[MAX][MAX] = {0};
	int ct;
	fscanf(f, "%d", &ct);

	for (int i=0; i<ct; i++) {
		int a, b;
		fscanf(f, "%d %d", &a, &b);
		prev_sol[a][b] = 1;
	}

	return score(prev_sol);
}

void output_gradient() {
	char filepath[100];
	sprintf(filepath, "outputs/%s_gradient.csv", path);
	FILE *g = fopen(filepath, "w");
	for (int i=0; i<D; i++) {
		if (i) fprintf(g, ",");
		fprintf(g, "%d", i);
	}
	fprintf(g, "\n");
	for (int i=0; i<D; i++) {
		for (int j=0; j<D; j++) {
			if (j) fprintf(g, ",");
			fprintf(g, "%lf", gradient[i][j]);
		}
		fprintf(g, "\n");
	}
}

void output_sol() {
	char filepath[100];
	sprintf(filepath, "outputs/%s.out", path);
	FILE *k = fopen(filepath, "w");
	fprintf(k, "# %lf\n", score(sol));
	int ct = 0;
	for (int i=0; i<D; i++) {
		for (int j=0; j<D; j++) {
			ct += sol[i][j];
		}
	}
	fprintf(k, "%d\n", ct);
	for (int i=0; i<D; i++) {
		for (int j=0; j<D; j++) {
			if (sol[i][j]) fprintf(k, "%d %d\n", i, j);
		}
	}
}

void output_progression() {
	char filepath[100];
	sprintf(filepath, "outputs/%s_progression.csv", path);
	FILE *g = fopen(filepath, "w");
	fprintf(g, "batch_average,batch_best\n");
	for (int i=0; i<ITER; i++) {
		fprintf(g, "%lf,%lf\n", progression_average[i], progression_best[i]);
	}
}

void solve(int batch) {
	std::shuffle(points[batch], points[batch]+N, g);
	/* sort(points[batch], points[batch] + N, [](auto a, auto b){ */
	/* 	return a.first * a.first + a.second * a.second < b.first * b.first + b.second * b.second; */
	/* }); */

	for (int n=0; n<N; n++) {
		auto [x,y] = points[batch][n];
		if (cover[batch][x][y]) continue;

		double k[MAX][MAX];
		double sum = 0;

		cmap(x, y, 3) {
			double ct = 0;
			cmap2(i, j, 3) {
				ct += grid[ii][jj] && !cover[batch][ii][jj];
			}
			double intrinsic = exp(ct/3)*2.0/(res[batch][x][y] + 1);
			sum += intrinsic * gradient[i][j];
			k[i][j] = sum;
		}

		std::uniform_real_distribution<double> distr(0, sum);
		double v = distr(eng);
		int cx, cy;

		cmap(x, y, 3) {
			if (v < k[i][j]) {
				cx = i;
				cy = j;
				goto k;
			}
		}

k:
		gen[batch][cx][cy] = 1;
		cmap(cx, cy, 3) {
			cover[batch][i][j] = 1;
		}
		cmap(cx, cy, 8) {
			res[batch][i][j] += 1;
		}
	}
}

char def[] = "default";

int main(int argc, char *argv[]) {
	path = argv[1] ? argv[1] : def;

	char filepath[100];
	sprintf(filepath, "inputs/%s.in", path);
	FILE *f = fopen(filepath, "r");

	char c = fgetc(f);
	if (c == '#') fscanf(f, "%*[^\n]");
	else ungetc(c, f);

	fscanf(f, "%d", &N);
	fscanf(f, "%d", &D);
	fscanf(f, "%d", &Rs);
	fscanf(f, "%d", &Rp);

	for (int i=0; i<N; i++) {
		int a, b;
		fscanf(f, "%d %d", &a, &b);
		grid[a][b] = 1;
		for (int j=0; j<BATCH_SIZE; j++) {
			points[j][i] = {a,b};
		}
	}

	for (int i=0; i<D; i++) {
		for (int j=0; j<D; j++) {
			gradient[i][j] = 1;
		}
	}

	// sort(points, points+N);

	for (int t=0; t<=ITER; t++) {
		memset(gen, 0, sizeof gen);
		memset(cover, 0, sizeof cover);
		memset(res, 0, sizeof res);
		#pragma omp parallel for
		for (int b=0; b<BATCH_SIZE; b++) {
			solve(b);
		}

		double batch_best = 1e50;
		double batch_avg = 0;

		pair<double,int> rank[BATCH_SIZE];

		for (int b=0; b<BATCH_SIZE; b++) {
			double s = score(gen[b]);
			rank[b] = {s,b};
			batch_avg += s;
			batch_best = min(batch_best, s);

			if (s < best) {
				best = s;
				for (int i=0; i<D; i++) {
					for (int j=0; j<D; j++) {
						sol[i][j] = gen[b][i][j];
					}
				}
			}
		}

		double new_gradient[MAX][MAX];

		for (int i=0; i<D; i++) {
			for (int j=0; j<D; j++) {
				new_gradient[i][j] = gradient[i][j] * (1.0-4*SPREAD);
				if (i != 0) new_gradient[i][j] += gradient[i-1][j] * SPREAD;
				if (j != 0) new_gradient[i][j] += gradient[i][j-1] * SPREAD;
				if (i != D-1) new_gradient[i][j] += gradient[i+1][j] * SPREAD;
				if (j != D-1) new_gradient[i][j] += gradient[i][j+1] * SPREAD;
			}
		}

		sort(rank, rank+BATCH_SIZE);

		for (int c=0; c<MAX_ELITE; c++) {
			auto [s, b] = rank[c];

			for (int i=0; i<D; i++) {
				for (int j=0; j<D; j++) {
					new_gradient[i][j] += gen[b][i][j];
				}
			}
		}

		for (int i=0; i<D; i++) {
			for (int j=0; j<D; j++) {
				gradient[i][j] = new_gradient[i][j] * DECAY;
			}
		}

		progression_best.push_back(batch_best);
		progression_average.push_back(batch_avg/BATCH_SIZE);
		fprintf(stderr, "%5d: %12lf %12lf %12lf\n", t, batch_best, batch_avg/BATCH_SIZE, best);
	}

	double prev = score_previous();
	cout << d(path) d(best) d(prev) << endl;
	if (best < prev) {
		output_gradient();
		output_sol();
		output_progression();
	}

	return 0;
}
