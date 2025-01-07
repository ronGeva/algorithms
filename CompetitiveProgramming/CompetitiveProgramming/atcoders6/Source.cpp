#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <deque>
#include <algorithm>
#include <limits.h>

using namespace std;
#define ull unsigned long long
#define ul unsigned long
#define ll long long

#define max(x, y) x > y ? x : y
#define min(x, y) x > y ? y : x

bool ex1Check()
{
	unordered_map<ul, ul> occurrences;
	for (ul i = 0; i < 4; i++)
	{
		ul num;
		cin >> num;

		if (occurrences.find(num) == occurrences.end())
		{
			occurrences[num] = 0;
		}

		occurrences[num]++;
	}
	
	for (ul first = 1; first <= 13; first++)
	{
		for (ul second = 1; second <= 13; second++)
		{
			if (first == second)
				continue;

			if (occurrences.find(first) == occurrences.end() ||
				occurrences.find(second) == occurrences.end())
			{
				continue;
			}

			ul first_occurences = occurrences[first];
			ul second_occurences = occurrences[second];

			if (first_occurences == 3 && second_occurences == 1)
				return true;

			if (first_occurences == 2 && second_occurences == 2)
				return true;
		}
	}
	return false;
}

void ex1()
{
	if (ex1Check())
	{
		cout << "Yes" << endl;
	}
	else
	{
		cout << "No" << endl;
	}
}

void ex2()
{
	string S;
	cin >> S;

	ul required = 0;

	for (ul i = 0; i < S.size();)
	{
		if (i < S.size() - 1 && S[i] == '0' && S[i + 1] == '0')
		{
			i += 2;
		}
		else
		{
			i++;
		}
		required++;
	}

	cout << required << endl;
}

ull ex3PairKey(ul s_start, ul t_start)
{
	return (((ull)s_start << 32) | (ull)t_start);
}

// forward declaration
ull ex3LevDistance(const string& S, ul s_start, const string& T, ul t_start,
	unordered_map<ull, ull>& memo, ull K);

// result is <distance, encountered limit>
pair<ull, bool> ex3LevDistanceNoCache(const string& S, ul s_start, const string& T, ul t_start,
	unordered_map<ull, ull>& memo, ull K)
{
	if (s_start == S.size())
		return { T.size() - t_start, false };

	if (t_start == T.size())
		return { S.size() - s_start, false };

	if (S[s_start] == T[t_start])
		return { ex3LevDistance(S, s_start + 1, T, t_start + 1, memo, K), false };

	// we can't make any more changes, return ULONG_MAX
	if (K == 0)
		return { 0xffffffff, true };

	ull option1 = ex3LevDistance(S, s_start + 1, T, t_start, memo, K - 1);
	ull option2 = ex3LevDistance(S, s_start, T, t_start + 1, memo, K - 1);
	ull option3 = ex3LevDistance(S, s_start + 1, T, t_start + 1, memo, K - 1);

	ull min_option = min(option1, option2);
	min_option = min(min_option, option3);

	min_option++;

	bool encountered_limit = false;
	if (option1 >= 0xffffffff || option2 >= 0xffffffff || option3 >= 0xffffffff)
	{
		encountered_limit = true;
	}

	return { min_option, encountered_limit };
}

ull ex3LevDistance(const string& S, ul s_start, const string& T, ul t_start,
				   unordered_map<ull, ull>& memo, ull K)
{
	ull key = ex3PairKey(s_start, t_start);
	if (memo.find(key) != memo.end())
		return memo[key];

	pair<ull, bool> min_option = ex3LevDistanceNoCache(S, s_start, T, t_start, memo, K);

	// only cache valid options
	if (!min_option.second && min_option.first < 0xffffffff)
	{
		memo[key] = min_option.first;
	}

	return min_option.first;
}

void ex3()
{
	ull K;
	string S, T;

	cin >> K;
	cin >> S >> T;

	unordered_map<ull, ull> memo;

	for (int i = min(S.size(), T.size()); i >= 0; i--)
	{
		ex3LevDistance(S, S.size() - i, T, T.size() - i, memo, K);
	}

	ull lev_distance = ex3LevDistance(S, 0, T, 0, memo, K);

	if (lev_distance <= K)
	{
		cout << "Yes\n";
	}
	else
	{
		cout << "No\n";
	}
}

int main()
{
	ex3();
	return 0;
}