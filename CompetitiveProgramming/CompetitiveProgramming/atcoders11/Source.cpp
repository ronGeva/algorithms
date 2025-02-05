#include <iostream>
#include <string>
#include <stack>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <set>
#include <map>
#include <memory>
#include <queue>
#include <limits.h>
#include <cassert>

using namespace std;
typedef unsigned long long ull;
typedef unsigned long ul;
typedef long long ll;
#define UL_MAX 0xffffffff
#define ULL_MAX 0xffffffffffffffff
#define LL_MAX 0x7fffffffffffffff
#define max(x, y) x > y ? x : y
#define min(x, y) x > y ? y : x
#define rep(i, n) for(ull i = 1; i <= n; i++)
#define rep0(i, n) for(ull i = 0; i < n; i++)
#define repi(i, n) for(ull i = n; i > 0; i--)
#define repi0(i, n) for(ll i = n - 1; i >= 0; i--)

void ex1()
{
    string s;
    cin >> s;

    unordered_map<string, string> m;
    m["N"] = "S";
    m["S"] = "N";

    m["E"] = "W";
    m["W"] = "E";

    m["NE"] = "SW";
    m["SW"] = "NE";

    m["NW"] = "SE";
    m["SE"] = "NW";

    cout << m[s] << endl;
}


void ex4()
{
    ull n, w;
    cin >> n >> w;

    vector<pair<ull, ull>> blocks = vector<pair<ull, ull>>(n + 1, {0, 0});

    // columns[i] = <the ordered set of blocks in the column, in ascending order>
    vector<set<tuple<ull, ull>>> columns = vector<set<tuple<ull, ull>>>(w + 1, set<tuple<ull, ull>>{});
    rep(i, n)
    {
        ull x, y;
        cin >> x >> y;

        blocks[i] = { x, y };

        columns[x].insert({ y, i });
    }

    ull q;
    cin >> q;
    vector<tuple<ull, ull, ull>> queries = vector<tuple<ull, ull, ull>>(q + 1, { 0, 0, 0 });
    rep(i, q)
    {
        ull t, a;
        cin >> t >> a;

        queries[i] = { t, a, i };
    }

    // sort the queries by time
    sort(queries.begin(), queries.end());

    // true == yes
    // false == no
    vector<bool> results = vector<bool>(q + 1, false);

    // go through the queries by order and calcualte the state of the blocks for each one
    ull current_time = 1;
    vector<bool> block_deleted = vector<bool>(n + 1, false);
    vector<ull> pending_deletion = vector<ull>(w + 1, 0);
    bool no_more_clears = false;
    rep(i, q)
    {
        ull time = get<0>(queries[i]);
        ull block = get<1>(queries[i]);
        ull result_index = get<2>(queries[i]);

        while (current_time <= time)
        {
            // delete all pending deletion items
            for (ull item : pending_deletion)
            {
                block_deleted[item] = true;
            }

            // compute next line clear
            ull time_for_next_clear = 0;
            rep(j, w)
            {
                if (columns[j].empty())
                {
                    // don't compute anything anymore
                    time_for_next_clear = ULL_MAX;
                    break;
                }

                auto closest_to_floor = *prev(columns[j].rend());
                ull distance_from_floor_at_start = get<0>(closest_to_floor);

                // every clear stops us for one round
                ull time_to_reach_floor = distance_from_floor_at_start;

                ull item_id = get<1>(closest_to_floor);
                time_for_next_clear = max(time_for_next_clear, distance_from_floor_at_start);

                pending_deletion[j] = item_id;

                // remove block from column
                columns[j].erase(closest_to_floor);
            }
            current_time = time_for_next_clear;
        }

        results[result_index] = !block_deleted[block];
    }

    rep(i, q)
    {
        if (results[i])
        {
            cout << "Yes" << endl;
        }
        else
        {
            cout << "No" << endl;
        }
    }
}

ull ex6Value(vector<tuple<ull, ull>>& A, vector<tuple<ull, ull>>& B, vector<tuple<ull, ull>>& C,
    ull AIndex, ull BIndex, ull CIndex)
{
    return get<0>(A[AIndex]) * get<0>(B[BIndex]) + get<0>(B[BIndex]) * get<0>(C[CIndex]) +
        get<0>(C[CIndex]) * get<0>(A[AIndex]);
}

void ex6()
{
    ull N, K;
    cin >> N >> K;

    vector<tuple<ull, ull>> A = vector<tuple<ull, ull>>(N, { 0, 0 });
    vector<tuple<ull, ull>> B = vector<tuple<ull, ull>>(N, { 0, 0 });
    vector<tuple<ull, ull>> C = vector<tuple<ull, ull>>(N, { 0, 0 });

    rep0(i, N)
    {
        ull val;
        cin >> val;
        A[i] = { val, i };
    }
    sort(A.begin(), A.end());

    rep0(i, N)
    {
        ull val;
        cin >> val;
        B[i] = { val, i };
    }
    sort(B.begin(), B.end());

    rep0(i, N)
    {
        ull val;
        cin >> val;
        C[i] = { val, i };
    }
    sort(C.begin(), C.end());

    ull current = 1;
    ull result = 0;

    // each item is <value, A index, B index, C index>
    set<tuple<ull, ull, ull, ull>> candidates;
    candidates.insert({ ex6Value(A, B, C, N - 1, N - 1, N - 1), N - 1, N - 1, N - 1 });

    // generate the K biggest values
    while (current <= K)
    {
        auto best_itr = prev(candidates.end());
        tuple<ull, ull, ull, ull> best = *best_itr;
        candidates.erase(best_itr);

        result = get<0>(best);
        ull a = get<1>(best);
        ull b = get<2>(best);
        ull c = get<3>(best);

        // generate next candidates
        if (a > 0)
        {
            ull val = ex6Value(A, B, C, a - 1, b, c);
            candidates.insert({ val, a - 1, b, c });
        }
        if (b > 0)
        {
            ull val = ex6Value(A, B, C, a, b - 1, c);
            candidates.insert({ val, a, b - 1, c });
        }
        if (c > 0)
        {
            ull val = ex6Value(A, B, C, a, b, c - 1);
            candidates.insert({ val, a, b, c - 1 });
        }

        current++;
    }

    cout << result << endl;
}

int main()
{
    ex4();
    return 0;
}
