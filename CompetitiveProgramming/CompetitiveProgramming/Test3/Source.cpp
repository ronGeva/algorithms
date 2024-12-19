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
#include <limits.h>

using namespace std;
typedef unsigned long long ull;
typedef unsigned long ul;
#define max(x, y) x > y ? x : y
#define min(x, y) x > y ? y : x

void ex1Dfs(unordered_map<ull, vector<ull>>& edges,
    unordered_set<ull>& snake_vertexes,
    ull parent, ull node, ull& cafe_counter, ull consec_snakes,
    ull max_consec_snakes)
{
    if (snake_vertexes.find(node) != snake_vertexes.end())
    {
        consec_snakes++;
        if (consec_snakes > max_consec_snakes)
            return;
    }
    else
    {
        consec_snakes = 0;
    }

    if (edges[node].size() == 1 && edges[node][0] == parent)
    {
        // leaf == cafe
        cafe_counter++;
        return;
    }

    for (ull child : edges[node])
    {
        if (child == parent)
            continue;

        ex1Dfs(edges, snake_vertexes, node, child, cafe_counter,
               consec_snakes, max_consec_snakes);
    }
}

void ex1()
{
    ull n, m;
    unordered_set<ull> snake_vertexes;
    unordered_map<ull, vector<ull>> edges;

    cin >> n;
    cin >> m;
    for (ull i = 1; i <= n; i++)
    {
        ull has_snake;
        cin >> has_snake;
        if (has_snake == 1)
        {
            snake_vertexes.insert(i);
        }
    }

    for (ull i=0; i < n - 1; i++)
    {
        ull source, dst;
        cin >> source >> dst;

        if (edges.find(source) == edges.end())
        {
            edges[source] = {};
        }
        if (edges.find(dst) == edges.end())
        {
            edges[dst] = {};
        }

        edges[source].push_back(dst);
        edges[dst].push_back(source);
    }

    ull cafe_counter = 0;
    ex1Dfs(edges, snake_vertexes, 0, 1, cafe_counter, 0, m);

    cout << cafe_counter << endl;
}

bool ex2IsDragonDead(vector<ull>& seconds, ull h, ull k)
{
    ull damage_done = k;
    for (ull i = 1; i < seconds.size(); i++)
    {
        ull diff = seconds[i] - seconds[i - 1];
        if (diff < k)
        {
            damage_done -= (k - diff);
        }
        damage_done += k;
    }

    return damage_done >= h;
}

ull ex2BinarySearch(vector<ull>& seconds, ull h)
{
    ull start = 0;
    ull end = h + 1;
    ull k;
    ull smallest_k_that_kills = h;

    while (start < end)
    {
        k = (start + end) / 2;

        if (!ex2IsDragonDead(seconds, h, k))
        {
            start = k + 1;
        }
        else
        {
            if (k < smallest_k_that_kills)
            {
                smallest_k_that_kills = k;
            }

            end = k;
        }
    }

    return smallest_k_that_kills;
}

void ex2()
{
    ull t, n, h;

    cin >> t;

    vector<ull> results;
    while (t--)
    {
        cin >> n >> h;
        vector<ull> seconds;
        for (ull i = 0; i < n; i++)
        {
            ull second;
            cin >> second;
            seconds.push_back(second);
        }

        ull k = ex2BinarySearch(seconds, h);
        results.push_back(k);
    }

    for (ull k : results)
    {
        cout << k << endl;
    }
}

bool ex5TopSortDfs(ull node,
    unordered_map<ull, unordered_set<ull>>& edges,
    unordered_map<ull, ul>& v, stack<ull>& sorted_vertices)
{
    // detect loops
    if (v[node] == 1)
        return false;

    if (v[node])
        return true;

    v[node] = 1;
    for (ull neighbor : edges[node])
    {
        if (!ex5TopSortDfs(neighbor, edges, v, sorted_vertices))
            return false;
    }
    v[node] = 2;
    sorted_vertices.push(node);
    return true;
}

bool ex5TopSortDfs(ull n,
    unordered_map<ull, unordered_set<ull>>& edges,
    stack<ull>& sorted_vertices)
{
    unordered_map<ull, ul> v;
    for (ull i = 1; i <= n; i++)
    {
        v[i] = 0;
    }

    for (ull i = 1; i <= n; i++)
    {
        if (v[i])
            continue;

        if (!ex5TopSortDfs(i, edges, v, sorted_vertices))
            return false;
    }

    return true;
}

void ex5()
{
    ull n, m;
    cin >> n >> m;

    unordered_map<ull, char> letters;

    for (ull i = 1; i <= n; i++)
    {
        char letter;
        cin >> letter;

        letters[i] = letter;
    }

    unordered_map<ull, unordered_set<ull>> edges;
    unordered_map<ull, unordered_set<ull>> reverse_edges;
    for (ull i = 0; i < m; i++)
    {
        ull source, dst;
        cin >> source >> dst;

        if (source == dst)
        {
            // infinite loop detected
            cout << -1 << endl;
            return;
        }

        if (edges.find(source) == edges.end())
        {
            edges[source] = {};
        }
        // set, ignore duplicates
        edges[source].insert(dst);

        if (reverse_edges.find(dst) == reverse_edges.end())
        {
            reverse_edges[dst] = {};
        }
        reverse_edges[dst].insert(source);
    }

    stack<ull> sorted_vertices;
    if (!ex5TopSortDfs(n, edges, sorted_vertices))
    {
        cout << -1 << endl;
        return;
    }

    vector<ull> sorted_vertices_vec;
    while (!sorted_vertices.empty())
    {
        sorted_vertices_vec.push_back(sorted_vertices.top());
        sorted_vertices.pop();
    }

    unordered_map<ull, vector<ull>> max_letters;
    ull max_value = 1;
    for (ull vertex: sorted_vertices_vec)
    {
        max_letters[vertex] = { };
        for (char l = 'a'; l <= 'z'; l++)
        {
            max_letters[vertex].push_back(0);
        }

        char letter = letters[vertex];
        if (reverse_edges.find(vertex) == reverse_edges.end())
        {
            // root
            max_letters[vertex][letter - 'a'] = 1;
            continue;
        }

        for (ull parent : reverse_edges[vertex])
        {
            for (char l = 0; l < 26; l++)
            {
                max_letters[vertex][l] =
                    max(max_letters[vertex][l], max_letters[parent][l]);
            }
        }
        max_letters[vertex][letter - 'a']++;

        max_value = max(max_value, max_letters[vertex][letter - 'a']);
    }

    cout << max_value << endl;
}

ull ex3Gcd(ull a, ull b)
{
    if (a == 0)
        return b;
    return ex3Gcd(b % a, a);
}

pair<ull, ull> ex3Dfs(unordered_map<ull, vector<ull>>& edges,
                      ull parent, ull node)
{
    if (edges.find(node) == edges.end())
    {
        // edge case for a single node
        // in any other case all nodes will have an edge
        return {0, 1};
    }

    if (edges[node].size() == 1 && edges[node][0] == parent)
    {
        // leaf
        return { 0, 1 };
    }

    vector<pair<ull, ull>> children_sum;
    for (ull child: edges[node])
    {
        if (child == parent)
            continue;

        children_sum.push_back(ex3Dfs(edges, node, child));
    }

    pair<ull, ull> sum = children_sum[0];
    for (ull i = 1; i < children_sum.size(); i++)
    {
        ull down = sum.second * children_sum[i].second;
        ull up = sum.first * children_sum[i].second +
            children_sum[i].first * sum.second;
        ull gcd = ex3Gcd(up, down);

        gcd = max(gcd, 1);
        sum.first = up / gcd;
        sum.second = down / gcd;
    }

    sum.second *= children_sum.size();
    return {sum.first + sum.second, sum.second};
}

void ex3()
{
    ull n;
    cin >> n;

    unordered_map<ull, vector<ull>> edges;
    for (ull i=0; i < n - 1; i++)
    {
        ull source, dst;
        cin >> source >> dst;

        if (edges.find(source) == edges.end())
        {
            edges[source] = {};
        }
        if (edges.find(dst) == edges.end())
        {
            edges[dst] = {};
        }

        edges[source].push_back(dst);
        edges[dst].push_back(source);
    }

    pair<ull, ull> result = ex3Dfs(edges, 0, 1);
    cout << (double)result.first / (double)result.second << endl;
}

void ex7()
{
    ull t, n, k;
    ull tmp;
    cin >> t;

    vector<ull> results;
    while (t--)
    {
        cin >> n >> k;
        vector<ull> a;
        ull s=0;
        for(ull i=0; i<n; i++){
            cin >> tmp;
            s += tmp;
            a.push_back(tmp);
        }
        sort(a.begin(), a.end());
        ull u=0;
        ull d, i;
        for(i=0; i<n-1; i++){
            d = (a[i + 1] - a[i]) * (i+1);
            if(u+d>=k){
                break;
            }
            u += d;
        }
        for(ull j=0;j<i;j++){
            a[j] = a[i];
        }
        d = k - u;
        ull rest = d / (i + 1);
        ull r = d % (i + 1);
        for(ull j=0;j<=i;j++){
            a[j] += rest;
            d -= rest;
        }
        for(ull j=0;j<r;j++){
            a[j] += 1;
        }

        s += k;

        ull ws=0;
        for(ull i=0;i<n;i++){
            s -= a[i];
            ws = (ws + (s * a[i])% ull(10e9+7)) % ull(10e9+7);
        }


        results.push_back(ws);
    }

    for (ull k : results)
    {
        cout << k << endl;
    }
}

int main()
{
    ex3();
	return 0;
}