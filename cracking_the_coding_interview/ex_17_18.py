"""
This solution is very similar to the first solution in the actual book, which is sub-optimal.
The current solution requires: O(N * M) time+space complexity (where N is the size of the big array and M is the size
of the searched items list).
"""


def build_next_character_array(item, arr):
    """
    Constructs an array in which the value at index i represents the first index bigger/equal to i in which the
    item can be found.
    :param item: The item to which the returned array references.
    :param arr: An array of of values, some of which may be item.
    :return: An array.
    """
    next_index = None
    result_arr = [None for _ in range(len(arr))]
    for i in range(len(arr) - 1, -1, -1):
        if arr[i] == item:
            next_index = i
        result_arr[i] = next_index
    return result_arr


def find_shortest_subset(arr: list, searched_items: set, next_occurence_dict: dict):
    min_found = (0, len(arr))
    for i in range(0, len(arr) - 1):
        if arr[i] in searched_items:
            other_items = {item for item in searched_items if item != arr[i]}
            max_others_index = 0
            for item in other_items:
                item_next_occurrence = next_occurence_dict[item][i]
                if item_next_occurrence is None:
                    max_others_index = len(arr) * 2  # make sure this subset will be treated as "too big"
                    break  # we didn't found a matching subset here
                max_others_index = max(max_others_index, item_next_occurrence)
            current_range = (i, max_others_index)
            if max_others_index - i < min_found[1] - min_found[0]:
                min_found = current_range
    return min_found


def main():
    arr = [0, 4, 3, 2, 6, 2, 6, 1, 3, 2, 0, 1, 1, 1, 2, 2, 4]
    searched_items = {1, 2, 3, 4}
    next_occurence_dict = {}
    for item in searched_items:
        next_occurence_dict[item] = build_next_character_array(item, arr)

    min_found = find_shortest_subset(arr, searched_items, next_occurence_dict)
    print(min_found)


if __name__ == '__main__':
    main()
