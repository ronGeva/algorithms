"""
Naive implementation of the Lyndon Array computation for a given string.
Runtime of this implementation is an horrendous O(n^4), although there exist a solution with O(n) runtime.
The purpose of this solution is to grasp the basics of lyndon array computation.
"""
import sys


def is_lyndon_word(word: str):
    """
    runtime: let n = len(word), runtime is O(n^2).
    """
    return all([word < word[i:] for i in range(1, len(word))])


def calculate_lyndon_value(index: int, st: str):
    """
    runtime: let n=len(st), runtime is O(n^3).
    """
    for possible_size in range(len(st) - index, 1, -1):
        possible_lyndon_word = st[index: index + possible_size]
        if is_lyndon_word(possible_lyndon_word):
            return possible_size
    return 1


def main(args):
    """
    runtime: let n=len(args[1]), runtime is O(n^4).
    :param args:
    :return:
    """
    if len(args) != 2:
        print("Usage: naive_lyndon_array.py <string>")
        return
    st = args[1]
    arr = []
    for i in range(len(st)):
        arr.append(calculate_lyndon_value(i, st))
    print(arr)


if __name__ == '__main__':
    main(sys.argv)
