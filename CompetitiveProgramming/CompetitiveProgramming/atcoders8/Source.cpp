#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <deque>
#include <algorithm>
#include <limits.h>

using namespace std;
#define ull unsigned long long
#define ul unsigned long
#define ll long long
#define UL_MAX 0xffffffff
#define ULL_MAX 0xffffffffffffffff

#define max(x, y) x > y ? x : y
#define min(x, y) x > y ? y : x

void ex1()
{
	string str;
	cin >> str;

	cout << str[0] << "UPC" << endl;
}

void ex4()
{
	ul N;
	cin >> N;

	vector<ul> A(N + 1, 0);
	for (ul i = 1; i <= N; i++)
	{
		cin >> A[i];
	}

	vector<ul> result(N + 1, 0);

	vector<ul> broke_on_round(N+1, 0);
	ul non_broke_adults = 0;

	for (ul i = 1; i <= N; i++)
	{
		ul stones_now = A[i] + non_broke_adults;
		if (stones_now < N - i)
		{
			result[i] = 0;
			broke_on_round[i + stones_now] += 1;
		}
		else
		{
			result[i] = A[i] + non_broke_adults - (N - i);
		}
		
		non_broke_adults++;

		non_broke_adults -= broke_on_round[i];
	}

	for (ul i = 1; i <= N; i++)
	{
		cout << result[i] << " ";
	}
}



void ex5()
{
	ul N;
	cin >> N;

	vector<ull> values(N, 0);
	for (ul i = 0; i < N; i++)
	{
		cin >> values[i];
	}

	std::sort(values.begin(), values.end());

	// 0 to N/2-1 is the lower half
	// N/2 to N-1 is the upper half

	// try to find a match for values in the lower half, starting
	// with the most problematic ones first (the big ones)
	ul upper_index = N - 1;
	ul matches = 0;
	for (ll i = N/2-1; i >= 0; i--)
	{
		if (values[i] <= values[upper_index] / 2)
		{
			matches++;
			upper_index--;
		}
	}

	cout << matches << endl;
}

void ex6()
{
	ull N, M, A, B;

	cin >> N >> M >> A >> B;

	vector<pair<ull, ull>> ranges(M, { 0, 0 });
	for (ull i = 0; i < M; i++)
	{
		ull L, R;
		cin >> L >> R;
		ranges.push_back({ L, R });
	}

	ul next_bad_range = 0;
	unordered_map<ull, bool> is_reachable;

	ull current_square = 1;
	for (ul i = 0; i < M; i++)
	{
		// don't go back
		ull next_start = max(static_cast<ll>(ranges[i].first) - 2 * B, current_square);

		ull diff = next_start - current_square;

		ull delta = diff % B;

		current_square = next_start + delta;

	}
}

int main()
{
	ex5();
	return 0;
}