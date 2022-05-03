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

/* #define h 1 */
#define d(...) " [" << #__VA_ARGS__ ": " << (__VA_ARGS__) << "] "
using namespace std;
typedef long long ll;

const int ITER = 5e3;
const int BATCH_SIZE = 512;
const int MAX = 1e2;
const int MAX_N = 2e2+5;
const int MAX_ELITE = 32;
const double DECAY = 0.980;
const double SPREAD = 0.00001;
const double START = 10;

char *path;
int N, D, Rs, Rp;

pair<int,int> points[MAX_N];
int order[MAX_N][MAX_N];
int grid[MAX][MAX];
int sol[MAX][MAX];

int gen[BATCH_SIZE][MAX][MAX];
int cover[BATCH_SIZE][MAX][MAX];
#ifdef h
double res[BATCH_SIZE][MAX][MAX];
#endif
int previous[MAX][MAX];

double best = 1e50;
double best_h = 1e50;
double gradient[MAX][MAX];
double past_resonance[MAX][MAX];

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

double score_h(int ans[MAX][MAX]) {
	double sum = 0;
	for (int x=0; x<D; x++) {
		for (int y=0; y<D; y++) {
			if (ans[x][y]) {
				double resonance = 0;
				cmap(x, y, (Rp+4)) {
					double dist = sqrt((x-i) * (x-i) + (y-j) * (y-j));
					resonance += ans[i][j] / (1.0 + 1.0*exp(9*(dist-Rp-0.1)));
				}
				sum += 170 * exp(0.170 * (resonance - 1));
			}
		}
	}
	return sum;
}

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

void load_previous() {
	char filepath[100];
	sprintf(filepath, "outputs/%s.out", path);
	FILE *f = fopen(filepath, "r");

	if (!f) {
		for (int i=0; i<D; i++) {
			for (int j=0; j<D; j++) {
				previous[i][j] = 1;
			}
		}
		return;
	}

	char c = fgetc(f);
	if (c == '#') fscanf(f, "%*[^\n]");
	else ungetc(c, f);

	int ct;
	fscanf(f, "%d", &ct);

	for (int i=0; i<ct; i++) {
		int a, b;
		fscanf(f, "%d %d", &a, &b);
		previous[a][b] = 1;
	}
}

void read_gradient() {
	char filepath[100];
	sprintf(filepath, "outputs/%s_gradient.csv", path);
	FILE *f = fopen(filepath, "r");
	load_previous();
	for (int i=0; i<D; i++) {
		int d;
		fscanf(f, "%d,", &d);
	}
	for (int i=0; i<D; i++) {
		for (int j=0; j<D; j++) {
			/* fscanf(f, "%lf,", &gradient[i][j]); */
			gradient[i][j] += 1;
			gradient[i][j] += 100 * previous[i][j];
		}
	}
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
	int st = rand() % N;

	for (int n=0; n<N; n++) {
		auto [x,y] = points[order[st][n]];
		if (cover[batch][x][y]) continue;

		double k[MAX][MAX];
		double sum = 0;

		cmap(x, y, 3) {
			double ct = 0;
			/* cmap2(i, j, 3) { */
			/* 	ct += grid[ii][jj] && !cover[batch][ii][jj]; */
			/* } */
			/* double intrinsic = exp(ct*2) * exp(-past_resonance[i][j]/8); */
			/* double intrinsic = exp(-ct/4); */
			double intrinsic = 1;
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
		points[i] = {a,b};
	}

	for (int st=0; st<N; st++) {
		order[st][0] = st;

		int used[MAX_N] = {0};
		used[st] = 1;

		for (int i=1; i<N; i++) {
			int best_point;
			int best = 1e9;
			for (int j=0; j<N; j++) {
				if (used[j]) continue;
				int least = 1e9;
				auto a = points[j];
				for (int k=0; k<N; k++) {
					if (!used[k]) continue;
					auto b = points[k];
					least = min(
							least,
							(a.first - b.first) * (a.first - b.first) +
							(a.second - b.second) * (a.second - b.second));
				}
				if (least < best) {
					best = least;
					best_point = j;
				}
			}
			used[best_point] = 1;
			order[st][i] = best_point;
		}
	}

	if (argc == 2) {
		for (int i=0; i<D; i++) {
			for (int j=0; j<D; j++) {
				gradient[i][j] = START;
			}
		}
	} else {
		read_gradient();
	}

	// sort(points, points+N);

	for (int t=0; t<=ITER; t++) {
		memset(gen, 0, sizeof gen);
		memset(cover, 0, sizeof cover);
#ifdef h
		memset(res, 0, sizeof res);
#endif

		double batch_best = 1e50;
		double batch_avg = 0;
		tuple<double,double,int> rank[BATCH_SIZE];

#pragma omp parallel for
		for (int b=0; b<BATCH_SIZE; b++) {
			solve(b);
			double s = score(gen[b]);
			double s_h = score_h(gen[b]);
			rank[b] = {s,s_h,b};
		}

		for (int i=0; i<BATCH_SIZE; i++) {
			auto [s, s_h, b] = rank[i];
			batch_best = min(batch_best, s);
			best_h = min(best_h, s_h);
			batch_avg += s;
		}

		double new_gradient[MAX][MAX];
		memset(past_resonance, 0, sizeof(past_resonance));

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
			auto [s, s_h, b] = rank[c];

			if (s < best || (s == best && s_h < best_h)) {
				best = s;
				best_h = s_h;
				for (int i=0; i<D; i++) {
					for (int j=0; j<D; j++) {
						sol[i][j] = gen[b][i][j];
					}
				}
			}

			double weight = pow(best / s, 3);

			for (int i=0; i<D; i++) {
				for (int j=0; j<D; j++) {
					past_resonance[i][j] += 1.0*gen[b][i][j] / MAX_ELITE;
					new_gradient[i][j] += 1.0*gen[b][i][j] / MAX_ELITE * weight;
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
		fprintf(stderr, "%5d: %13lf %13lf %13lf %13lf\n", t, batch_best, batch_avg/BATCH_SIZE, best, best_h);
	}

	load_previous();
	double prev = score(previous);
	cout << d(path) d(best) d(prev) << endl;
	if (best <= prev) {
		output_gradient();
		output_sol();
		output_progression();
	}

	return 0;
}
