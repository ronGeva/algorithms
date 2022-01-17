from typing import Optional


class StringSearchNode(object):
    def __init__(self, character: Optional[str]):
        self.character = character
        self.next_characters = {}  # type: dict[str, StringSearchNode]
        self.is_word = False


def build_search_tree(small_strings: list[str]):
    root = StringSearchNode(None)
    for string in small_strings:
        current_node = root
        for character in string:
            if character in current_node.next_characters:
                current_node = current_node.next_characters[character]
            else:
                new_node = StringSearchNode(character)
                current_node.next_characters[character] = new_node
                current_node = new_node
        current_node.is_word = True
    return root


def search_index(big_string: str, search_tree: StringSearchNode, index: int):
    current_node = search_tree
    current_string = ""
    results = []
    while index < len(big_string):
        new_character = big_string[index]
        if new_character in current_node.next_characters:
            current_node = current_node.next_characters[new_character]
            current_string += big_string[index]
            if current_node.is_word:
                results.append(current_string)
        else:
            break
        index += 1
    return results


def search(big_string: str, search_tree: StringSearchNode):
    results = {}
    for i in range(len(big_string)):
        new_results = search_index(big_string, search_tree, i)
        if new_results:
            results[i] = new_results

    return results


def main():
    small_strings = ["dog", "cat", "mydog", "blob"]
    search_tree = build_search_tree(small_strings)
    big_string = "mydog says we have a cat and my blob thinks the same"
    print(search(big_string, search_tree))


if __name__ == '__main__':
    main()
