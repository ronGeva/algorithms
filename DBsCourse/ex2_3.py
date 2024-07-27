Q1_BOOLEAN_EXPRESSIONS = {
    "T.F_2": "(({r1} or {r2}) and {s1} and {t1}) or"
             " (({r1} or {r2}) and {s2} and {t1}) or"
             " (({r1} or {r2}) and {s1} and {t2}) or"
             " (({r1} or {r2}) and {s2} and {t2})",
    "T.F_3": "{r3} and {s3} and {t3}"
}

Q2_BOOLEAN_EXPRESSIONS = {
    "T.D_2": "{r3} and {t3}",
    "T.D_1": "(({r1} or {r2}) and {t1}) or (({r1} or {r2}) and {t2})"
}

VARS_PROBABILITY = {
    "r1": 0.5,
    "r2": 0.5,
    "r3": 0.5,
    "s1": 1,
    "s2": 1,
    "s3": 1,
    "t1": 0.3,
    "t2": 0.3,
    "t3": 0.3
}


def calc_probability(expression: str) -> float:
    total = 0
    variable_names = list(VARS_PROBABILITY.keys())
    for assignment in range(0, pow(2, len(VARS_PROBABILITY))):
        # each bit in the assignment variable represents the boolean value of one of the boolean variables
        # we inspect the corresponding bit for each variable and then assign True/False to it appropriately.
        variables = {variable_names[i]: ((assignment >> i) & 1) == 1 for i in range(len(VARS_PROBABILITY))}

        # calculate the probability to get this assignment, given the VARS_PROBABILITY map
        assignment_probability = 1
        for var in variables:
            assignment_probability *= VARS_PROBABILITY[var] if variables[var] else 1 - VARS_PROBABILITY[var]

        if eval(expression.format(**variables)):
            total += assignment_probability

    return total


def main():
    for query in (Q1_BOOLEAN_EXPRESSIONS, Q2_BOOLEAN_EXPRESSIONS):
        for row_name in query:
            probability = calc_probability(query[row_name])

            # round up to account for precision errors
            val = round(probability, 5)
            print(f"{row_name}: {val}")


if __name__ == '__main__':
    main()
