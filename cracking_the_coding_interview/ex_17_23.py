"""
In this problem we receive a matrix filled with boolean values.
We need to find the biggest "square" inside the matrix whose borders are made entirely out of the "True" value.

Our solution revolves around precomputing the maximal consecutive True values starting from each cell and moving
right/bottom.
This way for each cell in the matrix we can conclude the maximal square which has it as its top-left corner in O(N) at
worst (where N is the amount of rows/column in the matrix).
This yields a total run complexity of O(N^3) for a N^2 sized matrix.
This solution also requires O(N^2) space (to save the pre-computed data).
"""


def build_max_col_matrix(input_matrix: list[list[bool]]):
    matrix = [[0 for _ in range(len(row))] for row in input_matrix]
    for col_index in range(len(input_matrix[0])):
        current_max_subsequent_cells = 0
        for row_index in range(len(input_matrix) - 1, -1, -1):
            if input_matrix[row_index][col_index]:
                current_max_subsequent_cells += 1
            else:
                current_max_subsequent_cells = 0
            matrix[row_index][col_index] = current_max_subsequent_cells
    return matrix


def build_max_row_matrix(input_matrix: list[list[bool]]):
    matrix = [[0 for _ in range(len(row))] for row in input_matrix]
    for row_index in range(len(input_matrix)):
        row = input_matrix[row_index]
        current_max_subsequent_cells = 0
        for col_index in range(len(row) - 1, -1, -1):
            if row[col_index]:
                current_max_subsequent_cells += 1
            else:
                current_max_subsequent_cells = 0
            matrix[row_index][col_index] = current_max_subsequent_cells
    return matrix


def find_maximal_square(input_matrix: list[list[bool]], row_mat: list[list[int]], col_mat: list[list[int]]):
    """
    Finds the biggest square in the input matrix whose borders are entirely made out of the True value.
    :param input_matrix: A boolean matrix.
    :param row_mat: A matrix with an equal size to the input matrix. The integer at [row][column] indicate the amount
    of consecutive True values starting from the cell at [row][column] and moving to the right.
    :param col_mat: A matrix with an equal size to the input matrix. The integer at [row][column] indicate the amount
    of consecutive True values starting from the cell at [row][column] and moving to the bottom.
    :return: The size of the maximal square and its top-left coordinates.
    """
    max_square_found = 0
    coordinates = (0, 0)
    for row_index in range(len(input_matrix)):
        for col_index in range(len(input_matrix[row_index])):
            current_max_square = min(col_mat[row_index][col_index], row_mat[row_index][col_index])
            while current_max_square > max_square_found:
                right_border_col = col_index + current_max_square - 1
                lower_border_row = row_index + current_max_square - 1
                if col_mat[row_index][right_border_col] >= current_max_square and \
                        row_mat[lower_border_row][col_index] >= current_max_square:
                    max_square_found = current_max_square
                    coordinates = (row_index, col_index)
                current_max_square -= 1
    return max_square_found, coordinates


def main():
    input_matrix = [[False, False, False, True],
                    [True, True, True, True],
                    [False, True, False, True],
                    [True, True, True, True]]
    col_mat = build_max_col_matrix(input_matrix)
    row_mat = build_max_row_matrix(input_matrix)
    max_square_size, coordinates = find_maximal_square(input_matrix, row_mat, col_mat)
    print(max_square_size, coordinates)


if __name__ == '__main__':
    main()
