"""
In this problem we are tasked with finding two missing numbers from a list of numbers that originally contained all
number from 0 to n, but is now missing 2 of them.

The solution for 1 number is simple, xor all elements then xor the result with the expected result (the result which
we would've gotten in case the number wasn't removed).
For 2 numbers we do something similar but instead of xor use addition, then we calculate the difference between the sum
of the items before the removal and the sum now.

The difference is the sum of the 2 numbers removed. We now need to run the first algorithm (for 1 item) for all the
items in the list that are smaller than the half of the difference (since exactly one of the numbers will be smaller than
half of their sum).
The number we get when we run this algorithm is one of the numbers missing, which we can use to immediately figure out
the second number (since we have their sum).
"""


import random


def find_missing_two(l: list[int]):
    expected_sum = sum(range(len(l) + 2))
    actual_sum = sum(l)
    sum_diff = expected_sum - actual_sum
    half_diff = sum_diff // 2

    expected_xor = 0
    for i in range(half_diff + 1):
        expected_xor ^= i
    actual_xor = 0
    for item in l:
        if item <= half_diff:
            actual_xor ^= item
    first_missing_item = expected_xor ^ actual_xor
    second_missing_item = sum_diff - first_missing_item
    return first_missing_item, second_missing_item


def find_missing_one(l: list[int]):
    expected_xor = 0
    for i in range(len(l) + 1):
        expected_xor ^= i

    actual_xor = 0
    for item in l:
        actual_xor ^= item

    return actual_xor ^ expected_xor


def test_solution():
    for _ in range(20000):
        list_size = random.randint(10, 200)
        l = [i for i in range(list_size)]
        first_item = random.choice(l)
        l.remove(first_item)
        second_item = random.choice(l)
        l.remove(second_item)
        assert set(find_missing_two(l)) == {first_item, second_item}, "{0}, {1}, {2}".format(list_size, first_item,
                                                                                             second_item)


def main():
    test_solution()


if __name__ == '__main__':
    main()
