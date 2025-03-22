#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>  // For std::stoi (C++11 and later)
#include <vector>
#include <algorithm>
#include <omp.h>
#include <chrono>
#include <math.h>

#define max(x, y) x > y ? x : y
#define min(x, y) x > y ? y : x

using std::vector;
using namespace std::chrono;

void* g_buffers = NULL;

void preallocate_buffers(size_t biggest_power)
{
	const size_t n = (1LL << biggest_power);

	g_buffers = malloc(sizeof(void*) * (biggest_power + 1));

	for (size_t power = 0; power <= biggest_power; power++)
	{
		// allocate n/2^i vectors of size 2^i
		uint64_t maximum_parallel_buffers = (n >> power) * 32;
		void* buffer = malloc(sizeof(void*) * maximum_parallel_buffers);
		for (size_t j = 0; j < maximum_parallel_buffers; j++)
		{
			reinterpret_cast<vector<uint64_t>**>(buffer)[j] =
				new vector<uint64_t>(1LL << power);
		}

		reinterpret_cast<void**>(g_buffers)[power] = buffer;
	}
}

vector<uint64_t>& get_preallocated_buffer(size_t power)
{
	static uint64_t indexes[64] = { 0 };

	uint64_t index;
	#pragma omp critical
	{
		index = indexes[power]++;
	}

	return *reinterpret_cast<vector<uint64_t>***>(g_buffers)[power][index];
}

vector<uint64_t>* g_buffer1;
vector<uint64_t>* g_buffer2;

void pad_to_power_of_two(vector<uint64_t>& arr, uint64_t value)
{
	size_t size = arr.size();

	uint64_t power = (uint64_t)ceil(log2(size));
	size = (1LL << power);

	arr.resize(size, value);
}

void odd_even_merge(vector<uint64_t>& left, vector<uint64_t>& right, vector<uint64_t>& merged_result)
{
	size_t sublist_size = left.size();
	if (sublist_size == 1)
	{
		merged_result[0] = min(left[0], right[0]);
		merged_result[1] = max(left[0], right[0]);
		return;
	}

	size_t power = (size_t)log2(sublist_size / 2);
	vector<uint64_t>& left_even_copy = get_preallocated_buffer(power);
	vector<uint64_t>& left_odd_copy = get_preallocated_buffer(power);
	vector<uint64_t>& right_even_copy = get_preallocated_buffer(power);
	vector<uint64_t>& right_odd_copy = get_preallocated_buffer(power);

	#pragma omp parallel for
	for (size_t i = 0; i < sublist_size / 2; i++)
	{
		left_even_copy[i] = left[2 * i];
		left_odd_copy[i] = left[2 * i + 1];

		right_even_copy[i] = right[2 * i];
		right_odd_copy[i] = right[2 * i + 1];
	}

	vector<uint64_t>& c = get_preallocated_buffer(power + 1);
	vector<uint64_t>& d = get_preallocated_buffer(power + 1);
	odd_even_merge(left_even_copy, right_even_copy, c);
	odd_even_merge(left_odd_copy, right_odd_copy, d);

	// interleave c and d
	#pragma omp parallel for
	for (size_t i = 0; i < sublist_size; i++)
	{
		merged_result[2 * i] = c[i];
		merged_result[2 * i + 1] = d[i];
	}

	// swap neighbors if their order is reversed, this is thread safe
	#pragma omp parallel for
	for (size_t i = 0; i < sublist_size * 2 - 1; i++)
	{
		if (merged_result[i] <= merged_result[i + 1])
			continue;

		// swap
		uint64_t tmp = merged_result[i];
		merged_result[i] = merged_result[i + 1];
		merged_result[i + 1] = tmp;
	}
}

// sorts the interval arr[l:r] in place
void odd_even_mergesort_internal(vector<uint64_t>& arr, uint64_t l, uint64_t r)
{
	if (r - l == 2)
	{
		uint64_t min_val = min(arr[l], arr[l + 1]);
		uint64_t max_val = max(arr[l], arr[l + 1]);
		arr[l] = min_val;
		arr[l + 1] = max_val;
		return;
	}

	const uint64_t mid = (l + r) / 2;

	// recursively call merge procedure for the two sublists
	// use OpenMP to speed things up
	#pragma omp parallel
	{
		#pragma omp single nowait
		{
			#pragma omp task
			odd_even_mergesort_internal(arr, l, mid);

			#pragma omp task
			odd_even_mergesort_internal(arr, mid, r);

			#pragma omp taskwait
		}
	}
	
	// merge the two lists
	uint64_t i = l, j = mid, k = 0;
	uint64_t sublist_size = (r - l) / 2;

	// use pre-allocated buffers to hold copies of the 2 sorted sublists
	// copy them using parallelism
	size_t power = (size_t)log2(sublist_size);
	/*vector<uint64_t> left_copy(sublist_size);
	vector<uint64_t> right_copy(sublist_size);*/
	vector<uint64_t>& left_copy = get_preallocated_buffer(power);
	vector<uint64_t>& right_copy = get_preallocated_buffer(power);
	
	#pragma omp parallel for
	for (size_t i = 0; i < sublist_size; i++)
	{
		left_copy[i] = arr[l + i];
		right_copy[i] = arr[mid + i];
	}

	vector<uint64_t>& merged_result = get_preallocated_buffer(power + 1);
	odd_even_merge(left_copy, right_copy, merged_result);
	#pragma omp parallel for
	for (size_t i = 0; i < sublist_size * 2; i++)
	{
		arr[l + i] = merged_result[i];
	}
	return;

	// merge the two copies back into arr
	size_t a = 0, b = 0;
	for (size_t i = 0; i < sublist_size * 2; i++)
	{
		if (b == sublist_size ||
			(a < sublist_size && left_copy[a] < right_copy[b]))
		{
			arr[l + i] = left_copy[a];
			a++;
		}
		else
		{
			arr[l + i] = right_copy[b];
			b++;
		}
	}
}

void parallel_mergesort(vector<uint64_t>& arr, uint64_t& duration)
{
	size_t original_size = arr.size();

	// we need the size of the vector to be a power of 2
	pad_to_power_of_two(arr, (uint64_t)-1);

	size_t biggest_power = (size_t)log2(arr.size()) + 1;
	preallocate_buffers(biggest_power);

	auto start = high_resolution_clock::now();
	odd_even_mergesort_internal(arr, 0, arr.size());
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