#include <iostream>
#include <string>
#include <stack>
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
	// we're calculating the distance sum of all children in each vertex.
	// no need to do so for the vertex itself
	if (children == edges.end())
		return;

	for (ul child : children->second)
	{
		if (child == parent)
			continue;

		// apply the formula described earlier:
		// D[n] = D[P(n)] + N - 2 * S(n)
		distances[child] = (distances[node] - 2 * (ull)size_of_tree[child]) + N;

		// continue the DFS scan
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

	// the tree is undirected, insert each edge in both direction
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

	// arbitrarily choose 1 as the root, in an undirected tree each vertex can be
	// the root
	ul root = 1;

	std::unordered_map<ul, ul> size_of_tree;
	// calculate the size of the tree under the root
	ex7SizeOfTree(root, edges, size_of_tree, 0);

	std::unordered_map <ul, ull> distances;
	// calculate in O(n) the sum of distances from the root to all other edges
	ull root_distances = ex7SumDistancesFromRootToTree(edges, 0, root, 0);
	distances[root] = root_distances;

	// now perform a DFS starting from the root, using the equation defined at the beginning
	// to compute in O(1) for each vertex the sum of distances from it to any other vertex
	// in the graph
	ex7CalcDistances(distances, edges, size_of_tree, root, N, 0);

	for (ul i = 1; i <= N; i++)
	{
		std::cout << distances[i] << " ";
	}
}

// If key exists in the map, add the value to the value.
// Otherwise, assign it.
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
		// this is a leaf, it has exactly the amount of civilians defined in the input,
		// and there is one of it.
		return std::make_shared<map<ull, ul>>(map<ull, ul>{ {citizens[node], 1} });
	}

	shared_ptr<map<ull, ul>> citizens_in_leaves;
	vector<shared_ptr<map<ull, ul>>> children_leaves;
	ul biggest_leaves = 0;
	ul biggest_leaves_index = ULONG_MAX;

	// recursively get the amount of civilians in all of the children
	// find the child with the biggest balanced tree of "civilians in leaves".
	// this is crucial to avoid O(n^2) computation time.
	// consider a tree: v1->v2->v3->...v(n-1)->vn, if we were to create a new data structure
	// at each level of the recursion we will have to copy all of the data over and over N times.
	//
	// Instead we take the biggest child's result and use it as the initial data structure for
	// this level. Then we apply the other children's result on top of it, this will ensure no
	// more than O(nlogn) operations is done during the recursion.
	for (ul child: edges[node])
	{
		children_leaves.push_back(ex6CitizensInLeaves(child, citizens, edges));
		if (children_leaves.back()->size() > biggest_leaves)
		{
			biggest_leaves_index = children_leaves.size() - 1;
			biggest_leaves = children_leaves.back()->size();
		}
	}

	// intialize this vertex' data structure to be the biggest data structure of the children
	citizens_in_leaves = children_leaves[biggest_leaves_index];
	children_leaves.erase(find(children_leaves.begin(), children_leaves.end(), citizens_in_leaves));

	// now merge the rest of the childrens' data with it
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
		// in each iteration - add as much civilians as possible to the smallest leaves, to make
		// them equal to the second-smallest leaves (if there are any).
		// if we have enough civilians to make all the smallest leaves as big as the second smallest,
		// merge them. If not, add as much as we can.
		// If we don't have enough civilians to add to all smallest leaves, add the remainder to some of
		// them and split the smallest leaves into <smallest leaves>, <smallest leaves + 1>.
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

		// update the smallest leaves in the data structure to the new smallest value
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

		// remove the civilians we've "used" in this iteration, so we can know when we're done
		addition -= current_addition;
	}

	return citizens_in_leaves;
}

void ex6()
{
	// The bandits always act after the civilians, which means they can always choose the leaf
	// with the most people.
	//
	// This means the only logical way the civilian should split is to make the biggest leaf
	// (all civilians will end up at the leaves) contain as little of civilians as possible.
	//
	// This can be done by splitting the civilians into equal groups as much as possible.
	// If two leaves contain an un-even amount of civilians, future civilians arriving
	// from upper vertices should attempt to even out the difference between them.
	//
	// Therefore we only need to save the amount of civilians in each leaf, ordered by the amount,
	// then split the civilians in each vertex (moving up from the leaves) among the leaves
	// with the least civilians.
	//
	// We can keep a balanced tree (std::map) where the key is the amount of civilians and the value
	// is the amount of leaves with such an amount. Then for each vertex we'll add the amount
	// of civilians into the smallest leaves until they have the same value as the next leaf size.
	// We'll continue until there are no more civilians in the current vertex.
	//
	// Finally, the result will be the biggest leaf.

	ul n;
	cin >> n;

	// get the edges input
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

	// get the citizens input
	unordered_map<ul, ull> citizens;
	for (ul node = 1; node <= n; node++)
	{
		ull amount;
		cin >> amount;

		citizens[node] = amount;
	}

	// use a shared_ptr as the return value of ex6CitizensInLeaves.
	// this will allow us to avoid copying the returned value on each recursion.
	shared_ptr<map<ull, ul>> leaves = ex6CitizensInLeaves(1, citizens, edges);
	cout << leaves->rbegin()->first << endl;
}

void tarjanDFS(ul node, ul& index, vector<ul>& indices, vector<ul>& lowlink, vector<bool>& onStack,
			   stack<ul>& s, unordered_map<ul, vector<ul>>& edges, vector<unordered_set<ul>>& sccs)
{
	indices[node] = index;
	lowlink[node] = index;
	index++;
	s.push(node);
	onStack[node] = true;

	// Explore all neighbors of the current node
	for (ul neighbor : edges[node]) {
		if (indices[neighbor] == -1) {
			// Neighbor has not been visited, recurse on it
			tarjanDFS(neighbor, index, indices, lowlink, onStack, s, edges, sccs);
			lowlink[node] = min(lowlink[node], lowlink[neighbor]);
		}
		else if (onStack[neighbor]) {
			// Neighbor is in the stack and hence in the current SCC
			lowlink[node] = min(lowlink[node], indices[neighbor]);
		}
	}

	// If node is a root node, pop the stack and generate an SCC
	if (lowlink[node] == indices[node]) {
		unordered_set<ul> scc;
		ul w;
		do {
			w = s.top();
			s.pop();
			onStack[w] = false;
			scc.insert(w);
		} while (w != node);
		sccs.push_back(scc);
	}
}

vector<unordered_set<ul>> findSCCs(unordered_map<ul, vector<ul>>& edges, ul N) {
	// the data structure required for the implementation of Tarjan's Algorithm (taken from the
	// presentation with a slight adjustment for the input/output types.
	vector<ul> indices(N + 1, -1);
	vector<ul> lowlink(N + 1, -1);
	vector<bool> onStack(N + 1, false);
	stack<ul> s;
	vector<unordered_set<ul>> sccs;
	ul index = 0;

	// Perform DFS for each node that has not been visited
	for (ul i = 1; i <= N; i++) {
		if (indices[i] == -1) {
			tarjanDFS(i, index, indices, lowlink, onStack, s, edges, sccs);
		}
	}

	return sccs;
}

void ex8()
{
	// In order to the capital to be able to reach every other vertex it needs
	// an edge into every connected component to which no other vertex (outside of the SCC) has an
	// incoming edge into, except for (of course) the SCC of the capital itself.
	// 
	// We find all the SCCs using Tarjan's Algorithm and then find all the "edges between SCCs".
	// We then need a single edge into every "root SCC" that the capital is not a part of.
	// That is, an SCC that has no incoming edge into it from a vertex that is not part of the
	// SCC.

	ul n, m, s;
	cin >> n;
	cin >> m;
	cin >> s;

	// get the edges from the input
	unordered_map<ul, vector<ul>> edges;
	for (ul i = 0; i < m; i++)
	{
		ul source, dst;
		cin >> source;
		cin >> dst;

		if (edges.find(source) == edges.end())
			edges[source] = {};

		edges[source].push_back(dst);
	}

	// Find all strongly connected components using Tarjan's Algorithm
	vector<unordered_set<ul>> sccs = findSCCs(edges, n);

	// map all vertices into their respective SCC, and also initialize all SCCs
	// as "root sccs". We will remove some of them from this set later on.
	unordered_map<ul, ul> vertex_to_scc;
	unordered_set<ul> root_sccs;
	for (ul scc_index = 0; scc_index < sccs.size(); scc_index++)
	{
		for (ul vertex : sccs[scc_index])
		{
			vertex_to_scc[vertex] = scc_index;
		}
		root_sccs.insert(scc_index);
	}

	// for each edge, if it connects vertices in different SCCs, determine the
	// destination vertex' SCC is not a "root SCC".
	for (auto& vertex_edges : edges)
	{
		ul source_vertex = vertex_edges.first;
		for (auto dst : vertex_edges.second)
		{
			ul dst_scc_index = vertex_to_scc[dst];
			ul src_scc_index = vertex_to_scc[source_vertex];
			if (src_scc_index == dst_scc_index)
				continue;

			root_sccs.erase(dst_scc_index);
		}
	}

	// if the capital is inside a root SCC, we don't need an edge to it.
	// other than that - we need an SCC to all other "root SCCs"
	ul edges_needed = static_cast<ul>(root_sccs.size());
	ul capital_scc_index = vertex_to_scc[s];
	if (root_sccs.find(capital_scc_index) != root_sccs.end())
	{
		edges_needed--;
	}
	cout << edges_needed << endl;
	return;
}

void ex3AssignDishValue(unordered_map<string, pair<ull, ull>>& dish_values,
	const string& node, ull prestige, ull price)
{
	auto node_values = dish_values.find(node);
	if (node_values == dish_values.end())
	{
		dish_values[node] = { prestige, price };
		return;
	}

	pair<ull, ull>& prev_values = node_values->second;
	if (price > prev_values.second)
		return;

	if (price < prev_values.second)
	{
		prev_values = { prestige, price };
		return;
	}

	if (prestige > prev_values.first)
	{
		prev_values = { prestige, price };
	}
}

void ex3DfsCalcDishValues(unordered_map<string, pair<ull, ull>>& dish_values,
	unordered_map<string, vector<tuple<string, ul, ul>>>& edges, const string& node,
	ull prestige, ull price)
{
	ex3AssignDishValue(dish_values, node, prestige, price);

	auto children = edges.find(node);
	if (children == edges.end())
	{
		return;
	}

	for (tuple<string, ul, ul>& child : children->second)
	{
		ex3DfsCalcDishValues(dish_values, edges, get<0>(child),
							 prestige + get<1>(child), price + get<2>(child));
	}
}

void ex3KnapsackProblem(const std::vector<std::pair<ull, ull>>& items, ul budget, ul& budget_used,
					    ull& maximal_value)
{
	ul n = items.size();
	// prestige[i][j] is the prestige achievable while only considering the first i-1 items, and having
	// a maximal budget of j
	vector<vector<ull>> prestige(n + 1, std::vector<ull>(budget + 1, 0));

	// Fill the DP table
	for (ul i = 1; i <= n; ++i) {
		for (ull b = 0; b <= budget; ++b) {
			if (items[i - 1].second <= b) {
				prestige[i][b] = max(prestige[i - 1][b], prestige[i - 1][b - items[i - 1].second] + items[i - 1].first);
			}
			else {
				prestige[i][b] = prestige[i - 1][b];
			}
		}
	}

	// The highest worth achievable within the budget
	ull maximal_prestige = prestige[n][budget];

	// the minimal budget achieving maximal prestige (but within the budget defined)
	ull minimal_budget = budget;

	while (minimal_budget > 0 && prestige[n][minimal_budget - 1] == maximal_prestige)
	{
		minimal_budget--;
	}

	maximal_value = maximal_prestige;
	budget_used = minimal_budget;
	return;

	//// Backtrack to find the total budget used
	//ull remainingBudget = budget;
	//budget_used = 0;
	//for (ull i = n; i > 0 && maximal_value > 0; --i) {
	//	if (prestige[i][remainingBudget] != prestige[i - 1][remainingBudget]) {
	//		budget_used += items[i - 1].second;
	//		maximal_value -= items[i - 1].first;
	//		remainingBudget -= items[i - 1].second;
	//	}
	//}

	//// Restore the original worth value
	//maximal_value = prestige[n][budget];
}

void ex3()
{
	// Create a graph from the dishes input.
	// Then DFS every root (base dish) in the graph to get the total prestige and cost
	// of every dish.
	//
	// Finally, solve the problem using the Knapsack's Problem algorithm.
	ul budget;
	cin >> budget;

	ul N;
	cin >> N;

	ul current_index = 0;
	// each dish has a list of derived dishes
	// each "Edge" to a derived dish has 3 values:
	// { dst dish, prestige, price}
	unordered_map<string, vector<tuple<string, ul, ul>>> edges;

	unordered_set<string> derived_dishes;

	// get the input edges
	for (ul i = 0; i < N; i++)
	{
		string derived_dish;
		string source_dish;
		string ingredient;
		ul price;
		ul prestige;
		cin >> derived_dish;
		cin >> source_dish;
		cin >> ingredient;
		cin >> price;
		cin >> prestige;

		derived_dishes.insert(derived_dish);

		if (edges.find(source_dish) == edges.end())
		{
			edges[source_dish] = {};
		}

		edges[source_dish].push_back({ derived_dish, prestige, price });
	}

	/*if (N > 100000)
		throw;*/

	// key is the dish index
	// value is <prestige, price>
	// cumulative value might pass 32 bit so we use ull
	unordered_map<string, pair<ull, ull>> dish_values;
	for (auto& dish : edges)
	{
		if (derived_dishes.find(dish.first) != derived_dishes.end())
		{
			// not a base dish
			continue;
		}

		// time limit exceeded here for N > 100,000
		ex3DfsCalcDishValues(dish_values, edges, dish.first, 0, 0);
	}

	vector<pair<ull, ull>> dish_values_vector;
	for (auto& item : dish_values)
	{
		dish_values_vector.push_back(item.second);
	}

	ul budget_used = 0;
	ull prestige_achieved = 0;
	ex3KnapsackProblem(dish_values_vector, budget, budget_used, prestige_achieved);
	cout << prestige_achieved << endl;
	cout << budget_used << endl;
}

int main()
{
	ios_base::sync_with_stdio(false);
	cin.tie(nullptr);

	ex3();
	return 0;
}