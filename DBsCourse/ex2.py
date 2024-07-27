import pandas
import pickle
import itertools
from typing import List, Tuple, Any
import numpy as np
from os.path import abspath, join, dirname, exists

CACHE_FILE = join(dirname(abspath(__file__)), 'ex2_cache')


CSV_PATH = r"C:\Users\Ron\Downloads\poverty_raw_data.csv"
NON_KEY_COLUMNS = ("decile10", "decile9", "decile8", "decile7", "decile6", "decile5", "decile4", "decile3", "decile2",
                   "decile1", "polarization", "gini", "median", "mean", "watts", "headcount", "mld", "cpi",
                   "reporting_pop", "reporting_gdp", "reporting_pce", "record_id", "ppp", "poverty_gap")


class FunctionalDependency(object):
    def __init__(self, source, dst):
        self.source = source
        self.dst = dst


def check_functional_dependency(data: pandas.DataFrame, src_indexes: Tuple[Any], dst_index: int) -> bool:
    src_to_dst = {}
    for value in data.values:
        src_values = tuple(value[index] for index in src_indexes)
        dst_value = value[dst_index]
        if np.nan in (*src_values, dst_value):
            continue  # ignore NULLs

        if src_values in src_to_dst and src_to_dst[src_values] != dst_value:
            return False

        src_to_dst[src_values] = dst_value

    return True


def find_functional_dependencies(data: pandas.DataFrame, max_key_size: int) -> List[FunctionalDependency]:
    if exists(CACHE_FILE):
        with open(CACHE_FILE, 'rb') as f:
            return pickle.load(f)

    dependencies = []
    possible_source_indexes = [list(data.columns).index(c) for c in set(data.columns) - set(NON_KEY_COLUMNS)]
    source_tuples = itertools.product(possible_source_indexes, repeat=max_key_size)
    for source_indexes in source_tuples:
        for dst_index in range(len(data.columns)):
            if dst_index in source_indexes:
                continue

            if check_functional_dependency(data, source_indexes, dst_index):
                dependencies.append(FunctionalDependency(tuple(data.columns[i] for i in source_indexes),
                                                         data.columns[dst_index]))

    with open(CACHE_FILE, 'wb') as f:
        pickle.dump(dependencies, f)

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

    return {data.columns[i]: values[i] for i in const_columns}


def main():
    poverty_flat = pandas.read_csv(CSV_PATH)
    const_columns = constant_columns(poverty_flat)

    poverty_flat = poverty_flat.drop(columns=const_columns.keys())
    dependencies = find_functional_dependencies(poverty_flat, max_key_size=3)
    nodes = {}
    for dependency in dependencies:
        if dependency.source not in nodes:
            nodes[dependency.source] = []
        nodes[dependency.source].append(dependency.dst)
    return


if __name__ == '__main__':
    main()
