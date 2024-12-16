#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unordered_set>
#include <deque>
#include <algorithm>
#include <limits.h>

using namespace std;
#define ull unsigned long long
#define ul unsigned long
#define ll long long

#define max(x, y) x > y ? x : y
#define min(x, y) x > y ? y : x

bool task4BinarySearch(vector<ull>& arr, ull num)
{
	ul low = 0;
	ul high = static_cast<ul>(arr.size());
	ul mid;

	while (low < high)
	{
		mid = (low + high) / 2;
		if (arr[mid] == num)
			return true;

		if (arr[mid] < num)
		{
			low = mid + 1;
		}
		else
		{
			high = mid;
		}
	}

	return false;
}

bool task4Check(unordered_set<ull>& sum_up_to, ull sum, ull S)
{
	for (ull start : sum_up_to)
	{
		// S % A = A_j - A_i
		// sum_up_to[i] is A_i
		// S % A is target
		// 
		// we need to look for A_j
		//
		// A - A_i + A_j [a1, a2, a_i, ... a_j, ...]
		// if such A_j exists we can solve

		// single A used
		// S = A_j - A_i -> S + A_i = A_j
		if (sum_up_to.find(S + start) != sum_up_to.end())
			return true;

		// 2+ As used
		// S % A = A - A_i + A_j ->
		// S % A + A_i - A = A_j
		if (S % sum + start < sum)
			continue;

		if (sum_up_to.find(S % sum + start - sum) != sum_up_to.end())
			return true;
	}

	return false;
}

void task4()
{
	ul N;
	cin >> N;
	ull S;
	cin >> S;

	vector<ull> A;
	unordered_set<ull> sum_up_to;
	ull sum = 0;
	for (ul i = 0; i < N; i++)
	{
		sum_up_to.insert(sum);

		ull num;
		cin >> num;
		A.push_back(num);

		sum += num;
	}
	sum_up_to.insert(sum);

	//A = [1, 3, 5, 2, 1]
	// A A A A A A .....
	// S = A - A_i + m*A + A_j = A_j - A_i + m'*A
	// S % A = A_j - A_i ->
	// S % A + A_i = A_j


	// S = 7
	// S = A - A_i + A_j-> S + A_i - A = A_j
	// [1,2,5] [1,2,5]
	// A_1 =1, A_1 + S = 8

	ull target = (S % sum);
	if (task4Check(sum_up_to, S, sum))
	{
		cout << "Yes" << endl;
	}
	else
	{
		cout << "No" << endl;
	}
}

int main()
{
	task4();
	return 0;
}