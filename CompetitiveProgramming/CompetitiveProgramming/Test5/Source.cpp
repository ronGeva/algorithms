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

using namespace std;
typedef unsigned long long ull;
typedef unsigned long ul;
typedef long long ll;
#define UL_MAX 0xffffffff
#define ULL_MAX 0xffffffffffffffff
#define max(x, y) x > y ? x : y
#define min(x, y) x > y ? y : x
#define rep(i, n) for(ull i = 1; i <= n; i++)

using Edge = pair<ull, ull>;
using Graph = vector<vector<Edge>>;

// Custom hash function for Edge
struct EdgeHash {
    std::size_t operator()(const Edge& edge) const {
        return std::hash<ull>()(edge.first) ^ std::hash<ull>()(edge.second);
    }
};

// Dijkstra's algorithm
ull dijkstra(const Graph& graph, ull source, ull destination) {
    // Priority queue to store (distance, vertex)
    priority_queue<pair<ull, ull>, vector<pair<ull, ull>>, greater<>> pq;

    // Distance map to track the shortest distance to each vertex
    vector<ull> distances(graph.size(), ULLONG_MAX);
    distances[source] = 0;

    // Parent map for reconstructing the path
    unordered_map<ull, ull> parent;
    parent[source] = source;

    pq.push({ 0, source }); // Start with the source node

    while (!pq.empty()) {
        ull current_distance = pq.top().first;
        ull current_vertex = pq.top().second;
        pq.pop();

        // If we reached the destination, stop
        if (current_vertex == destination) break;

        // Iterate over all neighbors of the current vertex
        for (const auto& edge : graph[current_vertex]) {
            ull neighbor = edge.first;
            ull weight = edge.second;

            // Calculate new distance to the neighbor
            ull new_distance = current_distance + weight;
            if (new_distance < distances[neighbor]) {
                distances[neighbor] = new_distance;
                pq.push({ new_distance, neighbor });
                parent[neighbor] = current_vertex;
            }
        }
    }

    return distances[destination];
}

void ex1()
{

    ul n, m;
    cin >> n >> m;

    vector<vector<Edge>> edges(n+1, vector<Edge>{});

    rep(i, m)
    {
        ul src, dst;
        ull weight;

        cin >> src >> dst >> weight;

        edges[src].push_back({dst, weight});
    }

    cout << dijkstra(edges, 1, n) << endl;
}

void ex2(){
    ll n, k;
    cin >> n >> k;
    vector<ll> prices(n + 1);
    rep(i, n) {
        cin >> prices[i];
    }

    vector<vector<ll>>  dp_sell(n+1, vector<ll>(k+1, 0));
    vector<vector<ll>>  dp_buy(n+1, vector<ll>(k+1, 0));

    // initialize
    for (ul i = 1; i <= k; i++)
    {
        dp_sell[n][i] = prices[n];
        dp_buy[n][i] = -prices[n];
    }
    dp_sell[n][0] = prices[n];

    for (ll day = n - 1; day > 0; day--)
    {
        for (ll i = 0; i <= k; i++)
        {
            // fill sell

            // option a - sell today
            ll option_a = prices[day] + dp_buy[day + 1][i];
            // option b - don't sell today
            ll option_b = dp_sell[day + 1][i];
            dp_sell[day][i] = max(option_a, option_b);

            // fill buy

            // option a - buy today
            if (i == 0)
            {
                // we can't buy today
                option_a = -0xffffff;
            }
            else
            {
                // we can buy today
                option_a = -prices[day] + dp_sell[day + 1][i - 1];
            }
            
            // option b - don't buy today
            option_b = dp_buy[day + 1][i];

            dp_buy[day][i] = max(option_a, option_b);
        }
        
    }

    ll best_trade = 0;
    for (ll i=0; i<=k; i++)
    {
        best_trade = max(best_trade, dp_buy[1][i]);
    }

    cout << best_trade << endl;

}

// returns the distances to all other vertices
vector<ull> ex3Dijkstra(const Graph& graph, ull source) {
    // Priority queue to store (distance, vertex)
    priority_queue<pair<ull, ull>, vector<pair<ull, ull>>, greater<>> pq;

    // Distance map to track the shortest distance to each vertex
    vector<ull> distances(graph.size(), ULLONG_MAX);
    distances[source] = 0;

    // Parent map for reconstructing the path
    unordered_map<ull, ull> parent;
    parent[source] = source;

    pq.push({ 0, source }); // Start with the source node

    while (!pq.empty()) {
        ull current_distance = pq.top().first;
        ull current_vertex = pq.top().second;
        pq.pop();

        // Iterate over all neighbors of the current vertex
        for (const auto& edge : graph[current_vertex]) {
            ull neighbor = edge.first;
            ull weight = edge.second;

            // Calculate new distance to the neighbor
            ull new_distance = current_distance + weight;
            if (new_distance < distances[neighbor]) {
                distances[neighbor] = new_distance;
                pq.push({ new_distance, neighbor });
                parent[neighbor] = current_vertex;
            }
        }
    }

    return distances;
}

void ex3()
{
    ul t;
    cin >> t;

    rep(i, t)
    {
        ul n, m;
        cin >> n >> m;

        vector<vector<pair<ull, ull>>> edges(n+1, vector<pair<ull, ull>>{});
        rep(e, m)
        {
            ul u, v, w;
            cin >> u >> v >> w;

            edges[u].push_back({v, w});
            edges[v].push_back({u, w});
        }

        vector<ul> slowness(n+1, 0);
        rep(c, n)
        {
            ul slow_factor;
            cin >> slow_factor;

            slowness[c] = slow_factor;
        }

        vector<vector<pair<ull, ull>>> new_edges(n+1, vector<pair<ull, ull>>{});
        rep (c, n)
        {
            auto distances = ex3Dijkstra(edges, c);
            vector<pair<ull, ull>> current_edges;
            rep (d, distances.size() - 1)
            {
                current_edges.push_back({d, distances[d] * slowness[c]});
            }
            new_edges[c] = std::move(current_edges);
        }

        auto final_distances = ex3Dijkstra(new_edges, 1);
        cout << final_distances[n] << endl;
    }
}

void ex8InitializeSegmentTree(vector<ll>& input, vector<ll>& tree)
{
    ul n = (ul)input.size();
    for (ul i = 0; i < n; i++)
    {
        tree[n + i] = input[i];
    }

    //for (ll i = n - 1; i > 0; i--)
    //{
    //    tree[i] = tree[2 * i] + tree[2 * i + 1];
    //}
}

void ex8AddToTree(vector<ll>& tree, ll addition, ll l, ll r, ll start, ll end, ll location)
{
    // if intersection is none return
    if (end < l || start > r)
        return;
    
    // we're contained in the range
    if (start <= l && r <= end)
    {
        tree[location] += addition;
        return;
    }

    // we have an intersection but it isn't complete, recursively enter sons

    // left son
    ll mid = (r + l) / 2;
    ex8AddToTree(tree, addition, l, mid, start,end, location * 2);

    // right son
    ex8AddToTree(tree, addition, mid + 1, r, start, end, location * 2 + 1);
}

ll ex8QueryValue(vector<ll>& tree, ll i)
{
    ll n = tree.size() / 2;
    ll res = 0;
    for (ll index = i + n; index > 0; index /= 2)
    {
        res += tree[index];
    }

    return res;
}

void ex8()
{
    ul N, M;
    cin >> N >> M;

    vector<ll> A(N, 0);
    for (ul i = 0; i < N; i++)
    {
        cin >> A[i];
    }

    vector<ll> B(M+1, 0);
    for (ul i = 1; i <= M; i++)
    {
        cin >> B[i];
    }

    vector<ll> tree(2 * N, 0);
    ex8InitializeSegmentTree(A, tree);

    ll value = ex8QueryValue(tree, 2);

    ex8AddToTree(tree, 100, 0, N - 1, 1, 3, 1);

    ll valueAfterChange = ex8QueryValue(tree, 2);
    return;
}

int main()
{
    ex8();
    return 0;
}
