"""
Given an array of N integers, find the majority element - that is, the element that appears as more than half of the
items in the array.
Required time complexity: O(n).
Required space complexity: O(1).

Solution:
If more than half of the elements are the majority number, we can conclude it is the median of the array.
The known "Select" algorithm allows us to find the median of an array in O(n) time and O(1) space.

NOTE: this solution is different from the one in the book.
"""
import random
from ex17_2 import shuffle


def generate_array():
    majority_num = 10  # The actual majority number doesn't affect the algorithm, so we can pre-determine this
    array_size = random.randint(10, 100)
    amount_of_majority_num = random.randint(array_size // 2 + 1, array_size)
    arr = [random.randint(1, 9) for _ in range(array_size - amount_of_majority_num)] + \
          [majority_num for _ in range(amount_of_majority_num)]
    shuffle(arr)
    return arr


def swap(arr: list[int], i: int, j: int):
    """
    Swaps the values of the i-th and j-th elements in arr.
    """
    temp = arr[i]
    arr[i] = arr[j]
    arr[j] = temp


def partition(arr: list[int], left: int, right: int, pivot_index: int):
    """
    This function only works on the sub-array arr[left:right].
    Let the pivot be the element at pivot_index.
    This function moves all elements smaller than the pivot to the left of the sub-array, and all elements greater
    than pivot to the right of the pivot.
    :return The new index of the pivot (which is located in the middle of all elements smaller than it and all elements
    larger than it).
    """
    pivot_value = arr[pivot_index]
    swap(arr, right, pivot_index)
    store_index = left
    for i in range(left, right):
        if arr[i] > pivot_value:
            swap(arr, i, store_index)
            store_index += 1
    swap(arr, right, store_index)
    return store_index


def select(arr: list[int], left: int, right: int, k: int):
    """
    Returns the k-th element in O(n) time and O(1) space.
    """
    while True:
        if left == right:
            return arr[left]
        pivot_index = left + (right - left) // 2
        pivot_index = partition(arr, left, right, pivot_index)
        if k == pivot_index:
            return arr[k]
        elif k < pivot_index:
            return select(arr, left, pivot_index - 1, k)
        else:
            return select(arr, pivot_index + 1, right, k)


def main():
    for i in range(10000):
        arr = generate_array()
        assert select(arr, 0, len(arr) - 1, len(arr) // 2) == 10
        x = 5


if __name__ == '__main__':
    main()
