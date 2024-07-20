import pandas
import pandas as pd
from typing import List
import numpy as np


CSV_PATH = r"C:\Users\rgeva\Downloads\poverty_raw_data.csv"


class FunctionalDependency(object):
    def __init__(self, source, dst):
        self.source = source
        self.dst = dst


def check_functional_dependency(data: pandas.DataFrame, src_index: int, dst_index: int) -> bool:
    src_to_dst = {}
    for value in data.values:
        src_value = value[src_index]
        dst_value = value[dst_index]
        if np.nan in (src_value, dst_value):
            continue  # ignore NULLs

        if src_value in src_to_dst and src_to_dst[src_value] != dst_value:
            return False

        src_to_dst[src_value] = dst_value

    return True


def find_functional_dependencies(data: pandas.DataFrame) -> List[FunctionalDependency]:
    dependencies = []
    for src_index in range(len(data.columns)):
        for dst_index in range(len(data.columns)):
            if src_index == dst_index:
                continue

            if check_functional_dependency(data, src_index, dst_index):
                dependencies.append(FunctionalDependency(data.columns[src_index], data.columns[dst_index]))

    return dependencies


def constant_columns(data: pandas.DataFrame):
    const_columns = [i for i in range(len(data.columns))]  # set all columns as constant, later remove the non-constant ones
    values = {}
    for row in data.values:
        non_const_discoveries = set()
        for i in const_columns:
            if row[i] is np.nan:
                continue

            if i not in values:
                values[i] = row[i]

            if values[i] != row[i]:
                non_const_discoveries.add(i)
        const_columns = [i for i in const_columns if i not in non_const_discoveries]

    return [data.columns[i] for i in const_columns]

def main():
    poverty_flat = pd.read_csv(r"C:\Users\rgeva\Downloads\poverty_raw_data.csv")
    dependencies = find_functional_dependencies(poverty_flat)
    const_columns = constant_columns(poverty_flat)
    return


if __name__ == '__main__':
    main()
