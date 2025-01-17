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
	vector<vector<ull>> col_hashes(hm - hp + 1, vector<ull>(wm, 0));
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

	for (ul row = 1; row <= hm - hp; row++)
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
			ull lower_cell = mat[row + hp - 1][col] * powers[0];
			hash_value += lower_cell;
			hash_value %= MOD;

			col_hashes[row][col] = hash_value;
		}
	}

	return col_hashes;
}

vector<vector<ull>> ex1RowHashes(vector<vector<char>>& mat, vector<ull>& powers, ul hp, ul wp, ul hm, ul wm)
{
	vector<vector<ull>> row_hashes(hm, vector<ull>(wm - wp + 1, 0));
	for (ul row = 0; row < hm; row++)
	{
		ull hash_value = 0;
		for (ul col = 0; col < wp; col++)
		{
			// treat this row as if it were the bottom row in a sub-matrix
			ull power = ex1CellPower(hp - 1, col, hp, wp);
			hash_value += mat[row][col] * powers[power];
			hash_value %= MOD;
		}

		row_hashes[row][0] = hash_value;
	}

	for (ul col = 1; col <= wm - wp; col++)
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
			ull right_cell = mat[row][col + wp - 1] * powers[0];
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
ull ex1KarpRabinShiftRight(vector<vector<char>>& mat, vector<ull>& powers, vector<vector<ull>>& col_hashes, ul hp, ul wp, ul i, ul j, ull hash_val)
{
	// subtract left (most significant column)
	ull left_col_hash = col_hashes[i][j];
	// adjust the column hash to the fact this column is treated as the most significant
	left_col_hash *= powers[wp - 1];
	left_col_hash %= MOD;

	hash_val = ex1ModSubtract(hash_val, left_col_hash);

	// multiply all values by the base 2
	hash_val *= 2;
	hash_val %= MOD;

	// add new right column (least significant)
	ull right_col_hash = col_hashes[i][j + wp];
	hash_val += right_col_hash;
	hash_val %= MOD;

	return hash_val;
}

ull ex1KarpRabinShiftDown(vector<vector<char>>& mat, vector<ull>& powers, vector<vector<ull>>& row_hashes,
	ul hp, ul wp, ul i, ul j, ull hash_val)
{
	// subtract upper row
	ull upper_row_hash = row_hashes[i][j];
	// adjust the row hash to the fact we're treating it as the most significant (it is the upper row)
	upper_row_hash *= powers[wp * (hp - 1)];
	upper_row_hash %= MOD;

	hash_val = ex1ModSubtract(hash_val, upper_row_hash);

	// multiply all values by 2^wp
	ull mult = powers[wp];
	hash_val *= mult;
	hash_val %= MOD;

	// add new bottom row 
	ull bottom_row_hash = row_hashes[i + hp][j];
	hash_val += bottom_row_hash;
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

	vector<vector<ull>> col_hashes = ex1ColumnHash(mat, powers, hp, wp, hm, wm);
	vector<vector<ull>> row_hashes = ex1RowHashes(mat, powers, hp, wp, hm, wm);

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
				current_hash = ex1KarpRabinShiftRight(mat, powers, col_hashes, hp, wp, row, col, current_hash);
			}
		}

		if (row < hm - hp)
		{
			start_hash = ex1KarpRabinShiftDown(mat, powers, row_hashes, hp, wp, row, 0, start_hash);
		}
	}

	cout << matches << endl;
}

void ex2Dfs(vector<unordered_set<ul>>& edges, ul node, ul parent, unordered_set<ul>& visited,
			ul region, vector<ul>& city_region, ull& region_population, vector<ul>& populations,
			unordered_set<ul>& region_cities)
{
	if (visited.find(node) != visited.end())
		return;

	visited.insert(node);
	region_cities.insert(node);
	city_region[node] = region;
	region_population += populations[node];

	unordered_set<ul>& neighbors = edges[node];
	for (ul neighbor : neighbors)
	{
		if (neighbor == parent)
			continue;

		ex2Dfs(edges, neighbor, node, visited, region, city_region, region_population, populations,
			   region_cities);
	}
}

ull ex2CurrentBiggestRegion(set<tuple<ull, ul>>& ordered_region_populations)
{
	return get<0>(*ordered_region_populations.rbegin());
}

void ex2()
{
	ul N, M, Q;

	cin >> N >> M >> Q;

	vector<ul> populations(N + 1, 0);
	for (ul i = 1; i <= N; i++)
	{
		cin >> populations[i];
	}

	vector<unordered_set<ul>> edges(N + 1, unordered_set<ul>{});
	vector<pair<ul, ul>> roads(M + 1, {0, 0});
	for (ul i = 1; i <= M; i++)
	{
		ul a, b;
		cin >> a >> b;
		edges[a].insert(b);
		edges[b].insert(a);

		roads[i] = { a, b };
	}

	// we now record the queries and then treat them as if they were executed in backward order
	vector<tuple<char, ul, ull>> queries(Q, { ' ', 0, 0 });
	for (ul i = 0; i < Q; i++)
	{
		char query_type;
		ul num1;
		ull num2 = 0;
		cin >> query_type >> num1;
		if (query_type == 'P')
		{
			cin >> num2;
		}

		if (query_type == 'D')
		{
			auto vertices = roads[num1];
			edges[vertices.first].erase(vertices.second);
			edges[vertices.second].erase(vertices.first);
		}
		else
		{
			// swap the population in the query with the current population to allow us to go through
			// it in reverse order
			ull prev_population = populations[num1];
			populations[num1] = num2;
			num2 = prev_population;
		}

		queries[i] = { query_type, num1, num2 };
	}

	// discover all connected components of the graph, assign each connected component a region number
	// and sum the values in every region.
	//
	// put the regions into an oredered set to allow us to easily find the biggest one
	//
	// next, process the queries in reverse order.
	// on population assign, simply modify the population of the city and then change the region's
	// population appropriately
	//
	// on edge addition take the smaller region and change all of its vertexes to belong to the bigger
	// region. combine their population, put the new value into the ordered set and remove the two
	// separated populations from it
	//
	// on each step, retrieve the biggest value in the ordered set and save it in a vector to be printed
	// out at the end
	vector<ul> city_region(N + 1, UL_MAX);
	vector<ull> region_poulation(N, 0);
	set<tuple<ull, ul>> ordered_region_populations;
	vector<unordered_set<ul>> region_cities(N, unordered_set<ul>{});

	// DFS the graph until all connected components are found
	unordered_set<ul> visited;
	for (ul i = 1; i <= N; i++)
	{
		ul region = i - 1;
		ull region_population = 0;
		unordered_set<ul> cities;
		ex2Dfs(edges, i, 0, visited, region, city_region, region_population, populations, cities);

		if (region_population > 0)
		{
			region_poulation[region] = region_population;
			ordered_region_populations.insert({ region_population, region });
			region_cities[region] = std::move(cities);
		}
	}

	vector<ull> results (Q, 0);
	results[Q - 1] = ex2CurrentBiggestRegion(ordered_region_populations);

	// process queries in reverse order
	for (ll q = Q - 1; q > 0; q--)
	{
		if (get<0>(queries[q]) == 'P')
		{
			// population change
			ul city = get<1>(queries[q]);
			ull new_population = get<2>(queries[q]);

			ull prev_population = populations[city];
			populations[city] = new_population;

			ul region = city_region[city];
			ull prev_region_population = region_poulation[region];

			ull new_region_population = prev_region_population - prev_population + new_population;
			region_poulation[region] = new_region_population;

			ordered_region_populations.erase({ prev_region_population, region });
			ordered_region_populations.insert({ new_region_population, region });
		}
		else
		{
			// a new road is added
			ul road = get<1>(queries[q]);

			auto& cities = roads[road];
			ul region1 = city_region[cities.first];
			ul region2 = city_region[cities.second];

			if (region1 == region2)
			{
				// road doesn't change anything
				results[q - 1] = ex2CurrentBiggestRegion(ordered_region_populations);
				continue;
			}
			
			// the new road connects two different regions, which are now merged into one
			// take the smaller region (by amount of cities) and merge it into the bigger one
			// this ensures we copy at most O(n) cities during the execution of the algorithm
			ul smaller_region = region1;
			ul bigger_region = region2;

			if (region_cities[region1].size() > region_cities[region2].size())
			{
				smaller_region = region2;
				bigger_region = region1;
			}

			// change the region for all region2 cities
			for (ul city : region_cities[smaller_region])
			{
				city_region[city] = bigger_region;
			}

			// append those cities to the "region_cities" structure
			region_cities[bigger_region].insert(
				region_cities[smaller_region].begin(),
				region_cities[smaller_region].end());

			ordered_region_populations.erase({ region_poulation[bigger_region], bigger_region });
			ordered_region_populations.erase({ region_poulation[smaller_region], smaller_region });

			// adjust the population of the bigger region accordingly
			region_poulation[bigger_region] += region_poulation[smaller_region];

			ordered_region_populations.insert({ region_poulation[bigger_region], bigger_region });
		}

		results[q - 1] = ex2CurrentBiggestRegion(ordered_region_populations);
	}

	for (ul i = 0; i < Q; i++)
	{
		cout << results[i] << endl;
	}
}

int main()
{
	ex2();
	return 0;
}
