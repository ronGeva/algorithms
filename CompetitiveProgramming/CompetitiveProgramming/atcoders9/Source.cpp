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

	char a = str[0];
	char b = str[2];
	
	ul num = (a - '0') * (b - '0');
	cout << num << endl;
}

bool ex4Check(ul distance_squard, ul row, ul column)
{
	double d = ((double)row + 0.5) * ((double)row + 0.5) +
		((double)column + 0.5) * ((double)column + 0.5);

	return d <= distance_squard;
}

ul ex4BinarySearch(ul R, ul row)
{
	ul start = 0;
	ul end = R;
	ull squared_distance = R * R;
	ul mid;

	while (start < end)
	{
		mid = (start + end) / 2;

		if (ex4Check(squared_distance, row, mid))
		{
			// column still in range, go right
			start = mid + 1;
		}
		else
		{
			end = mid;
		}
	}

	if (ex4Check(squared_distance, row, mid + 1))
		return mid + 1;

	if (ex4Check(squared_distance, row, mid))
		return mid;

	return mid - 1;
}

void ex4()
{
	ul R;
	cin >> R;

	ul num = 0;
	for (ul row = 1; row <= R - 1; row++)
	{
		// we're looking at the i-th row in the circle above the center
		// specifically on the right side of it - in the positives (+, +) (right upper side)
		//
		// do a binary search to see how many cells fit into the circle here
		ul biggest_column_that_fits = ex4BinarySearch(R, row);

		num += biggest_column_that_fits;
	}

	// the number we've calculated is symetric in all 4 sides
	num *= 4;

	// now add the middle column and middle row, all of which fit
	num += (R - 1) * 4 + 1;

	cout << num << endl;
}

void ex5()
{
	ull N, M;

	cin >> N >> M;
	vector<ull> prices(N + 1, 0);

	for (ul i = 1; i <= N; i++)
	{
		cin >> prices[i];
	}

	std::sort(prices.begin(), prices.end());

	vector<double> amount_ratios(N - 1, 0);
	for (ll i = 1; i < N; i++)
	{
		// compare ki^2 * P_i to k_(i+1)^2 * P_(i+1), we want them to be
		// as similar as possible

		double ratio = sqrt((double)prices[i + 1] / (double)prices[i]);
		amount_ratios[i - 1] = ratio;
	}

	ull biggest_amount = 0;
	// now guess the value of k_1, from it we'll get everything else
	for (ull k = 0; k <= 10000; k++)
	{
		ull item_sum = 0;
		ull amount = 0;
		double current_k = (double)k;
		vector<ull> amounts(N, 0); // for debugging

		// add the first item
		amount += current_k;
		item_sum += (current_k * current_k) * prices[1];
		amounts[0] = amount;
		for (ul i = 1; i < N - 1; i++)
		{
			current_k = (double)current_k / amount_ratios[i - 1];
			ull item_amount = (ull)current_k;

			amount += item_amount;
			item_sum += (ull)((item_amount * item_amount)*prices[i + 1]);
			amounts[i] = item_amount;
		}

		if (item_sum > M)
			break;

		if (amount > biggest_amount)
		{
			biggest_amount = amount;
		}
	}

	cout << biggest_amount << endl;
}

int main()
{
	ex5();
	return 0;
}