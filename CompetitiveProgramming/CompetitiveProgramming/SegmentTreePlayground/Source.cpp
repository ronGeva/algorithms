#include <vector>
#include <map>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <queue>
#include <map>
#define all(x) (x).begin(), (x).end()

using namespace std;
using ll = long long;

#define INF  1e9
#define MOD 998244353
ll gcd(ll a, ll b) {
	// Handle invalid inputs
	if (a == 0 && b == 0) {
		throw invalid_argument("GCD is undefined for both a and b being zero");
	}

	// Handle negative numbers, since GCD is always non-negative
	a = abs(a);
	b = abs(b);

	if (a == 0) return b;
	if (b == 0) return a;

	// Euclidean algorithm
	return gcd(b % a, a);
}

// Credits to HealthyUG for the inspiration.
// Segment Tree with Point Updates and Range Queries
// Supports multiple Segment Trees with just a change in the Node and Update
// Very few changes required everytime

template<typename Node, typename Update>
struct SegTree {
	vector<Node> tree;
	vector<ll> arr; // type may change
	int n;
	int s;
	SegTree(int a_len, vector<ll>& a) { // change if type updated
		arr = a;
		n = a_len;
		s = 1;
		while (s < 2 * n) {
			s = s << 1;
		}
		tree.resize(s); fill(all(tree), Node());
		build(0, n - 1, 1);
	}
	void build(int start, int end, int index)  // Never change this
	{
		if (start == end) {
			tree[index] = Node(arr[start]);
			return;
		}
		int mid = (start + end) / 2;
		build(start, mid, 2 * index);
		build(mid + 1, end, 2 * index + 1);
		tree[index].merge(tree[2 * index], tree[2 * index + 1]);
	}
	void update(int start, int end, int index, int query_index, Update& u)  // Never Change this
	{
		if (start == end) {
			u.apply(tree[index]);
			return;
		}
		int mid = (start + end) / 2;
		if (mid >= query_index)
			update(start, mid, 2 * index, query_index, u);
		else
			update(mid + 1, end, 2 * index + 1, query_index, u);
		tree[index].merge(tree[2 * index], tree[2 * index + 1]);
	}
	Node query(int start, int end, int index, int left, int right) { // Never change this
		if (start > right || end < left)
			return Node();
		if (start >= left && end <= right)
			return tree[index];
		int mid = (start + end) / 2;
		Node l, r, ans;
		l = query(start, mid, 2 * index, left, right);
		r = query(mid + 1, end, 2 * index + 1, left, right);
		ans.merge(l, r);
		return ans;
	}
	void make_update(int index, ll val) {  // pass in as many parameters as required
		Update new_update = Update(val); // may change
		update(0, n - 1, 1, index, new_update);
	}
	Node make_query(int left, int right) {
		return query(0, n - 1, 1, left, right);
	}
};

struct Node1 {
	ll val; // may change
	Node1() { // Identity element
		val = 0;	// may change
	}
	Node1(ll p1) {  // Actual Node
		val = p1; // may change
	}
	void merge(Node1& l, Node1& r) { // Merge two child nodes
		val = gcd(l.val, r.val);  // may change
	}
};

struct Update1 {
	ll val; // may change
	Update1(ll p1) { // Actual Update
		val = p1; // may change
	}
	void apply(Node1& a) { // apply update to given node
		a.val = val; // may change
	}
};

int main() {
	// Sample array to test the segment tree
	vector<ll> arr = { 1, 1, 6, 18, 36 };
	int n = arr.size();

	// Create the Segment Tree with Node1 and Update1
	SegTree<Node1, Update1> segTree(n, arr);

	// Query the GCD (in this case XOR) of the range [2, 4] (i.e., [3, 7, 2, 8])
	cout << "Query result of range [2, 4]: " << segTree.make_query(2, 4).val << endl;  // Should output the XOR of 3, 7, 2, 8

	// Update the value at index 2 (change it to 10)
	segTree.make_update(2, 18);

	// Query again the XOR of the range [1, 4] after the update
	cout << "Query result of range [1, 4] after update: " << segTree.make_query(2, 4).val << endl;  // Should output the XOR of 3, 10, 2, 8

	// Query the XOR of the entire array
	cout << "Query result of range [0, 5] (entire array): " << segTree.make_query(0, 4).val << endl;  // Should output the XOR of all elements

	return 0;
}