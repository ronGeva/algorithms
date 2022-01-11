"""
In this problem we receive a list of words and must find the biggest word that consists of other words within it (aka
the entire word is fully consisting of other words from the list, concatenated to each other - for example:
"dog", "cat", "dogcat".

The solution I came up with is going through each word and checking for each offset inside the word whether or not
word[offset:] is made out of other words from the list.
We memoize this information for every offset we try and fail to find a combination of words so that word[offset:]
 consists of them. Then when we encounter this offset in the word for another combination of previous words we don't
 need to go through the rest of the word in order to check whether the current combination is valid, since we know it
 isn't.

This solution has O(n * k) time complexity when n denotes the amount of words and k the maximal size of a word, and
O(n * k) space complexity (since we save an additional set of all words).
"""


def is_made_of_other_words(word: str, start_offset: int, words_set : set[str], checked_offsets: set):
    """
    Check whether word[start_offset:] can be built using words from words_set.
    :param word: The full word from which we derive the word we're performing this operation on.
    :param start_offset: The offset from which we'll examine word (aka word[start_offset:]).
    :param words_set: A set containing all valid words. Will be used to test for every substring of word[start_offset:]
    whether it is a word from the list given to us.
    :param checked_offsets: Every offset in check_offset signals that word[offset:] cannot be built using words from
    words_set. This param is used for memoization.
    :return: Whether word can be built using 2 or more words from words_set.
    """
    end_offset = start_offset + 1
    current_word = word[start_offset: end_offset]
    while end_offset < len(word):
        if current_word in words_set and end_offset not in checked_offsets and\
                is_made_of_other_words(word, end_offset, words_set, checked_offsets):
            return True
        end_offset += 1
        current_word = word[start_offset: end_offset]

    # Second part of the condition is to make sure we found at least 2 words this word is made from
    res = current_word in words_set and start_offset > 0
    if res is False:
        checked_offsets.add(start_offset)  # memoize this offset yields no valid results
    return res


def main():
    # For testing purposes we read the entire english Alphabet.
    with open('ex_17_15_input.txt', 'r') as f:
        data = f.read()
    words = data.splitlines()
    words_set = set(words)

    biggest_length = 0
    biggest_word = None
    for word in words:
        if len(word) > biggest_length and is_made_of_other_words(word, 0, words_set, set()):
            biggest_length = len(word)
            biggest_word = word
    print(biggest_word)


if __name__ == '__main__':
    main()
