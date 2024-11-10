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

// ******************** EX1 ********************

#define max(x, y) x > y ? x : y
#define min(x, y) x > y ? y : x

ul absDiff(long left, long right)
{
	long bigger = max(left, right);
	long smaller = min(left, right);
	return static_cast<ul>(bigger - smaller);
}

ul getMaximizedScore(const std::vector<ul>& arr)
{
	ul minimal_diff = 0xffffffff;

	for (ull i = 0; i < arr.size(); i++)
	{
		ul current_diff = 0;
		if (i != 0)
		{
			current_diff = absDiff(arr[i - 1], arr[i]);
		}

		if (i != arr.size() - 1)
		{
			current_diff = max(current_diff, absDiff(arr[i], arr[i + 1]));
		}

		minimal_diff = min(minimal_diff, current_diff);
	}

	return minimal_diff;
}

int ex1()
{
	ull test_cases;
	std::cin >> test_cases;

	for (ull i = 0; i < test_cases; i++)
	{
		ull array_size;
		std::cin >> array_size;

		std::vector<ul> arr;
		for (size_t j = 0; j < array_size; j++)
		{
			ul item;
			std::cin >> item;
			arr.push_back(item);
		}

		std::cout << getMaximizedScore(arr) << std::endl;
	}

	return 0;
}

// ******************** EX2 ********************

struct RangeItem {
	ull start;
	ull end;
	ul index;
};

int ex2()
{
	ul test_cases;
	std::cin >> test_cases;

	std::vector<RangeItem> items;
	for (ul i = 0; i < test_cases; i++)
	{
		ull start, end;
		std::cin >> start;
		std::cin >> end;

		auto item = RangeItem{
			.start = start,
			.end = end,
			.index = i
		};

		items.push_back(item);
	}

	// sort the items in place according to their start location
	std::sort(items.begin(), items.end(),
		[&](const RangeItem& left, const RangeItem& right)
		{
			return left.start < right.start;
		});

	std::unordered_set<ul> containing_items;
	std::unordered_set<ul> contained_items;

	ull smallest_end = ULONG_MAX;
	// iterate the items in reverse order
	for (long long i = items.size() - 1; i >= 0;)
	{
		std::vector<RangeItem> same_start_items = { items[i] };
		i--;
		while (i >= 0 && items[i].start == same_start_items[0].start)
		{
			same_start_items.push_back(items[i]);
			i--;
		}

		bool encountered_twice = false;
		for (const auto& item : same_start_items)
		{
			if (item.end > smallest_end)
				continue;


			if (item.end == smallest_end)
				encountered_twice = true;
			else
			{
				smallest_end = item.end;
				encountered_twice = false;
			}
		}


		for (const auto& item : same_start_items)
		{
			if (item.end > smallest_end ||
				(item.end == smallest_end && encountered_twice))
			{
				containing_items.insert(item.index);
			}
		}
	}

	// sort the items in place according to their end location
	std::sort(items.begin(), items.end(),
		[&](const RangeItem& left, const RangeItem& right)
		{
			return left.end < right.end;
		});
	ull smallest_start = ULONG_MAX;

	for (long long i = items.size() - 1; i >= 0;)
	{
		std::vector<RangeItem> same_end_items = { items[i] };
		i--;
		while (i >= 0 && items[i].end == same_end_items[0].end)
		{
			same_end_items.push_back(items[i]);
			i--;
		}

		bool encountered_twice = false;
		for (const auto& item : same_end_items)
		{
			if (item.start > smallest_start)
				continue;

			if (item.start == smallest_start)
				encountered_twice = true;
			else
			{
				smallest_start = item.start;
				encountered_twice = false;
			}
		}


		for (const auto& item : same_end_items)
		{
			if (item.start > smallest_start ||
				(item.start == smallest_start && encountered_twice))
			{
				contained_items.insert(item.index);
			}
		}
	}

	// print out containing ranges
	for (ul i = 0; i < test_cases; i++)
	{
		if (containing_items.find(i) != containing_items.end())
		{
			std::cout << 1 << " ";
		}
		else
		{
			std::cout << 0 << " ";
		}
	}

	std::cout << std::endl;

	// print out contained ranges
	for (ul i = 0; i < test_cases; i++)
	{
		if (contained_items.find(i) != contained_items.end())
		{
			std::cout << 1 << " ";
		}
		else
		{
			std::cout << 0 << " ";
		}
	}

	return 0;
}

int main()
{
	return ex2();
}