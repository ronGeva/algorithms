"""
Implement a "shuffle" method which shuffles a deck of cards and re-orders them in a random order.
Each of the different outcomes most have an equal chance of being chosen.
"""
from random import randint  # we're allowed to use a "perfect" random number generator


def shuffle(shuffled_list: list[int], size: int):
    """
    Receive a list of size + 1 (at least) elements, from which the first size elements are already randomly shuffled.
    This function uses the random number generator to choose a random item to swap with the size+1-th element in the
    list, thus turning the list into a list of size+1 shuffled items.
    Complexity: O(1)  (if we ignore the random number generator)
    """
    random_index = randint(0, size - 1)
    temp = shuffled_list[random_index]
    shuffled_list[random_index] = shuffled_list[size]
    shuffled_list[size] = temp


def main():
    """
    Create a list of 52 items (we choose to assign them the values 0 to 51), then shuffle them by first shuffling a 1
    item list, then 2 item list, and so on and so forth until the entire list is shuffled.
    Each shuffle is only O(1) complexity, which means that for n elements the complexity of this function is O(n).
    """
    n = 52
    cards = [i for i in range(n)]
    for i in range(1, n):
        shuffle(cards, i)
    print(cards)


if __name__ == '__main__':
    main()
