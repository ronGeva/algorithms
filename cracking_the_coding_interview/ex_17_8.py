"""
Problem: Given a list of people's heights and weights (given in the form of a 2-tuple), find the longest list of
people that can stand on top of each other, with the following restriction: each person can only stand on a person
that is both taller and weighs more than him.

Solution: First, we'll sort the list according to the height (the first value in each item). Then, we'll iterate the
items from the end to the start and for each item we'll found the biggest group to which it can join.
For example, given the values: (13, 56), (20, 80), (33, 100), at the first iteration, since no towers were found until
now we'll define [(33, 100)] as the only found tower, in the next iteration we'll check whether (20, 80) fits into any
of the current towers - it fits at the start of (33, 100) and therefore can go there.
This process goes on until we've found all possible towers, at this point we simply choose the longest one.

For each item, we go through all previous found towers (which is the same as going through all previously visited
items), therefore this algorithm operates in O(N^2) complexity.
We create a "Tower" node for each item, each taking O(1) space. Therefore, this algorithm also takes O(N) space.

The rational behind this algorithm is that for every tower we've already found, we can simply check if the current
item fits at its start, if not, either it does not fit at the start of any of the tower's potential sub-towers, or it
will fit at the start of another tower (since we'll iterate through them all either way).
"""


class Tower(object):
    """
    A class representing the start of a tower.
    The values of the tower are the values of the first element in it, and the next member points to the next item in
    the tower (an instance of Tower, or None).
    """
    def __init__(self, values: (int, int), next):
        self.values = values
        self.next = next  # type: Tower
        if self.next is None:
            if self.values is None:
                self.length = 0
            else:
                self.length = 1
        else:
            self.length = self.next.length + 1


def find_biggest_tower(items: list[(int, int)]):
    list_height = sorted(items, key=lambda item: item[0])

    possible_towers = []
    biggest_group_found = Tower(None, None)
    for item in list_height[::-1]:
        biggest_matching_group = Tower(None, None)
        for tower in possible_towers:
            if item[0] < tower.values[0] and item[1] < tower.values[1] and tower.length > biggest_matching_group.length:
                biggest_matching_group = tower

        if biggest_matching_group.values is not None:
            new_tower = Tower(item, biggest_matching_group)
            possible_towers.append(new_tower)
        else:
            new_tower = Tower(item, None)
            possible_towers.append(new_tower)

        if new_tower.length > biggest_group_found.length:
            biggest_group_found = new_tower

    return biggest_group_found


"""
TESTS
"""
from random import randint


def validate_tower(tower: list[(int, int)]):
    for i in range(len(tower) - 1):
        if not (tower[i][0] < tower[i + 1][0] and tower[i][1] < tower[i + 1][1]):
            return False
    return True


def naive_solutions(items: list[(int, int)]):
    items = sorted(items, key=lambda item: item[0])  # sort by height
    biggest_groups_found = [[]]
    possibilities = pow(2, len(items))
    for possibility in range(possibilities):
        current_tower = []
        for i in range(len(items)):
            if possibility & (1 << i):
                current_tower.append(items[i])
        if validate_tower(current_tower):
            if len(current_tower) > len(biggest_groups_found[0]):
                biggest_groups_found = [current_tower]
            elif len(current_tower) == len(biggest_groups_found[0]):
                biggest_groups_found.append(current_tower)

    return biggest_groups_found


def generate_items(size: int):
    return [(randint(0, 100), randint(0, 100)) for _ in range(size)]


def is_found_group_in_naive_results(found_group: Tower, naive_results: list[list[(int, int)]]):
    if found_group.length != len(naive_results[0]):
        return False

    for result in naive_results:
        found = True
        current_item = found_group
        for i in range(len(result)):
            if current_item.values != result[i]:
                found = False
                break
            current_item = current_item.next

        if found:
            return True
    return False


def items_str(item: Tower):
    description = ""
    while item is not None:
        description += "{} -> ".format(item.values)
        item = item.next
    return description


def test_algorithm(number_of_tests: int):
    for _ in range(number_of_tests):
        items = generate_items(10)
        biggest_group_found = find_biggest_tower(items)
        actual_biggest_groups = naive_solutions(items)
        assert is_found_group_in_naive_results(biggest_group_found, actual_biggest_groups),\
            "items: {0}\r\n {1} is not in \r\n {2}".format(items, items_str(biggest_group_found), actual_biggest_groups)


def main():
    test_algorithm(10000)


if __name__ == '__main__':
    main()
