#include <vector>
#include <algorithm>
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
using namespace std;

void swap(int& X, int& Y)
{
    int tmp = X;
    X = Y;
    Y = tmp;
}

//DSU
struct dsu
{
    vector<int> parent;
    vector<int> sz;

    dsu(int n)
    {
        parent = vector<int>(n);

        for (int i = 0; i < n; i++)
        {
            parent[i] = i;
        }

        sz = vector<int>(n, 1);
    }

    int find_(int x)
    {
        int root = x;
        while (root != parent[root])
            root = parent[root];

        //Path compression
        while (parent[x] != root)
        {
            int p = parent[x];
            parent[x] = root;
            x = p;
        }
        return root;
    }

    int size_(int root)
    {
        return sz[root];
    }

    bool union_(int x, int y)
    {
        int X = find_(x);
        int Y = find_(y);
        // x and y are already in the same set
        if (X == Y)
            return false;
        // x and y are not in same set, so we merge them
        if (sz[X] < sz[Y])
            swap(X, Y);
        // merge yRoot into xRoot
        parent[Y] = X;
        sz[X] += sz[Y];
        return true;
    }
};

int fallen_bricks(dsu& disjoint_set, int ceiling_vertex, int total_bricks)
{
    int stable_bricks = disjoint_set.size_(disjoint_set.find_(ceiling_vertex));

    // ignore ceiling vertex
    stable_bricks--;

    return total_bricks - stable_bricks;
}

vector<int> hitBricks(vector<vector<int>>& grid, vector<vector<int>>& hits) {
    int total_bricks = 0;
    rep0(i, grid.size())
    {
        rep0(j, grid[0].size())
        {
            if (grid[i][j] == 1)
                total_bricks++;
        }
    }

    // apply all hits
    for (vector<int>& hit : hits)
    {
        int row = hit[0];
        int col = hit[1];

        // nothing happens, mark the hit as redundant for our reverse walkthrough
        if (grid[row][col] == 0)
        {
            hit[0] = -1;
            continue;
        }

        total_bricks--;

        // clear the brick
        grid[row][col] = 0;
    }

    // there are up to N * M + 1 vertices in the graph, the +1 is for the "ceiling" vertex
    // Arbitrarily treat N*M as the ceiling vertex
    int ceiling_vertex = grid.size() * grid[0].size();
    dsu disjoint_set(ceiling_vertex + 1);
    int N = (int)grid.size();
    int M = (int)grid[0].size();

    for (int row = 0; row < N; row++)
    {
        for (int col = 0; col < M; col++)
        {
            if (grid[row][col] != 1)
                continue;

            // all top row is connected to the ceiling vertex
            if (row == 0 )
            {
                disjoint_set.union_(row * M + col, ceiling_vertex);
            }

            // only need to look right and down since we move from top to bottom and left to right
            if (col < M - 1 && grid[row][col+1] == 1)
            {
                disjoint_set.union_(row * M + col, row * M + col + 1);
            }
            if (row < N - 1 && grid[row + 1][col] == 1)
            {
                disjoint_set.union_(row * M + col, (row + 1) * M + col);
            }
        }
    }

    vector<int> bricks_fallen = vector<int>(hits.size());
    bricks_fallen[hits.size() - 1] =
        fallen_bricks(disjoint_set, ceiling_vertex, total_bricks);
    for (int i = (int)hits.size() - 1; i > 0; i--)
    {
        if (hits[i][0] == -1)
        {
            // a false hit - nothing new fell
            //bricks_fallen[i - 1] = fallen_bricks(disjoint_set, ceiling_vertex, total_bricks);
            bricks_fallen[i - 1] = 0;
            continue;
        }

        total_bricks++;
        int row = hits[i][0];
        int col = hits[i][1];
        grid[row][col] = 1;

        if (row == 0)
        {
            disjoint_set.union_(ceiling_vertex, row* M + col);
        }

        // left
        if (col > 0 && grid[row][col - 1] == 1)
        {
            disjoint_set.union_(row * M + col, row * M + col - 1);
        }
        // right
        if (col < M - 1 && grid[row][col + 1] == 1)
        {
            disjoint_set.union_(row * M + col, row * M + col + 1);
        }
        // up
        if (row > 0 && grid[row - 1][col] == 1)
        {
            disjoint_set.union_(row * M + col, (row - 1) * M + col);
        }
        // down
        if (row < N - 1 && grid[row + 1][col] == 1)
        {
            disjoint_set.union_(row * M + col, (row + 1) * M + col);
        }

        bricks_fallen[i - 1] =
            fallen_bricks(disjoint_set, ceiling_vertex, total_bricks);
    }

    for (int i = (int)bricks_fallen.size() - 1; i > 0; i--)
    {
        if (bricks_fallen[i] < bricks_fallen[i] - 1)
        {
            bricks_fallen[i] = 0; // nothing new fell
        }
        else
        {
            bricks_fallen[i] -= bricks_fallen[i - 1];
        }
    }
    return bricks_fallen;
}

int _main()
{
    vector<vector<int>> grid = { {1, 0, 1}, {0, 0, 1} };
    vector<vector<int>> hits = { {1, 0} ,{0, 0} };

    auto res = hitBricks(grid, hits);

    return 0;
}