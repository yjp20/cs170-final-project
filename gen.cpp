// yjp20
// 2022 04

#include<stdio.h>
#include<math.h>
#include<string.h>
#include<algorithm>
#include<iostream>
#include<queue>
#include<stack>
#include<vector>
#include<utility>

#define d(...) " [" << #__VA_ARGS__ ": " << (__VA_ARGS__) << "] "
using namespace std;
typedef long long ll;

#define cmap(x, y, r) \
	for (int i=max(0,x-r); i<=min(D-1,x+r); i++) \
		for (int j=max(0,y-r); j<=min(D-1,y+r); j++) \
			if ( (x-i)*(x-i) + (y-j)*(y-j) <= r * r)

const int MAX = 100;
int N, D, Rs, Rp;
int grid[MAX][MAX];

int main(int argc, char **argv) {
	char *path = argv[1];
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
	}

	cout << "Minimize" << endl;
	cout << " obj: ";
	for (int x=0; x<D; x++) {
		for (int y=0; y<D; y++) {
			if (x || y) cout << " + ";
			cout << "170 x" << x*D + y;
			/* cout << "28.9 r" << x*D + y << " + "; */
		}
	}
	cout << endl;

	cout << "Subject To" << endl;
	for (int x=0; x<D; x++) {
		for (int y=0; y<D; y++) {
			if (!grid[x][y]) continue;
			printf(" c%d: ", x*D + y);
			bool first = true;
			cmap(x, y, 3) {
				if (!first) cout << "+";
				first = false;
				cout << "x" << i*D + j;
			}
			cout << ">=1" << endl;
		}
	}

	for (int x=0; x<D; x++) {
		for (int y=0; y<D; y++) {
			printf(" v%d: ", x*D+y);
			bool first = true;
			cmap(x, y, Rp) {
				if (!first) cout << "+";
				first = false;
				cout << "x" << i*D+j;
			}
			cout << "-r" << x*D+y << "=0" << endl;
		}
	}

	cout << "Binary" << endl;
	for (int x=0; x<D; x++) {
		for (int y=0; y<D; y++) {
			cout << " x" << x*D + y << endl;
		}
	}

	cout << "General" << endl;
	for (int x=0; x<D; x++) {
		for (int y=0; y<D; y++) {
			cout << " r" << x*D + y << endl;
		}
	}

	cout << "End" << endl;

	return 0;
}
