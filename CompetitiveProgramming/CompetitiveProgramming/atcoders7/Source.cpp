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
#define UL_MAX 0xffffffff
#define ULL_MAX 0xffffffffffffffff

#define max(x, y) x > y ? x : y
#define min(x, y) x > y ? y : x

void ex1()
{
	ull A, B;
	cin >> A;
	cin >> B;

	ull C = A + B;

	ull c_power = C * C;
	cout << c_power << endl;
}

ull ex4Key(ul i, ul j)
{
	return ((ull)i << 32) | j;
}

void ex4BfsAddNeighbors(vector<vector<char>>& matrix, pair<ul, ul> vertex, ul visit_direction,
						ul distance, deque<tuple<ul, ul, ul, ul>>& vertices)
{
	if (visit_direction != 1)
	{
		// not a horizontial visit, we can go up and down
		if (vertex.first > 0)
		{
			vertices.push_back({ vertex.first - 1, vertex.second, 1, distance + 1 });
		}

		if (vertex.first < matrix.size() - 1)
		{
			vertices.push_back({ vertex.first + 1, vertex.second, 1, distance + 1 });
		}
	}

	if (visit_direction != 2)
	{
		// not a vertical visit, we can left and right
		if (vertex.second > 0)
		{
			vertices.push_back({ vertex.first, vertex.second - 1, 2, distance + 1 });
		}

		if (vertex.second < matrix[0].size() - 1)
		{
			vertices.push_back({ vertex.first, vertex.second + 1, 2, distance + 1 });
		}
	}
}

void ex4Bfs(vector<vector<char>>& matrix, unordered_map<ull, ull>& horizontial_visit_distance,
	unordered_map<ull, ull>& vertical_visit_distance, pair<ul, ul> start)
{
	// each element is <row, column, visit direction, distance>
	// visit_direction = 1 -> horizontial_visit
	// visit_direction = 2-> vertical_visit
	deque<tuple<ul, ul, ul, ul>> vertices;

	ex4BfsAddNeighbors(matrix, start, 0, 0, vertices);

	while (!vertices.empty())
	{
		tuple<ul, ul, ul, ul> current = vertices.front();
		vertices.pop_front();

		ul row = get<0>(current);
		ul column = get<1>(current);
		ul visit_direction = get<2>(current);
		ul distance = get<3>(current);

		if (matrix[row][column] == '#')
		{
			// obstacle, ignore
			continue;
		}

		ull key = ex4Key(row, column);
		if (visit_direction == 1)
		{
			// if we've already visited this node from this direction then
			// we must've already calculated the fastest way to reach it,
			// since this is BFS
			if (horizontial_visit_distance.find(key) != horizontial_visit_distance.end())
				continue;

			horizontial_visit_distance[key] = distance;
		}

		if (visit_direction == 2)
		{
			// if we've already visited this node from this direction then
			// we must've already calculated the fastest way to reach it,
			// since this is BFS
			if (vertical_visit_distance.find(key) != vertical_visit_distance.end())
				continue;

			vertical_visit_distance[key] = distance;
		}

		ex4BfsAddNeighbors(matrix, { row, column }, visit_direction, distance, vertices);
	}
}

void ex44()
{
	ul H, W;
	cin >> H >> W;

	pair<ul, ul> start;
	pair<ul, ul> goal;

	vector<vector<char>> matrix(H, vector<char>(W));
	for (ul i = 0; i < H; i++)
	{
		string S;
		cin >> S;
		for (ul j = 0; j < W; j++)
		{
			matrix[i][j] = S[j];

			if (matrix[i][j] == 'S')
				start = { i, j };

			if (matrix[i][j] == 'G')
				goal = { i, j };
		}
	}

	unordered_map<ull, ull> horizontial_visit_distance;
	unordered_map<ull, ull> vertical_visit_distance;
	ex4Bfs(matrix, horizontial_visit_distance, vertical_visit_distance, start);
	//ex4Dfs(matrix, horizontial_visit_distance, vertical_visit_distance, start, 0, 0);

	ull goal_key = ex4Key(goal.first, goal.second);
	ull distance = UL_MAX;
	if (horizontial_visit_distance.find(goal_key) != horizontial_visit_distance.end())
	{
		distance = min(distance, horizontial_visit_distance[goal_key]);
	}

	if (vertical_visit_distance.find(goal_key) != vertical_visit_distance.end())
	{
		distance = min(distance, (ll)vertical_visit_distance[goal_key]);
	}

	ll result = -1;
	if (distance < UL_MAX)
		result = (ll)distance;

	cout << result << endl;
}

ul ex3DigitsInNum(ul num)
{
	ul digits = 0;
	while (num > 0)
	{
		digits++;
		num /= 10;
	}

	return digits;
}

void ex3()
{
	ull L, R;

	cin >> L, R;

	ul start_digits = ex3DigitsInNum(L);
	ul end_digits = ex3DigitsInNum(R);
}

int main()
{
	ex44();
	return 0;
}