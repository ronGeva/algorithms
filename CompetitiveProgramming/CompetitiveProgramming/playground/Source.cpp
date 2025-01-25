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
#define max(x, y) x > y ? x : y
#define min(x, y) x > y ? y : x
#define rep(i, n) for(ull i = 1; i <= n; i++)
#define rep0(i, n) for(ull i = 0; i < n; i++)
#define repi(i, n) for(ull i = n; i > 0; i--)
#define repi0(i, n) for(ll i = n - 1; i >= 0; i--)

class SimpleSegTree {
public:
    SimpleSegTree(vector<ull>& arr)
    {
        tree = vector<ull>(arr.size() * 2, 0);
        n = static_cast<ull>(arr.size());

        // insert leaf nodes in tree
        for (ul i = 0; i < arr.size(); i++)
            tree[n + i] = arr[i];

        // build the tree by calculating parents
        for (ll i = n - 1; i > 0; --i)
            tree[i] = tree[i << 1] + tree[i << 1 | 1];
    }

    // function to update a tree node
    void update(ul p, ull value)
    {
        // set value at position p
        tree[p + n] = value;
        p = p + n;

        // move upward and update parents
        for (ul i = p; i > 1; i >>= 1)
            tree[i >> 1] = tree[i] + tree[i ^ 1];
    }

    ull query(ul l, ul r)
    {
        ull res = 0;

        // loop to find the sum in the range
        for (l += n, r += n; l < r; l >>= 1, r >>= 1)
        {
            if (l & 1)
                res += tree[l++];

            if (r & 1)
                res += tree[--r];
        }

        return res;
    }

private:
    vector<ull> tree;
    ul n;
};

// H - Mancala 2 (test 5) solution
//
// we need a data structure (some variation of a segment tree) that can do the following:
//
// Hold N items at the start.
// Do the following quickly (O(log(n)) is fine):
// 1. Add x to all items
// 2. Set value of a specific item to 0
// 3. Add 1 to items in range [start, end)
//
// Proposed solution - a segment tree in which tree(i) holds a number that needs to be added
// to 2i and 2i+1 in order to calcualte each of their value.
// that means that the value of the i-th item can be retrieved via the following algorithm:
// i = n + i
// while (i > 0) {
//      value += tree[i];
//      i >>= 1;
// }

class SegTreeRangeUpdates {
public:
    SegTreeRangeUpdates(vector<ull>& arr)
    {
        tree = vector<ll>(arr.size() * 2, 0);
        n = static_cast<ull>(arr.size());

        // insert leaf nodes in tree
        for (ul i = 0; i < arr.size(); i++)
            tree[n + i] = arr[i];

        // the "additions" kept in the parents are initialized to 0
        // so there is no further work to do
    }

    ll query(ul index)
    {
        ll res = 0;
        for (ll i = index + n; i > 0; i >>= 1)
        {
            res += tree[i];
        }

        return res;
    }

    void clear(ul index)
    {
        ll total_value = query(index);
        ll value_in_leaf = tree[index + n];

        tree[index + n] = value_in_leaf - total_value;
    }

    void addToRange(ul l, ul r, ll addition)
    {
        // loop to find the sum in the range
        for (l += n, r += n; l < r; l >>= 1, r >>= 1)
        {
            if (l & 1)
                tree[l++] += addition;

            if (r & 1)
                tree[--r] += addition;
        }
    }

private:
    vector<ll> tree;
    ul n;
};

void testSegRangeUpdates()
{
    vector<ull> arr = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    SegTreeRangeUpdates tree(arr);

    for (ul i = 0; i < arr.size(); i++)
    {
        ll value = tree.query(i);
        assert(value == arr[i]);
    }
    
    tree.clear(3);
    ll cleared_value = tree.query(3);
    assert(cleared_value == 0);

    tree.addToRange(0, 4, 50);
    ll a1 = tree.query(0);
    ll a2 = tree.query(1);
    ll a3 = tree.query(2);
    ll a4 = tree.query(3);
    assert(a1 == 51);
    assert(a2 == 52);
    assert(a3 == 53);
    assert(a4 == 50);

    tree.clear(1);
    assert(tree.query(1) == 0);

    tree.addToRange(1, 7, 1000);
    ll a5 = tree.query(5);
    assert(a5 == 1006);
}

// F - Falling bars (atcoders question)
//
// Another segment tree question.
//
// To solve it we need the following features:
// 1. Add +1 to range [x, y)
// 2. Query maximum value in range [x, y)
//
// For this functionality we can copy the previous segment tree and
// modify it a bit to support both features

class SegTreeFallingBars {
public:
    SegTreeFallingBars(vector<ull>& arr)
    {
        n = static_cast<ull>(arr.size());
        tree = vector<ll>(2 * n, 0);
        max_tree = vector<ll>(2 * n, 0);

        // insert leaf nodes in tree
        for (ul i = 0; i < arr.size(); i++)
        {
            tree[n + i] = arr[i];
            max_tree[n + i] = arr[i];
        }

        // the "additions" kept in the parents are initialized to 0
        // so there is no further work to do
        for (ll i = n - 1; i > 0; i--)
        {
            max_tree[i] = max(max_tree[2 * i], max_tree[2 * i + 1]);
        }
    }

    ll queryMax(ul l, ul r)
    {
        ull res = 0;

        // loop to find the sum in the range
        for (l += n, r += n; l < r; l >>= 1, r >>= 1)
        {
            if (l & 1)
            {
                res = max(max_tree[l], res);
                l++;
            }

            if (r & 1)
            {
                r--;
                res = max(max_tree[r], res);
            }
        }

        return res;
    }
    
    void addToRange(ul l, ul r, ll addition)
    {
        // loop to find the sum in the range
        // ends after O(logn) iteration
        for (l += n, r += n; l < r; l >>= 1, r >>= 1)
        {
            if (l & 1)
            {
                tree[l] += addition;

                // update max
                max_tree[l] += addition;
                max_tree[l / 2] = max(max_tree[l], max_tree[l - 1]);

                l++;
            }

            if (r & 1)
            {
                r--;

                tree[r] += addition;

                // update max
                max_tree[r] += addition;
                max_tree[r / 2] = max(max_tree[r], max_tree[r + 1]);
            }
        }

        while (l > 0)
        {
            max_tree[l] = max(max_tree[2 * l], max_tree[2 * l + 1]);
            l >>= 1;
        }
    }

private:
    vector<ll> tree;
    vector<ll> max_tree;
    ul n;
};

class SegTreeFallingBars2 {
public:
    SegTreeFallingBars2(vector<ull>& arr)
    {
        n = static_cast<ull>(arr.size());
        tree = vector<ll>(2 * n, 0);
        lazy_updates = vector<ll>(2 * n, 0);

        constructionUtil(arr, 0, n - 1, 0);
    }

    ll queryMax(ul l, ul r)
    {
        return queryMaxUtil(0, n - 1, l, r, 0);
    }

    void addToRange(ul l, ul r, ll addition)
    {
        updateRangeUtil(0, 0, n - 1, l, r, addition);
    }

private:
    void constructionUtil(vector<ull>& arr, ul ss, ul se, ul si)
    {
        // out of range as ss can never be greater than se
        if (ss > se)
            return;

        // If there is one element in array, store it in
        // current node of segment tree and return
        if (ss == se)
        {
            tree[si] = arr[ss];
            return;
        }

        // If there are more than one elements, then recur
        // for left and right subtrees and store the sum
        // of values in this node
        ul mid = (ss + se) / 2;
        constructionUtil(arr, ss, mid, si * 2 + 1);
        constructionUtil(arr, mid + 1, se, si * 2 + 2);

        tree[si] = max(tree[si * 2 + 1], tree[si * 2 + 2]);
    }

    ll queryMaxUtil(ul s, ul e, ul l, ul r, ul index)
    {
        // If lazy flag is set for current node of segment tree,
        // then there are some pending updates. So we need to
        // make sure that the pending updates are done before
        // processing the sub sum query
        if (lazy_updates[index] != 0)
        {
            // Make pending updates to this node. Note that this
            // node represents maximum of elements in arr[s..e] and
            // all these elements must be increased by lazy_updates[index]
            tree[index] += lazy_updates[index];

            // checking if it is not leaf node because if
            // it is leaf node then we cannot go further
            if (s != e)
            {
                // Since we are not yet updating children os si,
                // we need to set lazy values for the children
                lazy_updates[index * 2 + 1] += lazy_updates[index];
                lazy_updates[index * 2 + 2] += lazy_updates[index];
            }

            // unset the lazy value for current node as it has
            // been updated
            lazy_updates[index] = 0;
        }

        // Out of range
        if (s > e || s > r || e < l)
            return 0;

        // At this point we are sure that pending lazy updates
        // are done for current node. So we can return value 
        // (same as it was for query in our previous post)

        // If this segment lies in range
        if (s >= l && e <= r)
            return tree[index];

        // If a part of this segment overlaps with the given
        // range
        ul mid = (s + e) / 2;
        return max(queryMaxUtil(s, mid, l, r, 2 * index + 1), queryMaxUtil(mid + 1, e, l, r, 2 * index + 2));
    }

    /*  si -> index of current node in segment tree
    ss and se -> Starting and ending indexes of elements for
                 which current nodes stores max.
    us and ue -> starting and ending indexes of update query
    diff -> which we need to add in the range us to ue */
    void updateRangeUtil(ul index, ul s, ul e, ul l,
        ul r, ll addition)
    {
        // If lazy value is non-zero for current node of segment
        // tree, then there are some pending updates. So we need
        // to make sure that the pending updates are done before
        // making new updates. Because this value may be used by
        // parent after recursive calls (See last line of this
        // function)
        if (lazy_updates[index] != 0)
        {
            // Make pending updates using value stored in lazy
            // nodes
            tree[index] += lazy_updates[index];

            // checking if it is not leaf node because if
            // it is leaf node then we cannot go further
            if (s != e)
            {
                // We can postpone updating children we don't
                // need their new values now.
                // Since we are not yet updating children of si,
                // we need to set lazy flags for the children
                lazy_updates[index * 2 + 1] += lazy_updates[index];
                lazy_updates[index * 2 + 2] += lazy_updates[index];
            }

            // Set the lazy value for current node as 0 as it
            // has been updated
            lazy_updates[index] = 0;
        }

        // out of range
        if (s > e || s > r || e < l)
            return;

        // Current segment is fully in range
        if (s >= l && e <= r)
        {
            // Add the difference to current node
            tree[index] += addition;

            // same logic for checking leaf node or not
            if (s != e)
            {
                // This is where we store values in lazy nodes,
                // rather than updating the segment tree itself
                // Since we don't need these updated values now
                // we postpone updates by storing values in lazy[]
                lazy_updates[index * 2 + 1] += addition;
                lazy_updates[index * 2 + 2] += addition;
            }
            return;
        }

        // If not completely in rang, but overlaps, recur for
        // children,
        ul mid = (s + e) / 2;
        updateRangeUtil(index * 2 + 1, s, mid, l, r, addition);
        updateRangeUtil(index * 2 + 2, mid + 1, e, l, r, addition);

        // And use the result of children calls to update this
        // node
        tree[index] = max(tree[index * 2 + 1], tree[index * 2 + 2]);
    }

    vector<ll> tree;
    vector<ll> lazy_updates;
    ul n;
};


class SegTreeFallingBars3 {
public:
    SegTreeFallingBars3(vector<ull>& arr)
    {
        n = (ul)arr.size();
        t = vector<ll>(4 * n, 0);
        lazy = vector<ll>(4 * n, 0);

        build(arr, 1, 0, n - 1);
    }

    void addToRange(ul l, ul r, ll addition)
    {
        update(1, 0, n - 1, l, r, addition);
    }

    ll queryMax(ul l, ul r)
    {
        return query(1, 0, n - 1, l, r);
    }

private:
    void build(vector<ull>& arr, int v, int tl, int tr) {
        if (tl == tr) {
            t[v] = arr[tl];
        }
        else {
            int tm = (tl + tr) / 2;
            build(arr, v * 2, tl, tm);
            build(arr, v * 2 + 1, tm + 1, tr);
            t[v] = max(t[v * 2], t[v * 2 + 1]);
        }
    }

    void push(int v) {
        t[v * 2] += lazy[v];
        lazy[v * 2] += lazy[v];
        t[v * 2 + 1] += lazy[v];
        lazy[v * 2 + 1] += lazy[v];
        lazy[v] = 0;
    }

    void update(ul v, ul tl, ul tr, ul l, ul r, ll addend) {
        if (l > r)
            return;
        if (l == tl && tr == r) {
            t[v] += addend;
            lazy[v] += addend;
        }
        else {
            push(v);
            int tm = (tl + tr) / 2;
            update(v * 2, tl, tm, l, min(r, tm), addend);
            update(v * 2 + 1, tm + 1, tr, max(l, tm + 1), r, addend);
            t[v] = max(t[v * 2], t[v * 2 + 1]);
        }
    }

    ll query(ul v, ul tl, ul tr, ul l, ul r) {
        if (l > r)
            return -0xffff;
        if (l == tl && tr == r)
            return t[v];
        push(v);
        ul tm = (tl + tr) / 2;
        return max(query(v * 2, tl, tm, l, min(r, tm)), query(v * 2 + 1, tm + 1, tr, max(l, tm + 1), r));
    }

    vector<ll> t;
    vector<ll> lazy;
    ul n;
};


void exFallingBars()
{
    ul H, W, N;

    cin >> H >> W >> N;

    vector<tuple<ul, ul, ul, ul>> mat(N, { 0, 0, 0, 0 });
    rep0(i, N)
    {
        ul R, C, L;

        cin >> R >> C >> L;
        
        mat[i] = { R, C, L, i };
    }

    std::sort(mat.begin(), mat.end());

    // the columns are one based, so the heights array needs to start with a dummy
    // cell 0 (which will never be used)
    vector<ull> heights(W + 1, 0);
    SegTreeFallingBars3 tree(heights);
    vector<ul> bar_heights(N, 0);

    repi0(i, N)
    {
        tuple<ul, ul, ul, ul> bar = mat[i];

        ul start = get<1>(bar);
        ul end = get<2>(bar) + start;

        // query [start, end)
        ll heighest_point_in_range = tree.queryMax(start, end - 1);

        ll bar_final_row = H - heighest_point_in_range;
        ul bar_original_index = get<3>(bar);
        bar_heights[bar_original_index] = bar_final_row;

        // add +1 to [start, end)
        tree.addToRange(start, end - 1, 1);
    }
    
    rep0(i, N)
    {
        cout << bar_heights[i] << endl;
    }
}

void testMaxSegmentTree()
{
    vector<ull> arr = { 1, 2, 3, 4, 5, 6, 7, 8 };
    SegTreeFallingBars3 tree(arr);

    tree.addToRange(1, 4, 3);
    ll res1 = tree.queryMax(0, 1);
    ll res2 = tree.queryMax(0, 7);
    ll res3 = tree.queryMax(4, 7);
    ll res4 = tree.queryMax(2, 6);
    return;
}

namespace test_cp_algorithms
{
    int t[1000] = { 0 };
    int lazy[1000] = { 0 };

    void build(int a[], int v, int tl, int tr) {
        if (tl == tr) {
            t[v] = a[tl];
        }
        else {
            int tm = (tl + tr) / 2;
            build(a, v * 2, tl, tm);
            build(a, v * 2 + 1, tm + 1, tr);
            t[v] = max(t[v * 2], t[v * 2 + 1]);
        }
    }

    void push(int v) {
        t[v * 2] += lazy[v];
        lazy[v * 2] += lazy[v];
        t[v * 2 + 1] += lazy[v];
        lazy[v * 2 + 1] += lazy[v];
        lazy[v] = 0;
    }

    void update(int v, int tl, int tr, int l, int r, int addend) {
        if (l > r)
            return;
        if (l == tl && tr == r) {
            t[v] += addend;
            lazy[v] += addend;
        }
        else {
            push(v);
            int tm = (tl + tr) / 2;
            update(v * 2, tl, tm, l, min(r, tm), addend);
            update(v * 2 + 1, tm + 1, tr, max(l, tm + 1), r, addend);
            t[v] = max(t[v * 2], t[v * 2 + 1]);
        }
    }

    int query(int v, int tl, int tr, int l, int r) {
        if (l > r)
            return -0xffff;
        if (l == tl && tr == r)
            return t[v];
        push(v);
        int tm = (tl + tr) / 2;
        return max(query(v * 2, tl, tm, l, min(r, tm)),
            query(v * 2 + 1, tm + 1, tr, max(l, tm + 1), r));
    }

    void test()
    {
        int a[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };

        build(a, 1, 0, 8);

        // update items [2-5], biggest item will now become 106
        update(1, 0, 8, 2, 5, 100);

        // query [4,6], should return 106
        int res1 = query(1, 0, 8, 4, 6);

        update(1, 0, 8, 1, 8, -200);

        int res2 = query(1, 0, 8, 0, 8);
        return;
    }
}

namespace trie_tree
{
    class TrieTree
    {

    int max_len;
    vector<unordered_map<char, int>> t; //t[0]['a']=1  means that e(0,1)='a'.
    vector<ll> cnt;
    int szT;
    ll sum;

    int optimalBundlingInternal(int current, int group_size, int depth, ll& sum)
    {
        int up = cnt[current];

        for (const pair<char, int>& p : t[current])
        {
            up += optimalBundlingInternal(p.second, group_size, depth + 1, sum);
        }

        int teams_number = up / group_size;
        sum += (depth * teams_number);

        up -= (teams_number * group_size);

        return up;
    }

public:
    TrieTree(int max_len) :max_len(max_len) {
        szT = 0;
        t = vector<unordered_map<char, int>>(max_len);
        cnt = vector<ll>(max_len);
        sum = 0;
    }

    void add(const string& s) {
        int len = s.size();
        int v = 0;
        for (int i = 0; i < len; i++) {
            int c = s[i];
            // c key in map t[v] is exist or created here.
            if (t[v][c] == 0)
                t[v][c] = ++szT;

            v = t[v][c];
        }
        ++cnt[v];
    }

    int find(const string& s) {
        int len = s.size();
        int v = 0;
        for (int i = 0; i < len; i++) {
            int c = s[i];
            if (t[v][c] == 0) return 0;
            v = t[v][c];
        }
        return cnt[v];
    }

    // returns an integer describing the sum of shared prefixes of each group when we divide
    // the strings in the trie tree into groups of size K
    int optimalBundling(int group_size)
    {
        ll sum = 0;
        optimalBundlingInternal(0, group_size, 0, sum);

        return sum;
    }
};
}

namespace test5
{

void exMancala2() {
    ul N, M;
    cin >> N >> M;

    vector<ull> A(N, 0);
    rep0(i, N)
    {
        cin >> A[i];
    }

    SegTreeRangeUpdates tree(A);

    vector<ul> B(M + 1, 0);
    rep(i, M)
    {
        cin >> B[i];
    }

    rep(i, M)
    {
        ul box_num = B[i];

        // take all the balls out of box_num
        ll balls = tree.query(box_num);
        tree.clear(box_num);

        // first, add the amount of balls distributed to all boxes
        ll addition = balls / N;
        tree.addToRange(0, N, addition);

        // now add 1 to the box that get one extra ball each
        ul remainder = balls % N;

        if (box_num == N - 1)
        {
            // handle edge case
            tree.addToRange(0, remainder, 1);
            continue;
        }

        ul range_start = box_num + 1;
        ul range_end = min(box_num + 1 + remainder, N);
        tree.addToRange(range_start, range_end, 1);

        if (box_num + 1 + remainder > N)
        {
            range_start = 0;
            range_end = box_num + 1 + remainder - N;
            tree.addToRange(range_start, range_end, 1);
        }
    }

    rep0(i, N)
    {
        cout << tree.query(i) << " ";
    }
}

string exDconvertToNewString(const string& s) {
    string newString = "@";

    for (int i = 0; i < s.size(); i++) {
        newString += "#" + s.substr(i, 1);
    }

    newString += "#$";
    return newString;
}

// given a string s, find the longest string t such that t=a+b, where a is 
// a prefix of s and b is a suffix of s, and t is a palindrome.
//
// The algorithm is as such:
// run Manacher's Algorithm on the string, we now have the biggest palindrome centered on i for every
// 1<=i<=size(Q)
//
// Next, find how many matching characters we have from the start and end of the string.
// Finally, search for the biggest palindrome that intersects the start/end sequence of equal
// characters.
//
// The concatenation of s[:equalSides] + middlePalindrome + s[-equalSides:] will yield the biggest
// palindrome matching the question.
string exDLongestSplitPalindrome(const string& s) {
    string Q = exDconvertToNewString(s);
    vector<int> P(Q.size(), 0);
    int c = 0, r = 0;   // current center, right limit

    for (int i = 1; i < Q.size() - 1; i++) {
        // find the corresponding letter in the palidrome subString
        int iMirror = c - (i - c);

        if (r > i) {
            P[i] = min(r - i, P[iMirror]);
        }

        // expanding around center i
        while (Q[i + 1 + P[i]] == Q[i - 1 - P[i]]) {
            P[i]++;
        }

        // Update c,r in case if the palindrome centered at i expands past r,
        if (i + P[i] > r) {
            c = i;              // next center = i
            r = i + P[i];
        }
    }

    ul equalEnds = 0;
    for (int i = 1; i < (Q.size() - 1) / 2; i++)
    {
        if (Q[i] != Q[Q.size() - 1 - i])
            break;

        equalEnds++;
    }

    int biggestCompletingPalindrome = 0;
    int palindromeCenter = 0;
    // find the biggest palindrome that ends at N-equalEnds-2
    for (int i = 1; i < Q.size() - 1; i++) {

        if ((i + P[i] >= Q.size() - 1 - equalEnds && i < Q.size() - equalEnds - 1)
            || (i - P[i] <= equalEnds && i > equalEnds))
        {
            int intersection;
            // if palindrome is straight after prefix
            if (i - P[i] <= equalEnds)
            {
                intersection = equalEnds - i + P[i];
            }
            else
            {
                intersection = equalEnds - (Q.size() - 1 - i - P[i]);
            }

            int currentPalindromeSize = P[i] - intersection;
            if (currentPalindromeSize > biggestCompletingPalindrome)
            {
                palindromeCenter = i;
                biggestCompletingPalindrome = currentPalindromeSize;
            }
        }
    }

    string res = s.substr(0, equalEnds / 2);

    res += s.substr((palindromeCenter - 1 - biggestCompletingPalindrome) / 2,
        biggestCompletingPalindrome);

    res += s.substr(s.size() - equalEnds / 2, equalEnds / 2);

    return res;
}

void exD()
{
    ul t;
    cin >> t;

    rep(i, t)
    {
        string s;
        cin >> s;

        string res = exDLongestSplitPalindrome(s);
        cout << res << endl;
    }
}

}
//
//void testLongestPalindromicSubsequence()
//{
//    string s = "abaccdcckkokokkw";
//    string res = longestPalindromeSubstring(s);
//    return;
//}


int main()
{
    test5::exD();
    return 0;
}
