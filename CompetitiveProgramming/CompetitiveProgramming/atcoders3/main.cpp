#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unordered_set>
#include <queue>
#include <algorithm>

#define ull unsigned long long
#define ul unsigned long
#define ll long long
#define max(x, y) x > y ? x : y

void ex1() {
	ll size = 0;
	std::cin >> size;

	ll p = 0;
	std::cin >> p;

	std::string s;
	std::cin >> s;

	ll count = std::count_if(s.begin(), s.end(), [](char c) {return c == '.'; });
	std::cout << count + p << std::endl;
}

void ex2() {
	ll N, D;
	std::cin >> N >> D;
	std::string S;
	std::cin >> S;
	std::string result(S);

	ll i = N - 1;
	ll found = 0;
	while (i >= 0 && found < D) {
		if (S[i] == '@') {
			found++;
			result[i] = '.';
		}
		i--;
	}
	std::cout << result << std::endl;
}

long long ex3BinarySearch(const std::vector<std::pair<ul, ul>>& descending_arr, ul value)
{
	ul start = 0;
	ul end = descending_arr.size();
	ul mid = -1;
	while (start < end)
	{
		mid = (start + end) / 2;

		if (descending_arr[mid].first == value)
			break;

		if (descending_arr[mid].first < value)
		{
			end = mid;
		}
		else
		{
			start = mid + 1;
		}
	}

	// we did a normal binary search, but we don't necessarily have this exact value
	// in our descending array.
	// it's enough to find the first element whose value is smaller.
	if (mid > 0 && descending_arr[mid - 1].first <= value)
	{
		return descending_arr[mid - 1].second;
	}
	if (descending_arr[mid].first <= value)
		return descending_arr[mid].second;

	if (mid < descending_arr.size() - 1 && descending_arr[mid + 1].first <= value)
		return descending_arr[mid + 1].second;

	return -1;
}

long long ex3BinarySearchOneBase(const std::vector<std::pair<ul, ul>>& descending_arr, ul value)
{
	long long result = ex3BinarySearch(descending_arr, value);
	if (result == -1)
		return -1;

	return result + 1;
}

void ex3()
{
	ul n, m;
	std::cin >> n;
	std::cin >> m;

	std::vector<ul> gourmet_levels;
	std::vector<ul> delicousness_levels;

	for (ul i = 0; i < n; i++)
	{
		ul level;
		std::cin >> level;

		gourmet_levels.push_back(level);
	}

	for (ul i = 0; i < m; i++)
	{
		ul delicousness;
		std::cin >> delicousness;

		delicousness_levels.push_back(delicousness);
	}

	std::vector<std::pair<ul, ul>> relevant_groumet_levels;
	for (ul i = 0; i < gourmet_levels.size(); i++)
	{
		if (i == 0)
		{
			relevant_groumet_levels.push_back({ gourmet_levels[0], 0 });
			continue;
		}

		// this person will never eat anything
		if (gourmet_levels[i] >= relevant_groumet_levels.back().first)
			continue;

		relevant_groumet_levels.push_back({ gourmet_levels[i], i });
	}

	// relevant_groumet_levels is now a descending list of numbers
	// iterate the sushi pieces and use a binary search to determine who eats
	// each one
	for (ul i = 0; i < delicousness_levels.size(); i++)
	{
		std::cout << ex3BinarySearchOneBase(relevant_groumet_levels, delicousness_levels[i]) << std::endl;
	}
}

void ex4Inner2(ll i, std::vector<ll>& v, ll r, ll& su) {
	if (i >= (ll)v.size()) {
		su++;
		return;
	}

	for (ll j = v[i - 1]; j <= r; j++) {
		v[i] = j;
		ex4Inner2(i + 1, v, r, su);
	}
}

void ex4Inner(ll i, std::vector<ll>& arr, ll r) {
	if (i >= (ll)arr.size()) {
		for (ll a = 0; a < (ll)arr.size(); a++) {
			std::cout << 10 * a + 1 + arr[a] << " ";
		}
		std::cout << std::endl;
		return;
	}

	for (ll j = arr[i - 1]; j <= r; j++) {
		arr[i] = j;
		ex4Inner(i + 1, arr, r);
	}
	return;
}

void ex4() {
	ll n = 0;
	std::cin >> n;

	ll m = 0;
	std::cin >> m;

	ll r = m - 10 * (n - 1) - 1;

	ll su = 0;
	std::vector<ll> v1(n, 0);

	for (ll i = 0; i <= r; i++) {
		v1[0] = i;
		ex4Inner2(1, v1, r, su);
	}
	std::cout << su << std::endl;

	std::vector<ll> v(n, 0);

	for (ll i = 0; i <= r; i++) {
		v[0] = i;
		ex4Inner(1, v, r);
	}
}

struct Ex6Segment {
	ul start;
	ul end;
	ul height;
	Ex6Segment* next;
	//Ex6Segment* prev;
};

void ex6()
{
	ul H, W, N;
	std::cin >> H;
	std::cin >> W;
	std::cin >> N;

	std::vector<std::tuple<ul, ul, ul>> bars;
	for (ul i = 0; i < N; i++)
	{
		ul R, C, L;
		std::cin >> R;
		std::cin >> C;
		std::cin >> L;

		bars.push_back({ R, C, L });
	}

	for (ul i = 0; i < W; i++)
	{

	}
	//std::vector
}

int main()
{
	ex4();
	return 0;
}