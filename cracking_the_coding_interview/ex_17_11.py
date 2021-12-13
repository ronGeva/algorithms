"""
Problem: Given a text file containing many words, and 2 words, find the shortest "path" between these two words in the
file (path == the amount of words between the two words).

Solution:
First we'll create a map mapping the words into their locations in the file (str -> list[int]).
Then, for every 2 words we'll retrieve their locations and iterate through them, each time incrementing the smaller
location.

Explaining the location iteration logic:
Given 2 locations A, B, if A > B we can rest assure all locations prior to B won't yield a better result than
B compared to A (and all location following it).
In addition, we can rest assure all locations posterior to A won't yield a better result than A compared to B (and all
 locations before it).
Therefore, for each 2 locations, we know there's no point in comparing all number below the smaller location and all
number above the bigger location, and can therefore increment the smaller location and continue our algorithm.
"""


def read_words(file_path: str) -> list[str]:
    with open(file_path, 'r') as f:
        data = f.read()
    return data.split("\n")[:-1]  # remove last element which will be an empty string


def find_shortest_path(first_word: str, second_word: str, words_locations: dict[str, list[int]]):
    first_locations = words_locations[first_word]
    second_locations = words_locations[second_word]
    min_path = abs(first_locations[0] - second_locations[0])
    first_index = 0
    second_index = 0
    while True:
        current_path = abs(first_locations[first_index] - second_locations[second_index])
        if current_path < min_path:
            min_path = current_path
        if first_locations[first_index] < second_locations[second_index] and first_index < len(first_locations) - 1:
            first_index += 1
        elif second_index < len(second_locations) - 1:
            second_index += 1
        else:
            break  # we're done
    return min_path


def naive_shortest_path(first_word: str, second_word: str, words_locations: dict[str, list[int]]):
    first_locations = words_locations[first_word]
    second_locations = words_locations[second_word]
    min_path = abs(first_locations[0] - second_locations[0])
    for location in first_locations:
        for location_2 in second_locations:
            if abs(location - location_2) < min_path:
                min_path = abs(location - location_2)
    return min_path


def test_solution(words_locations: dict[str, list[int]]):
    from random import randint
    words = list(words_locations.keys())
    for _ in range(10000):
        first_word = words[randint(0, len(words) - 1)]
        second_word = words[randint(0, len(words) - 1)]
        naive_answer = naive_shortest_path(first_word, second_word, words_locations)
        fast_answer = find_shortest_path(first_word, second_word, words_locations)
        assert naive_answer == fast_answer, "Failed for words: {0}, {1}".format(first_word, second_word)


def main():
    words = read_words('ex_17_11_input.txt')
    words_locations = {}
    for words_index in range(len(words)):
        words_locations.setdefault(words[words_index], []).append(words_index)
    test_solution(words_locations)
    pass


if __name__ == '__main__':
    main()
