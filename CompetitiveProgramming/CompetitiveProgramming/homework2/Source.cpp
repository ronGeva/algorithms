#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <set>
#include <map>
#include <limits.h>

typedef unsigned long long ull;
typedef unsigned long ul;

ul ex6FindRoot(std::unordered_map<ul, std::vector<ul>>& edges, ul N)
{
	std::unordered_map<ul, std::vector<ul>> reversed_edges;
	for (const auto& node_edges : edges)
	{
		for (ul dst : node_edges.second)
		{
			if (reversed_edges.find(dst) == reversed_edges.end())
			{
				reversed_edges[dst] = {};
			}
			reversed_edges[dst].push_back(node_edges.first);
		}
	}

	for (ul i = 1; i <= N; i++)
	{
		if (reversed_edges.find(i) == reversed_edges.end())
			return i;
	}

	// should never happen
	return 0;
}

ul ex6SizeOfTree(ul node, std::unordered_map<ul, std::vector<ul>>& edges,
				 std::unordered_map<ul, ul>& size_of_tree)
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
			tree_size += ex6SizeOfTree(child, edges, size_of_tree);
		}

		size_of_tree[node] = tree_size;
	}

	return size_of_tree[node];
}

ull ex6SumDistancesFromRootToTree(std::unordered_map<ul, std::vector<ul>>& edges, ul depth, ul node)
{
	auto children = edges.find(node);

	// this is a leaf, the total distance of this subtree is just the depth of the leaf
	if (children == edges.end())
		return depth;

	ull sum = 0;
	for (ul child : children->second)
	{
		sum += ex6SumDistancesFromRootToTree(edges, depth + 1, child);
	}

	// return sum of all children's distances, with addition to the distance to the current node
	return sum + depth;
}

void ex6CalcDistances(std::unordered_map<ul, ull>& distances,
				      const std::unordered_map<ul, std::vector<ul>>& edges,
					  std::unordered_map<ul, ul>& size_of_tree,
					  ul node, ul N)
{
	auto children = edges.find(node);
	if (children == edges.end())
		return;

	for (ul child : children->second)
	{
		distances[child] = distances[node] + N - 2 * size_of_tree[child];
		ex6CalcDistances(distances, edges, size_of_tree, child, N);
	}
}

void ex6()
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
	}

	ul root = ex6FindRoot(edges, N);
	std::unordered_map<ul, ul> size_of_tree;
	ex6SizeOfTree(root, edges, size_of_tree);

	std::unordered_map <ul, ull> distances;
	ull root_distances = ex6SumDistancesFromRootToTree(edges, 0, root);
	distances[root] = root_distances;

	ex6CalcDistances(distances, edges, size_of_tree, root, N);

	for (ul i = 1; i <= N; i++)
	{
		std::cout << distances[i] << " ";
	}
}

int main()
{
	ex6();
	return 0;
}