#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unordered_set>
#include <queue>

#define ull unsigned long long
#define ul unsigned long
#define max(x, y) x > y ? x : y

bool ex1Check(const std::string& str)
{
	if (str.size() % 2 == 0)
		return false;

	if (str[(str.size() / 2)] != '/')
		return false;

	for (ul i = 0; i < str.size() / 2; i++)
	{
		if (str[i] != '1')
			return false;
	}

	for (ul i = str.size() / 2 + 1; i < str.size(); i++)
	{
		if (str[i] != '2')
			return false;
	}

	return true;
}

void ex1()
{
	ul n;
	std::cin >> n;
	std::string str;
	std::cin >> str;

	if (ex1Check(str))
		std::cout << "Yes" << std::endl;
	else
	{
		std::cout << "No" << std::endl;
	}
}

bool ex2Check(const std::string& str)
{
	if (str.size() % 2 != 0)
	{
		return false;
	}

	std::unordered_set<ul> numbers;
	for (long long i = 0; i < str.size() / 2; i++)
	{
		if (str[i * 2] != str[i * 2 + 1])
			return false;

		if (numbers.find(str[i * 2]) != numbers.end())
			return false;

		numbers.insert(str[i * 2]);
	}

	return true;
}

void ex2()
{
	std::string str;
	std::cin >> str;
	if (ex2Check(str))
	{
		std::cout << "Yes";
	}
	else
	{
		std::cout << "No";
	}
}

ul ex4LongestSubarray(long long start, long long end, std::vector<ul> arr)
{
	ull biggest_subsequence = 0;
	std::unordered_set<ul> current_numbers;
	std::queue<ul> numbers_seen;

	for (long long i = start; i + 1 <= end; i += 2)
	{
		ul current_num = arr[i];
		if (current_num == arr[i + 1] &&
			current_numbers.find(current_num) == current_numbers.end())
		{
			current_numbers.insert(current_num);
			numbers_seen.push(current_num);
		}
		else
		{
			if (current_numbers.size() > biggest_subsequence)
			{
				biggest_subsequence = current_numbers.size();
			}

			if (current_num == arr[i + 1])
			{
				while (numbers_seen.front() != current_num)
				{
					current_numbers.erase(numbers_seen.front());
					numbers_seen.pop();
				}
				current_numbers.erase(numbers_seen.front());
				numbers_seen.pop();

				current_numbers.insert(current_num);
				numbers_seen.push(current_num);
			}
			else
			{
				current_numbers.clear();
				numbers_seen = std::queue<ul>();
			}
		}
	}
	if (current_numbers.size() > biggest_subsequence)
	{
		biggest_subsequence = current_numbers.size();
	}

	return biggest_subsequence * 2;
}

void ex4()
{
	long long n;
	std::cin >> n;
	std::vector<ul> arr;
	for (ul i = 0; i < n; i++)
	{
		ul num;
		std::cin >> num;
		arr.push_back(num);
	}

	ull biggest_subsequence = 0;
	std::unordered_set<ul> current_numbers;

	// even offsets
	ul start = 0;
	long long end = n % 2 == 0 ? n - 1: n - 2;
	biggest_subsequence = max(biggest_subsequence, ex4LongestSubarray(start, end, arr));

	// uneven offsets
	start = 1;
	end = n % 2 == 0 ? n - 2 : n - 1;
	biggest_subsequence = max(biggest_subsequence, ex4LongestSubarray(start, end, arr));

	std::cout << biggest_subsequence << std::endl;
}

struct CharBlock {
	ul start;
	ul end;
	char value;
};

void ex5()
{
	ul n;
	ul q;
	std::string str;

	std::cin >> n;
	std::cin >> q;
	std::cin >> str;

	char current_char = str[0];
	ul current_start = 0;
	std::vector<CharBlock> blocks;
	ul i;
	for (i = 0; i < n; i++)
	{
		if (str[i] == current_char)
			continue;
		
		CharBlock block;
		block.start = current_start;
		block.end = i;
		block.value = current_char;

		for (ul j = current_start; j < i; j++)
		{
			blocks.push_back(block);
		}

		current_char = str[i];
		current_start = i;
	}

	CharBlock block;
	block.start = current_start;
	block.end = i;
	block.value = current_char;
	for (ul j = current_start; j < i; j++)
	{
		blocks.push_back(block);
	}

	for (ul i = 0; i < q; i++)
	{
		long long l;
		long long r;
		std::cin >> l;
		std::cin >> r;
	}
}

int main()
{
	ex4();
	return 0;
}