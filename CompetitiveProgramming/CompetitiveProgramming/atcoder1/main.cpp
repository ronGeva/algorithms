#include <iostream>
#include <string>
#include <vector>
#include <map>

#define ull unsigned long long
#define ul unsigned long

void task1()
{
	ull num;

	std::cin >> num;

	ul ones = 0, twos = 0, threes = 0;
	while (num > 0)
	{
		ul digit = num % 10;
		if (digit == 1)
			ones++;
		if (digit == 2)
			twos++;
		if (digit == 3)
			threes++;

		num /= 10;
	}

	if (ones == 1 && twos == 2 && threes == 3)
	{
		std::cout << "Yes" << std::endl;
	}
	else
	{
		std::cout << "No" << std::endl;
	}
}

void task2()
{
	std::string str;
	std::cin >> str;
	std::vector<ul> numbers;

	ul current_num = 0;
	for (ull i = 1; i < str.size(); i++)
	{
		if (str[i] == '|')
		{
			numbers.push_back(current_num);
			current_num = 0;
		}
		else
		{
			current_num++;
		}
	}

	for (ull i = 0; i < numbers.size(); i++)
	{
		std::cout << numbers[i] << " ";
	}
}

void task3()
{
	ul str_size, k;
	std::string str;
	std::cin >> str_size;
	std::cin >> k;
	std::cin >> str;

	// the start and end of 1 blocks
	std::vector<std::pair<ul, ul>> one_blocks;
	size_t end = 0;
	size_t start;
	while (one_blocks.size() < k)
	{
		start = str.find('1', end);
		end = str.find('0', start);
		if (end == str.npos)
		{
			end = str.size();
		}
		one_blocks.push_back({ start, end });
	}

	str.erase(one_blocks[k - 1].first, one_blocks[k - 1].second - one_blocks[k - 1].first);
	str.insert(one_blocks[k - 2].second, one_blocks[k - 1].second - one_blocks[k - 1].first,
			   (const char)'1');

	std::cout << str << std::endl;
}

struct Task5ColorBlock {
	Task5ColorBlock(ul s, ul e, ul c) {
		start = s;
		end = e;
		color = c;
	}

	ul start;
	ul end;
	ul color;
};

void task5()
{
	ul n, q;
	std::cin >> n;
	std::cin >> q;

	// key is block start index, value is color
	std::vector<Task5ColorBlock*> color_blocks;
	std::map<ul, ul> color_amount;
	std::vector<ul> output;
	for (ul i = 0; i < n; i++)
	{
		auto block = new Task5ColorBlock(i + 1, i + 2, i + 1);
		color_blocks.push_back(block);

		color_amount[i + 1] = 1;
	}

	for (ul i = 0; i < q; i++)
	{
		ul type;
		std::cin >> type;

		if (type == 2)
		{
			ul color;
			std::cin >> color;

			output.push_back(color_amount[color]);
		}
		else
		{
			ul cell_index;
			ul color;
			std::cin >> cell_index;
			std::cin >> color;

			Task5ColorBlock* block = color_blocks[cell_index - 1];
			// if it is the same color, do nothing
			if (color == block->color)
				continue;

			color_amount[block->color] -= (block->end - block->start);
			color_amount[color] += (block->end - block->start);

			block->color = color;
			// 1 based so we start at 1
			if (block->start > 1)
			{
				Task5ColorBlock* prev_block = color_blocks[block->start - 2];
				if (prev_block->color == block->color)
				{
					if (prev_block->end - prev_block->start > block->end - block->start)
					{
						// it is better to go through all the current block's elements, since
						// the current block is smaller
						prev_block->end = block->end;
						for (ul j = block->start; j < block->end; j++)
						{
							color_blocks[j - 1] = prev_block;
						}

						delete block;

						block = prev_block;
					}
					else
					{
						// it is better to go through all the previous block's elements, since
						// the current block is bigger
						block->start = prev_block->start;
						for (ul j = prev_block->start; j < prev_block->end; j++)
						{
							color_blocks[j - 1] = block;
						}
						delete prev_block;
					}
				}
			}

			if (block->end != n + 1)
			{
				Task5ColorBlock* next_block = color_blocks[block->end - 1];
				if (next_block->color == block->color)
				{
					if (next_block->end - next_block->start > block->end - block->start)
					{
						// next block is bigger, change elements in this block
						next_block->start = block->start;
						for (ul j = block->start; j < block->end; j++)
						{
							color_blocks[j - 1] = next_block;
						}

						delete block;
					}
					else
					{
						// this block is bigger, change elements in next block
						block->end = next_block->end;
						for (ul j = next_block->start; j < next_block->end; j++)
						{
							color_blocks[j - 1] = block;
						}

						delete next_block;
					}
				}
			}
		}
	}

	for (ul i = 0; i < output.size(); i++)
	{
		std::cout << output[i] << std::endl;
	}
}

int main()
{
	task5();
	return 0;
}