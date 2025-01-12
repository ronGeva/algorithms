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

inline ull ex1CellPower(ul row, ul col, ul hp, ul wp)
{
	return wp * hp - 1 - col - row * wp;
}

ull ex1ModSubtract(ull left, ull right)
{
	if (left >= right)
		return left - right;

	return left + MOD - right;
}

// returns a matrix describing the hash value of every column size hp in the large matrix
// each column is assumed to be the least-significant column in the sub-matrix
// in order to treat column hashes[row][col] as the left-side column (most significant) we need
// to multiply its hash value by 2^(wp-1)
vector<vector<ull>> ex1ColumnHash(vector<vector<char>>& mat, vector<ull>& powers, ul hp, ul wp, ul hm, ul wm)
{
	vector<vector<ull>> col_hashes(hm - hp, vector<ull>(wm, 0));
	for (ul col = 0; col < wm; col++)
	{
		ull hash_value = 0;
		for (ul row = 0; row < hp; row++)
		{
			// treat this column as if it were the left-most column in a sub-matrix
			ull power = ex1CellPower(row, wp - 1, hp, wp);
			hash_value += mat[row][col] * powers[power];
			hash_value %= MOD;
		}

		col_hashes[0][col] = hash_value;
	}

	for (ul row = 1; row < hm - hp; row++)
	{
		for (ul col = 0; col < wm; col++)
		{
			// shift column down by one cell

			// remove upper cell
			ull hash_value = col_hashes[row - 1][col];
			ull upper_cell = mat[row - 1][col] * powers[wp * (hp - 1)];
			hash_value = ex1ModSubtract(hash_value, upper_cell);

			// multiply all the remaning column items by 2^wp
			hash_value *= powers[wp];

			// add the new bottom cell
			ull lower_cell = mat[row + hp][col] * powers[0];
			hash_value += lower_cell;
			hash_value %= MOD;

			col_hashes[row][col] = hash_value;
		}
	}
}

vector<vector<ull>> ex1RowHashes(vector<vector<char>>& mat, vector<ull>& powers, ul hp, ul wp, ul hm, ul wm)
{
	vector<vector<ull>> row_hashes(hm, vector<ull>(wm - wp, 0));
	for (ul row = 0; row < hm; row++)
	{
		ull hash_value = 0;
		for (ul col = 0; col < wm - wp; col++)
		{
			// treat this row as if it were the bottom row in a sub-matrix
			ull power = ex1CellPower(hp - 1, col, hp, wp);
			hash_value += mat[row][col] * powers[power];
			hash_value %= MOD;
		}

		row_hashes[row][0] = hash_value;
	}

	for (ul col = 1; col < wm - wp; col++)
	{
		for (ul row = 0; row < hm; row++)
		{
			// shift line right by one cell
			ull hash_value = row_hashes[row][col - 1];

			// remove left cell
			ull left_cell = mat[row][col - 1] * powers[wp - 1];
			hash_value = ex1ModSubtract(hash_value, left_cell);

			// multiplyall the remaining row cells by 2
			hash_value *= 2;
			hash_value %= MOD;

			// add the new right cell
			ull right_cell = mat[row][col + wp] * powers[0];
			hash_value += right_cell;
			hash_value %= MOD;

			row_hashes[row][col] = hash_value;
		}
	}

	return row_hashes;
}

ull ex1KarpRabinFull(vector<vector<char>>& mat, ul hp, ul wp, vector<ull>& powers, ul i, ul j)
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
			ull power = ex1CellPower(row, col, hp, wp);
			hash_val += (ull)mat[row + i][col + j] * powers[power];
			hash_val %= MOD;
		}
	}

	return hash_val;
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
		ull power = ex1CellPower(row_offset, 0, hp, wp);
		left_col_value += (ull)mat[row][j] * powers[power];
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
		ull power = ex1CellPower(row_offset, wp - 1, hp, wp);
		right_col_value += (ull)mat[row][j+wp] * powers[power];
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
	for (ul col = 0; col < wp; col++)
	{
		ull power = ex1CellPower(0, col, hp, wp);
		upper_row_value += (ull)mat[i][col] * powers[power];
		upper_row_value %= MOD;
	}
	hash_val = ex1ModSubtract(hash_val, upper_row_value);

	// multiply all values by 2^wp
	ull mult = powers[wp];
	hash_val *= mult;
	hash_val %= MOD;

	// add new bottom row 
	ull bottom_row_value = 0;
	for (ul col = 0; col < wp; col++)
	{
		ull power = ex1CellPower(hp - 1, col, hp, wp);
		bottom_row_value += (ull)mat[i+hp][col] * powers[power];
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

	vector<ull> powers = ex1PowersOfTwo(wp * hp);
	vector<vector<char>> mat(hm, vector<char>(wm, (char)0));
	vector<vector<char>> small_mat(hp, vector<char>(wp, (char)0));
	string str;
	for (ul i = 0; i < hp; i++)
	{
		cin >> str;
		for (ul j = 0; j < str.size(); j++)
		{
			if (str[j] == 'x')
				small_mat[i][j] = 1;
			else
				small_mat[i][j] = 0;
		}
	}

	ull target_hash = ex1KarpRabinFull(small_mat, hp, wp, powers, 0, 0);

	for (ul i = 0; i < hm; i++)
	{
		cin >> str;
		for (ul j = 0; j < str.size(); j++)
		{
			if (str[j] == 'x')
				mat[i][j] = 1;
			else
				mat[i][j] = 0;
		}
	}

	ull start_hash = ex1KarpRabinFull(mat, hp, wp, powers, 0, 0);
	ull matches = 0;

	for (ul row = 0; row <= hm - hp; row++)
	{
		ull current_hash = start_hash;
		for (ull col = 0; col <= wm - wp; col++)
		{
			if (current_hash == target_hash)
			{
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