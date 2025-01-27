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

// returns all matching indexes in the original list
vector<ul> addZeroesBinarySearch(vector<tuple<ull, ul>>&B, ull size)
{
	ul start = 0;
	ul end = B.size();
	ul mid;

	while (start < end)
	{
		mid = (start + end) / 2;

		ull value = get<0>(B[mid]);
		if (value < size + 1)
		{
			start = mid + 1;
			continue;
		}
		if (value > size + 1)
		{
			end = mid;
			continue;
		}

		vector<ul> result;
		result.push_back(get<1>(B[mid]) - 1);
		ul tmp = mid;
		while (tmp + 1 < end && get<0>(B[tmp + 1]) == size + 1)
		{
			tmp++;
			result.push_back(get<1>(B[tmp]) - 1);
		}
		tmp = mid;
		while (tmp - 1 > 0 && get<0>(B[tmp - 1]) == size + 1)
		{
			tmp--;
			result.push_back(get<1>(B[tmp]) - 1);
		}

		return result;
	}

	return {};
}

ull addZeroesDfs(vector<tuple<ull, ul>>&B, ull size, unordered_set<ul>& visited)
{
	// currently |a| = size
	if (visited.find(size) != visited.end())
		return 0;

	visited.insert(size);

	ull biggest_size = size;
	vector<ul> possible_zeroes_added = addZeroesBinarySearch(B, size);
	for (ul zeroes : possible_zeroes_added)
	{
		ull possible_size = addZeroesDfs(B, size + zeroes, visited);
		biggest_size = max(biggest_size, possible_size);
	}

	return biggest_size;
}

int main()
{
	ul t;
	cin >> t;
	rep(unused, t)
	{
		ul n;
		cin >> n;

		vector<ull> arr(n + 1, 0);
		rep(i, n)
		{
			cin >> arr[i];
		}

		vector<tuple<ull, ul>> B(n + 1, { 0, 0 });
		rep(i, n)
		{
			B[i] = { arr[i] + i, i };
		}

		sort(B.begin(), B.end());

		unordered_set<ul> visited;
		ull size = addZeroesDfs(B, n, visited);

		cout << size << endl;
	}

	return 0;
}