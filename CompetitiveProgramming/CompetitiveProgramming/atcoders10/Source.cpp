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
#define rep(i, n) for(ull i = 1; i <= n; i++)
#define rep0(i, n) for(ull i = 0; i < n; i++)
#define repi(i, n) for(ull i = n; i > 0; i--)
#define repi0(i, n) for(ll i = n - 1; i >= 0; i--)

#define max(x, y) x > y ? x : y
#define min(x, y) x > y ? y : x

bool ex1Check(vector<ul>&arr)
{
	rep(i, 5)
	{
		if (arr[i] != i)
			return false;
	}

	return true;
}

void ex11()
{
	ul a1, a2, a3, a4, a5;

	vector<ul> A(6, 0);
	rep(i, 5)
	{
		cin >> A[i];
	}

	vector<ul> tmp_A(6, 0);

	bool success = false;
	rep(i, 4)
	{
		tmp_A = vector<ul>(A.begin(), A.end());

		// swap i and i+1
		ul tmp = tmp_A[i];
		tmp_A[i] = tmp_A[i + 1];
		tmp_A[i + 1] = tmp;

		if (ex1Check(tmp_A))
		{
			success = true;
			break;
		}
	}

	if (success)
	{
		cout << "Yes" << endl;
	}
	else
	{
		cout << "No" << endl;
	}
}

namespace std {
	namespace
	{

		// Code from boost
		// Reciprocal of the golden ratio helps spread entropy
		//     and handles duplicates.
		// See Mike Seymour in magic-numbers-in-boosthash-combine:
		//     http://stackoverflow.com/questions/4948780

		template <class T>
		inline void hash_combine(std::size_t& seed, T const& v)
		{
			seed ^= std::hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}

		// Recursive template code derived from Matthieu M.
		template <class Tuple, size_t Index = std::tuple_size<Tuple>::value - 1>
		struct HashValueImpl
		{
			static void apply(size_t& seed, Tuple const& tuple)
			{
				HashValueImpl<Tuple, Index - 1>::apply(seed, tuple);
				hash_combine(seed, std::get<Index>(tuple));
			}
		};

		template <class Tuple>
		struct HashValueImpl<Tuple, 0>
		{
			static void apply(size_t& seed, Tuple const& tuple)
			{
				hash_combine(seed, std::get<0>(tuple));
			}
		};
	}

	template <typename ... TT>
	struct hash<std::tuple<TT...>>
	{
		size_t
			operator()(std::tuple<TT...> const& tt) const
		{
			size_t seed = 0;
			HashValueImpl<std::tuple<TT...> >::apply(seed, tt);
			return seed;
		}

	};
}

ull ex4XorArray(vector<ull>& arr)
{
	ull x = 0;
	rep0(i, arr.size())
	{
		x ^= arr[i];
	}

	return x;
}

ull ex4Choose(vector<ull> A, unordered_map<ull, vector<vector<ull>>>& xors)
{
	ul N = (ul)A.size();
	if (N == 0)
		return 0;

	ull counter = 0;

	ull x = ex4XorArray(A);
	if (xors.find(x) == xors.end())
	{
		xors[x] = { A };

		counter++;
	}
	else
	{
		vector<vector<ull>>& permutations_seen = xors[x];	
		rep0(i, permutations_seen.size())
		{
			if (A.size() != permutations_seen[i].size())
				continue;

			bool identical = true;
			rep0(j, A.size())
			{
				if (A[j] != permutations_seen[i][j])
				{
					identical = false;
					break;
				}
			}

			// prevent computing the same thing over and over again
			if (identical)
				return 0;
		}

		permutations_seen.push_back(A);
	}

	// choose any 2 elements and combine them
	rep0(i, N)
	{
		for (ul j = i + 1; j < N; j++)
		{
			// swap A[i] and A[j]
			vector<ull> new_A = vector<ull>(N - 1, 0);
			for (ul k = 0; k < i; k++)
			{
				new_A[k] = A[k];
			}

			new_A[i] = A[i] + A[j];
			for (ul k = i + 1; k < j; k++)
			{
				new_A[k] = A[k];
			}

			for (ul k = j + 1; k < N; k++)
			{
				new_A[k - 1] = A[k];
			}

			counter += ex4Choose(std::move(new_A), xors);
		}
	}

	return counter;
}

void ex4()
{
	ul N;
	cin >> N;

	vector<ull> A(N, 0);
	rep0(i, N)
	{
		cin >> A[i];
	}

	unordered_set<tuple<ull, ull, ull, ull, ull, ull, ull, ull, ull, ull, ull, ull>> permutations;
	unordered_map<ull, vector<vector<ull>>> xors;

	ull res = ex4Choose(A, xors);

	cout << res << endl;
}

ull ex5MinVitamin(vector<ull>& vitamins)
{
	ull min_vitamin = vitamins[0];
	rep(i, vitamins.size() - 1)
	{
		min_vitamin = min(min_vitamin, vitamins[i]);
	}

	return min_vitamin;
}

void knapsackProblem(const vector<tuple<ul, ul, ul>>& items, ul budget,  ull& smallest_vitamin)
{
	ul n = items.size();
	// vitamins[i][j][k] is the amount of k vitamins while only considering the first i-1 items, and having
	// a mxaimal budget of j
	vector<vector<vector<ull>>> vitamins(n + 1, vector<vector<ull>>(budget + 1, vector<ull>(3, 0)));

	// Fill the DP table
	for (ul i = 1; i <= n; ++i) {
		for (ull b = 0; b <= budget; ++b) {
			ul vitamin_type = get<0>(items[i - 1]);
			ul vitamin_amount = get<1>(items[i - 1]);
			ul calories = get<2>(items[i - 1]);
			if (calories <= b) {
				// standard Knapsack step - take the best prestige out of the two option:
				// option A - we don't choose the i-th item
				// option B - we choose the i-th item
				auto option_a = vitamins[i - 1][b];
				auto option_b = vitamins[i - 1][b - calories];
				option_b[vitamin_type - 1] += vitamin_amount;

				if (ex5MinVitamin(option_a) > ex5MinVitamin(option_b))
				{
					vitamins[i][b] = option_a;
				}
				else
				{
					vitamins[i][b] = option_b;
				}
			}
			else {
				vitamins[i][b] = vitamins[i - 1][b];
			}
		}
	}

	smallest_vitamin = ex5MinVitamin(vitamins[n][budget]);
	return;
}

void ex5()
{
	ul N, X;
	cin >> N >> X;

	vector<tuple<ul, ul, ul>> mat = vector<tuple<ul, ul, ul>>(N, { 0, 0, 0 });
	rep0(i, N)
	{
		ul V, A, C;
		cin >> V >> A >> C;

		mat[i] = { V, A, C };
	}

	ull smallest_vitamin;
	knapsackProblem(mat, X, smallest_vitamin);

	cout << smallest_vitamin << endl;
}

int main()
{
	ex5();
	return 0;
}