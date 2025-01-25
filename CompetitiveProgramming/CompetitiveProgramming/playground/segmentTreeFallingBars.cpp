// author : sentheta
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

#define Int long long
#define V vector
#define pii pair<int,int>
#define ff first
#define ss second

#define msb(x) (63-__builtin_clzll(x))
#define popcount(x) (__builtin_popcountll(x))

#define rep(i,a,b) for(int i=(int)(a); i<(int)(b); i++)
#define min(x, y) x > y ? y : x

#define cerr if(0) cout

#define int long long
// const Int MOD = 1e9+7;
const Int MOD = 998244353;
Int bpow(Int a, Int b) {
	Int ret = 1;
	for (; b; a = a * a % MOD, b /= 2) if (b & 1) ret = ret * a % MOD;
	return ret;
}
Int inv(Int a) { return bpow(a, MOD - 2); }

const int N = 2e5 + 5;

int h, w, n;

int r[N], c[N], l[N];

#define mid (tl+tr)/2
#define lc (v+1)
#define rc (v+2*(mid-tl+1))
struct Segtree {
	int st[2 * N], lz[2 * N];

	void reset() {
		rep(v, 0, 2 * N) st[v] = h + 1, lz[v] = 0;
		cerr << st[0] << endl;
	}

	void upd(int l, int r, int x, int v = 0, int tl = 1, int tr = w) {
		if (r < tl || tr < l)
			return;

		if (l <= tl && tr <= r) {
			st[v] = min(st[v], x);
			lz[v] = x;
			return;
		}

		if (lz[v]) {
			upd(1, w, lz[v], lc, tl, mid);
			upd(1, w, lz[v], rc, mid + 1, tr);
			lz[v] = 0;
		}

		upd(l, r, x, lc, tl, mid);
		upd(l, r, x, rc, mid + 1, tr);
		st[v] = min(st[lc], st[rc]);
	}

	int qry(int l, int r, int v = 0, int tl = 1, int tr = w) {
		if (r < tl || tr < l)
			return h + 1;

		if (l <= tl && tr <= r)
			return st[v];

		if (lz[v]) {
			upd(1, w, lz[v], lc, tl, mid);
			upd(1, w, lz[v], rc, mid + 1, tr);
			lz[v] = 0;
		}

		return min(qry(l, r, lc, tl, mid), qry(l, r, rc, mid + 1, tr));
	}
} segtree;

void solve() {

	cin >> h >> w >> n;
	cerr << "h: " << h << endl;

	vector<int> ord(n);
	rep(i, 0, n) {
		cin >> r[i] >> c[i] >> l[i];
		ord[i] = i;
	}
	std::sort(ord.begin(), ord.end(), [&](int i, int j) {
		return r[i] > r[j];
		});

	segtree.reset();
	cerr << "segtree.qry(8,15) " << segtree.qry(8, 15) << endl;
	for (int i : ord) {
		cerr << "i r[i] c[i] l[i] " << i + 1 << " " << r[i] << " " << c[i] << " " << l[i] << endl;
		cerr << "segtree.qry(...) " << segtree.qry(c[i], c[i] + l[i] - 1) << endl;
		r[i] = segtree.qry(c[i], c[i] + l[i] - 1) - 1;
		cerr << "r[i] " << r[i] << endl;
		segtree.upd(c[i], c[i] + l[i] - 1, r[i]);
	}

	rep(i, 0, n) {
		cout << r[i] << '\n';
	}
}