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

// ******************** EX3 ********************
int ex3()
{
	const ull modulo = 998244353;
	ul card_amount;
	std::cin >> card_amount;

	std::vector<std::pair<ull, ull>> cards;

	// possibilities[i][j] is a pair (a, b) which represents the amount of different 
	// valid combinations of the subset of cards from i onwards.
	// if the i-1 card is flipped then the possibilities are a, otherwise b.
	std::vector<std::pair<ull, ull>> possibilities;
	for (ul i = 0; i < card_amount; i++)
	{
		ull first, second;
		std::cin >> first;
		std::cin >> second;

		cards.push_back({ first, second });
		possibilities.push_back({ 0, 0 });
	}

	possibilities[card_amount - 1] = { 1, 1 };

	for (long long i = cards.size() - 2; i >= 0; i--)
	{
		// first option
		ull value = cards[i].first;
		if (value != cards[i + 1].first)
			possibilities[i].first += possibilities[i + 1].first;

		if (value != cards[i + 1].second)
			possibilities[i].first += possibilities[i + 1].second;

		possibilities[i].first = possibilities[i].first % modulo;

		// second option
		value = cards[i].second;
		if (value != cards[i + 1].first)
			possibilities[i].second += possibilities[i + 1].first;

		if (value != cards[i + 1].second)
			possibilities[i].second += possibilities[i + 1].second;

		possibilities[i].second = possibilities[i].second % modulo;
	}

	ull result = (possibilities[0].first + possibilities[0].second) % modulo;

	std::cout << result << std::endl;
	return 0;
}

int ex4()
{
	ul test_cases;
	std::cin >> test_cases;

	std::vector<std::vector<ull>> results;
	for (ul i = 0; i < test_cases; i++)
	{
		ul array_size;
		std::cin >> array_size;

		std::vector<ull> current_array_results;
		ul odd_amount = 0, even_amount = 0;
		ull sum = 0;
		for (ul j = 0; j < array_size; j++)
		{
			ull value;
			std::cin >> value;

			sum += value;
			if (value % 2 == 0)
				even_amount++;
			else
				odd_amount++;

			if (j == 0)
			{
				current_array_results.push_back(sum);
				continue;
			}

			// If there are only even numbers then no odd-even pairs will ever be chosen.
			// If there is at least 1 even number, choosing even number will always lead to
			// us eventually having only 1 even number, at which point if we choose the even
			// number we always match an even-odd pair.
			//
			// Therefore, player1 will always choose 2 odds (unless there are no odds in which case
			// the "game" ends immediately).
			ull odd_even_pairs_chosen = odd_amount / 3;
			if (odd_amount % 3 == 1)
				odd_even_pairs_chosen++;

			current_array_results.push_back(sum - odd_even_pairs_chosen);
		}
		results.push_back(std::move(current_array_results));
	}

	for (const auto& result : results)
	{
		for (const auto& sum : result)
		{
			std::cout << sum << " ";
		}
		std::cout << std::endl;
	}
	return 0;
}

ul maxInCounters(ul counters[256])
{
	ul biggest = 0;
	for (ul i = 0; i < 256; i++)
	{
		biggest = max(biggest, counters[i]);
	}

	return biggest;
}

ul alternatingStringEvenOddSumMax(const std::string& str)
{
	ul evenCounters[256] = { 0 };
	ul oddCounters[256] = { 0 };

	for (ul i = 0; i < str.size() - 1; i++)
	{
		if (i % 2 == 0)
			evenCounters[str[i]]++;
		else
			oddCounters[str[i]]++;
	}

	if (str.size() % 2 == 0)
	{
		oddCounters[str[str.size() - 1]]++;
		return maxInCounters(evenCounters) + maxInCounters(oddCounters);
	}

	ul maxEven = maxInCounters(evenCounters);
	ul maxOdd = maxInCounters(oddCounters);

	ul evenOddSumMax = maxEven + maxOdd;

	// assume str.size() is odd
	for (long long i = str.size() - 2; i >= 0; i--)
	{
		// evenCounters and oddCounters contain the data relevant if
		// the (i+1)th element was removed.
		// When we look at the i-th case, all the elements except for
		// str[i], str[i+1] are unchanged - they remain even/odd and with the
		// same value.
		// We only need to treat those two characters.
		ul* counters = nullptr;
		if (i % 2 == 0)
			counters = evenCounters;
		else
			counters = oddCounters;

		counters[str[i + 1]]++;
		counters[str[i]]--;

		maxEven = maxInCounters(evenCounters);
		maxOdd = maxInCounters(oddCounters);
		evenOddSumMax = max((maxEven + maxOdd), evenOddSumMax);
	}

	return evenOddSumMax;
}

ul MinimumChangesForAlternatingString(const std::string& str)
{
	if (str.size() == 1)
		return 1;

	ul evenOddSumMax = alternatingStringEvenOddSumMax(str);

	// note that if the size of the string is odd then we've alrady accounted
	// for the deletion procedure since we've performed the even-odd-max calculation
	// on an even string that is smaller by 1, therefore when we use str.size() the
	// deletion procedure is taken into account.
	return (ul)str.size() - evenOddSumMax;
}

int ex5()
{
	ul test_cases;
	std::cin >> test_cases;
	std::vector<ul> results;

	for (ul i = 0; i < test_cases; i++)
	{
		ul str_size;
		std::cin >> str_size;

		std::string str;
		std::cin >> str;

		ul result = MinimumChangesForAlternatingString(str);
		results.push_back(result);
	}

	for (ul i = 0; i < results.size(); i++)
	{
		std::cout << results[i] << std::endl;
	}
	return 0;
}

int main()
{
	return ex5();
}