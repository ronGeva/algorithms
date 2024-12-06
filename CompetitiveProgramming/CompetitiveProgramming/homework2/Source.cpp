#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <set>
#include <map>
#include <memory>
#include <limits.h>

using namespace std;
typedef unsigned long long ull;
typedef unsigned long ul;
#define max(x, y) x > y ? x : y
#define min(x, y) x > y ? y : x

ul ex7SizeOfTree(ul node, std::unordered_map<ul, std::vector<ul>>& edges,
				 std::unordered_map<ul, ul>& size_of_tree, ul parent)
{
	auto children = edges.find(node);

	// leaf - size of tree is 1
	if (children == edges.end())
	{
		size_of_tree[node] = 1;
	}
	else
	{
		ul tree_size = 1;
		for (ul child : children->second)
		{
			if (child == parent)
				continue;
			tree_size += ex7SizeOfTree(child, edges, size_of_tree, node);
		}

		size_of_tree[node] = tree_size;
	}

	return size_of_tree[node];
}

ull ex7SumDistancesFromRootToTree(std::unordered_map<ul, std::vector<ul>>& edges, ul depth, ul node, ul parent)
{
	auto children = edges.find(node);

	// this is a leaf, the total distance of this subtree is just the depth of the leaf
	if (children == edges.end())
		return depth;

	ull sum = 0;
	for (ul child : children->second)
	{
		if (child == parent)
			continue;
		sum += ex7SumDistancesFromRootToTree(edges, depth + 1, child, node);
	}

	// return sum of all children's distances, with addition to the distance to the current node
	return sum + depth;
}

void ex7CalcDistances(std::unordered_map<ul, ull>& distances,
				      const std::unordered_map<ul, std::vector<ul>>& edges,
					  std::unordered_map<ul, ul>& size_of_tree,
					  ul node, ul N, ul parent)
{
	auto children = edges.find(node);
	if (children == edges.end())
		return;

	for (ul child : children->second)
	{
		if (child == parent)
			continue;

		distances[child] = (distances[node] - 2 * (ull)size_of_tree[child]) + N;
		ex7CalcDistances(distances, edges, size_of_tree, child, N, node);
	}
}

void ex7()
{
	// Let D[n] denote the distance between node n and all other nodes in the
	// tree.
	//
	// Let P(n) denote the parent node of node n.
	// 
	// Let N denote the total amount of nodes.
	// 
	// Let S(n) denote the size of the subtree rooted at node n (including the root).
	//
	// D[n] = D[P(n)] + N - 2 * S(n)
	//
	// The is because when we compare all the distances of node n compared to its parent,
	// all the nodes in the subtree rooted at n will now be closer to it by 1 - this reduces
	// the total distance by S(n).
	// The distance to all nodes not rooted at n will now be longer by 1 (as the path to them
	// will have to go through P(n) first, there are exactly N-S(n) nodes like that, so we get
	// the total diff: -S(n) + (N - S(n)) = N - 2S(n).
	ul N;
	std::cin >> N;

	std::unordered_map<ul, std::vector<ul>> edges;
	for (ul i = 0; i < N - 1; i++)
	{
		ul source, dst;
		std::cin >> source;
		std::cin >> dst;

		if (edges.find(source) == edges.end())
		{
			edges[source] = {};
		}

		edges[source].push_back(dst);

		ul tmp = source;
		source = dst;
		dst = tmp;
		if (edges.find(source) == edges.end())
		{
			edges[source] = {};
		}

		edges[source].push_back(dst);
	}

	// arbitrarily choose 1 as the root
	ul root = 1;

	std::unordered_map<ul, ul> size_of_tree;
	ex7SizeOfTree(root, edges, size_of_tree, 0);

	std::unordered_map <ul, ull> distances;
	ull root_distances = ex7SumDistancesFromRootToTree(edges, 0, root, 0);
	distances[root] = root_distances;

	ex7CalcDistances(distances, edges, size_of_tree, root, N, 0);

	for (ul i = 1; i <= N; i++)
	{
		std::cout << distances[i] << " ";
	}
}

bool ex6CompareLeaves(pair<ull, ul> a, pair<ull, ul> b)
{
	return a.first > b.first;
}

void ex6MapAssignOrAdd(map<ull, ul>& m, ull key, ul value)
{
	auto it = m.find(key);
	if (it == m.end())
	{
		m[key] = value;
	}
	else
	{
		it->second += value;
	}
}

shared_ptr<map<ull, ul>> ex6CitizensInLeaves(ul node, unordered_map<ul, ull>& citizens,
	unordered_map<ul, vector<ul>>& edges)
{
	if (edges.find(node) == edges.end())
	{
		return std::make_shared<map<ull, ul>>(map<ull, ul>{ {citizens[node], 1} });
	}

	shared_ptr<map<ull, ul>> citizens_in_leaves;
	vector<shared_ptr<map<ull, ul>>> children_leaves;
	ul biggest_leaves = 0;
	ul biggest_leaves_index = ULONG_MAX;
	for (ul child: edges[node])
	{
		children_leaves.push_back(ex6CitizensInLeaves(child, citizens, edges));
		if (children_leaves.back()->size() > biggest_leaves)
		{
			biggest_leaves_index = children_leaves.size() - 1;
			biggest_leaves = children_leaves.back()->size();
		}
	}
	citizens_in_leaves = children_leaves[biggest_leaves_index];
	children_leaves.erase(find(children_leaves.begin(), children_leaves.end(), citizens_in_leaves));

	for (shared_ptr<map<ull, ul>> child_leaves : children_leaves)
	{
		for (auto leaf : *child_leaves)
		{
			ex6MapAssignOrAdd(*citizens_in_leaves, leaf.first, leaf.second);
		}
	}

	ull addition = citizens[node];
	while (addition > 0)
	{
		auto smallest_leaves = *citizens_in_leaves->begin();
		auto second_smallest = next(citizens_in_leaves->begin());
		ull needed_to_merge;
		if (second_smallest != citizens_in_leaves->end())
			needed_to_merge = (second_smallest->first - smallest_leaves.first) * smallest_leaves.second;
		else
			needed_to_merge = ULLONG_MAX;

		ull current_addition = min(addition, needed_to_merge);
		ull addition_per_leaf = current_addition / smallest_leaves.second;
		ull smallest_value = smallest_leaves.first + addition_per_leaf;

		citizens_in_leaves->erase(smallest_leaves.first);
		ex6MapAssignOrAdd(*citizens_in_leaves, smallest_value, smallest_leaves.second);

		if (current_addition % smallest_leaves.second != 0)
		{
			// if addition % <amount of smallest leaves> != 0 then what we need to do is:
			// add addition/<amount> to all smallest leaves.
			// add 1 to addition % <amount> leaves, thus seperating them from the other leaves
			ul leaves_incremented = current_addition % smallest_leaves.second;

			(*citizens_in_leaves)[smallest_value] -= leaves_incremented;
			ex6MapAssignOrAdd(*citizens_in_leaves, smallest_value + 1, leaves_incremented);
		}

		addition -= current_addition;
	}

	return citizens_in_leaves;
}

void ex6()
{
	ul n;
	cin >> n;

	unordered_map<ul, vector<ul>> edges;
	for (ul dst = 2; dst <= n; dst++)
	{
		ul source;
		cin >> source;

		if (edges.find(source) == edges.end())
		{
			edges[source] = {};
		}
		edges[source].push_back(dst);
	}

	unordered_map<ul, ull> citizens;
	for (ul node = 1; node <= n; node++)
	{
		ull amount;
		cin >> amount;

		citizens[node] = amount;
	}

	shared_ptr<map<ull, ul>> leaves = ex6CitizensInLeaves(1, citizens, edges);
	cout << leaves->rbegin()->first << endl;
}

int main()
{
	ex6();
	return 0;
}