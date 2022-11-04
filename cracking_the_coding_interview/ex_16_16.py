"""
Given an array of integers, find the indexes m,n such that if we sort arr[m:n] the entire arrary will be sorted.
Minimize (n-m) as much as possible.
"""


def find_first_element_not_in_order(arr: list[int]):
    for i in range(len(arr) - 1):
        if arr[i] > arr[i + 1]:
            return i
    return None


def find_last_element_not_in_order(arr: list[int]):
    for i in range(len(arr) - 2, -1, -1):
        if arr[i] > arr[i + 1]:
            return i
    return None


def get_index_of_first_biggest_integer(arr: list[int], value: int):
    for i in range(len(arr)):
        if arr[i] > value:
            return i
    return None


def main():
    arr = [1, 2, 4, 6, 7, 3, 5, 9, 10, 13, 8, 15]
    first, last = find_first_element_not_in_order(arr), find_last_element_not_in_order(arr)
    m = get_index_of_first_biggest_integer(arr, first)
    n = last
    print("m, n = {0}, {1}".format(m, n))


if __name__ == '__main__':
    main()
