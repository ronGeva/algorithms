#include <vector>
#include <map>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <unordered_map>

using namespace std;
using ll = long long;

const ll INF = 1e9;

void solve() {
    ll a, b, c, d, e;
    cin >> a >> b >> c >> d >> e;
    vector<ll> scores{ a, b, c, d, e };
    unordered_map<ll, vector<string>> m;
    for (ll i = 0; i <= 2718 * 5; i++) {
        m[i] = {};
    }
    for (ll i = 1; i <= 31; i++) {
        ll current_score = 0;
        ll score_idx = 0;
        for (ll j = 1; j <= 16; j = 2) {
            if (i & j) current_score += scores[score_idx];
            score_idx++;
        }
        string s = "";
        if (i & 1) s += "A";
        if (i & 2) s += "B";
        if (i & 4) s += "C";
        if (i & 8) s += "D";
        if (i & 16) s += "E";
        m[current_score].push_back(s);
    }
    for (ll i = 27185; i >= 0; i--) {
        if (m[i].size() > 0) {
            sort(m[i].begin(), m[i].end());
            for (ll j = 0; j < m[i].size(); j++) {
                cout << m[i][j] << endl;
            }
        }
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    solve();
    return 0;
}