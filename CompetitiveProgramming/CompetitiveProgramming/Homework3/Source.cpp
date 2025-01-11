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
#define MOD (ull)1e9+9

#define max(x, y) x > y ? x : y
#define min(x, y) x > y ? y : x

vector<ull> ex1PowersOfTwo(ul size)
{
	vector<ull> powers(size, 0);
	ull current_power = 1;
	for (ul i = 0; i < size; i++)
	{
		powers[i] = current_power;
		current_power <<= 1;
		current_power %= MOD;
	}

	return powers;
}

ull ex1KarpRabinFull(vector<vector<char>>& mat, ul hp, ul wp, vector<ull>& powers)
{
	// each cell is given a bit 0 or 1, each column multiplies this number by a different
	// power of 2.
	// so column wp-1 is power of 0, column wp-2 power of 1 and generally speaking column
	// i is power of wp-i-1.

	ull hash_val = 0;
	for (ul row = 0; row < hp; row++)
	{
		for (ul col = 0; col < wp; col++)
		{
			hash_val += (ull)mat[row][col] * powers[wp - col - 1];
			hash_val %= MOD;
		}
	}

	return hash_val;
}

ull ex1ModSubtract(ull left, ull right)
{
	return left >= right ? left - right : (left + MOD - right) % MOD;
}

// i - the index of the first row in the previous sub-matrix
// j - the index of the first column in the previous sub-matrix
// we know shift the matrix left so that its upper-left point will be (i, j+1)
ull ex1KarpRabinShiftRight(vector<vector<char>>& mat, vector<ull>& powers, ul hp, ul wp, ul i, ul j, ull hash_val)
{
	// subtract left (most significant column)
	ull left_col_value = 0;
	for (ul row_offset = 0; row_offset < hp; row_offset++)
	{
		ul row = i + row_offset;
		left_col_value += (ull)mat[row][j] * powers[wp - 1];
		left_col_value %= MOD;
	}
	hash_val = ex1ModSubtract(hash_val, left_col_value);

	// multiply all values by the base 2
	hash_val *= 2;
	hash_val %= MOD;

	// add new right column (least significant)
	ull right_col_value = 0;
	for (ul row_offset = 0; row_offset < hp; row_offset++)
	{
		ul row = i + row_offset;
		right_col_value += (ull)mat[row][j+wp] * powers[0];
		right_col_value %= MOD;
	}
	hash_val += right_col_value;
	hash_val %= MOD;

	return hash_val;
}

ull ex1KarpRabinShiftDown(vector<vector<char>>& mat, vector<ull>& powers, ul hp, ul wp, ul i, ul j, ull hash_val)
{
	// subtract upper row
	ull upper_row_value = 0;
	for (ul col = 0; col < hp; col++)
	{
		upper_row_value += (ull)mat[i][col] * powers[wp - col - 1];
		upper_row_value %= MOD;
	}
	hash_val = ex1ModSubtract(hash_val, upper_row_value);

	// no need to multiply the hash now - all the columns remain with the same power

	// add new bottom row 
	ull bottom_row_value = 0;
	for (ul col = 0; col < hp; col++)
	{
		bottom_row_value += (ull)mat[i+hp][col] * powers[wp - col - 1];
		bottom_row_value %= MOD;
	}
	hash_val += bottom_row_value;
	hash_val %= MOD;

	return hash_val;
}

bool ex1CheckSubMatrix(vector<vector<char>>& big_mat, vector<vector<char>>& small_mat, ul row_start, ul col_start)
{
	for (ul i = 0; i < small_mat.size(); i++)
	{
		for (ul j = 0; j < small_mat[0].size(); j++)
		{
			if (small_mat[i][j] != big_mat[i + row_start][j + col_start])
				return false;
		}
	}

	return true;
}

void ex1()
{
	ul hp, wp, hm, wm;

		cin >> hp >> wp >> hm >> wm;

	vector<ull> powers = ex1PowersOfTwo(wp);
	vector<vector<char>> mat(hm, vector<char>(wm, (char)0));
	vector<vector<char>> small_mat(hp, vector<char>(wp, (char)0));
	for (ul i = 0; i < hp; i++)
	{
		for (ul j = 0; j < wp; j++)
		{
			char c;
			cin >> c;
			if (c == 'x')
				small_mat[i][j] = 1;
			else
				small_mat[i][j] = 0;
		}
	}

	ull target_hash = ex1KarpRabinFull(small_mat, hp, wp, powers);

	for (ul i = 0; i < hm; i++)
	{
		for (ul j = 0; j < wm; j++)
		{
			char c;
			cin >> c;
			if (c == 'x')
				mat[i][j] = 1;
			else
				mat[i][j] = 0;
		}
	}

	ull start_hash = ex1KarpRabinFull(mat, hp, wp, powers);
	ull matches = 0;

	for (ul row = 0; row <= hm - hp; row++)
	{
		ull current_hash = start_hash;
		for (ull col = 0; col <= wm - wp; col++)
		{
			if (ex1CheckSubMatrix(mat, small_mat, row, col))
			{
				if (current_hash != target_hash)
					continue;
				matches++;
			}

			if (col < wm - wp)
			{
				// shift left
				current_hash = ex1KarpRabinShiftRight(mat, powers, hp, wp, row, col, current_hash);
			}
		}

		if (row < hm - hp)
		{
			start_hash = ex1KarpRabinShiftDown(mat, powers, hp, wp, row, 0, start_hash);
		}
	}

	cout << matches << endl;
}

int main()
{
	ex1();
	return 0;
}