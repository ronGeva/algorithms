def compute_sums_from_top_left(input_matrix: list[list[int]]):
    """
    Compute the sums of all matrices whose top-left corner is the cell (0, 0) in the input matrix.
    :param input_matrix: A matrix of integers.
    :return: A matrix of integers. value at [row][col] represents the sum of the matrix with [row] rows and [col] columns.
    """
    row_size = len(input_matrix[0])
    column_size = len(input_matrix)
    res = []

    first_row_sum = 0
    first_row_sums = []
    for col_index in range(row_size):
        first_row_sum += input_matrix[0][col_index]
        first_row_sums.append(first_row_sum)
    res.append(first_row_sums)

    for row_index in range(1, column_size):
        row_sum = 0
        row_sums = []
        for col_index in range(row_size):
            row_sum += input_matrix[row_index][col_index]
            current_mat_sum_without_current_row = res[row_index - 1][col_index]
            row_sums.append(current_mat_sum_without_current_row + row_sum)
        res.append(row_sums)
    return res


def compute_submatrix_sum(computed_matrices: list[list[int]], row_index: int, col_index: int, rows_amount: int,
                          cols_amount: int):
    mat_1 = computed_matrices[row_index + rows_amount - 1][col_index + cols_amount - 1]
    mat_4 = 0
    if row_index == 0:
        mat_2 = 0
    else:
        mat_2 = computed_matrices[row_index - 1][col_index + cols_amount - 1]
    if col_index == 0:
        mat_3 = 0
    else:
        mat_3 = computed_matrices[row_index + rows_amount - 1][col_index - 1]
    if col_index > 0 and row_index > 0:
        mat_4 = computed_matrices[row_index - 1][col_index - 1]

    current_mat_sum = mat_1 - mat_2 - mat_3 + mat_4
    return current_mat_sum


def find_maximal_submatrix(input_matrix: list[list[int]], computed_matrices: list[list[int]]):
    max_found = input_matrix[0][0]
    maximal_submatrix = (0, 0, 1, 1, input_matrix[0][0])
    for row_index in range(len(input_matrix)):
        for col_index in range(len(input_matrix[row_index])):
            for rows_amount in range(1, len(input_matrix) - row_index + 1):
                for cols_amount in range(1, len(input_matrix[row_index]) - col_index + 1):
                    current_mat_sum = compute_submatrix_sum(computed_matrices, row_index, col_index, rows_amount,
                                                            cols_amount)
                    if current_mat_sum > max_found:
                        max_found = current_mat_sum
                        maximal_submatrix = (row_index, col_index, rows_amount, cols_amount)
    return maximal_submatrix, max_found


def main():
    input_matrix = [[0, -10, -36],
                   [15, 26, 5],
                   [-110, -30, 11]]
    sums = compute_sums_from_top_left(input_matrix)
    maximal_submatrix = find_maximal_submatrix(input_matrix, sums)
    print(maximal_submatrix)


if __name__ == '__main__':
    main()
