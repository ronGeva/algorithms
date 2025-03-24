#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>  // For std::stoi (C++11 and later)
#include <cstring>
#include <vector>
#include <algorithm>
#include <omp.h>
#include <chrono>
#include <math.h>

#define max(x, y) x > y ? x : y
#define min(x, y) x > y ? y : x

using std::vector;
using namespace std::chrono;
uint64_t* g_buffer1 = NULL;
uint64_t* g_buffer2 = NULL;

void pad_to_power_of_two(vector<uint64_t>& arr, uint64_t value)
{
	size_t size = arr.size();

	uint64_t power = (uint64_t)ceil(log2(size));
	size = (1LL << power);

	arr.resize(size, value);
}

void odd_even_merge(vector<uint64_t>& arr, size_t start, size_t step, size_t len)
{
	if (len == 1)
		return;

	#pragma omp task shared(arr)
	// odd
	odd_even_merge(arr, start + step, 2 * step, len / 2);

	#pragma omp task shared(arr)	
	// even
	odd_even_merge(arr, start, 2 * step, len / 2);

	#pragma omp taskwait

	// assume each half of the subarray is sorted, merge them
	#pragma omp parallel for shared(arr)
	for (size_t i = 0; i < len - 1; i++)
	{
		if (arr[start + i * step] > arr[start + (i + 1) * step])
		{
			std::swap(arr[start + i * step], arr[start + (i + 1) * step]);
		}
	}
}

void merge(vector<uint64_t>& arr, uint64_t l, uint64_t r)
{
	size_t len = r - l;
	memcpy(g_buffer1, &arr[l], (len / 2) * sizeof(uint64_t));
	memcpy(g_buffer2, &arr[l + len / 2], (len / 2) * sizeof(uint64_t));

	size_t i = 0, j = 0, index = 0;
	while (i + j < len)
	{
		if ((j == len / 2) ||
			((i < len / 2) && (g_buffer1[i] < g_buffer2[j])))
		{
			// choose left element
			arr[l + index] = g_buffer1[i];
			i++;
		}
		else
		{
			arr[l + index] = g_buffer2[j];
			j++;
		}

		index++;
	}
}

// sorts the interval arr[l:r] in place
void parallel_mergesort_internal(vector<uint64_t>& arr, uint64_t l, uint64_t r)
{
	if (r - l <= 1)
	{
		// a single element subarray, already sorted
		return;
	}

	const uint64_t mid = (l + r) / 2;

	// recursively call merge procedure for the two sublists
	// use OpenMP to speed things up
	#pragma omp task shared(arr)
	parallel_mergesort_internal(arr, l, mid);
	
	#pragma omp task shared(arr)
	parallel_mergesort_internal(arr, mid, r);

	#pragma omp taskwait

	//odd_even_merge(arr, l, 1, r - l);
	merge(arr, l, r);
}

void parallel_mergesort(vector<uint64_t>& arr, uint64_t& duration)
{
	size_t original_size = arr.size();

	// we need the size of the vector to be a power of 2
	pad_to_power_of_two(arr, (uint64_t)-1);

	// allocate temporary buffers to speed up code
	g_buffer1 = reinterpret_cast<uint64_t*>(malloc(sizeof(uint64_t) * arr.size()));
	g_buffer2 = reinterpret_cast<uint64_t*>(malloc(sizeof(uint64_t) * arr.size()));

	auto start = high_resolution_clock::now();
	parallel_mergesort_internal(arr, 0, arr.size());
	auto stop = high_resolution_clock::now();

	duration = duration_cast<microseconds>((stop - start)).count();

	arr.resize(original_size);
}

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		std::cout << "usage: <binary> <num of threads> <input file>" << std::endl;
		return 1;
	}

	int number_of_threads = atoi(argv[1]);
	char* file_path = argv[2];

	omp_set_num_threads(number_of_threads);

	std::ifstream input_file(file_path);
	if (!input_file)
	{
		std::cout << "failed to open file " << file_path << std::endl;
		return 1;
	}

	std::vector<uint64_t> numbers;
	std::string line;
	while (std::getline(input_file, line))
	{
		try {
			uint64_t number = std::stoull(line);
			numbers.push_back(number);
		}
		catch (const std::exception& e) {
			std::cerr << "Invalid integer in file: " << line << std::endl;
		}
	}

	uint64_t duration;
	parallel_mergesort(numbers, duration);

	std::cout << "merge sort: " << duration << std::endl;

	for (size_t i = 0; i < numbers.size(); i++)
	{
		std::cout << numbers[i] << std::endl;
	}

	return 0;
}