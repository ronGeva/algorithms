import pandas
import pickle
import itertools
import duckdb
from typing import List, Tuple, Any, Dict, Optional, Set
import numpy as np
from os.path import abspath, join, dirname, exists
from progressbar import progressbar

CACHE_FILE = join(dirname(abspath(__file__)), 'ex2_cache')


CSV_PATH = join(dirname(abspath(__file__)), 'poverty_raw_data.csv')
NON_KEY_COLUMNS = ("decile10", "decile9", "decile8", "decile7", "decile6", "decile5", "decile4", "decile3", "decile2",
                   "decile1", "polarization", "gini", "median", "mean", "watts", "headcount", "mld", "cpi",
                   "reporting_pop", "reporting_gdp", "reporting_pce", "record_id", "ppp", "poverty_gap",
                   "poverty_severity")


class FunctionalDependency(object):
    def __init__(self, source, dst):
        self.source = source
        self.dst = dst


class BCNFTable(object):
    def __init__(self, columns: Set[str], primary_key: Set[str]):
        self.columns = columns
        self.primary_key = primary_key

    def __str__(self):
        desc = ""
        non_primary_key_cols = self.columns - self.primary_key
        for key in self.primary_key:
            desc += "*{}*, ".format(key)

        for col in non_primary_key_cols:
            desc += col + ", "

        return desc


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


def find_sources_functional_dependencies(data: pandas.DataFrame, source_indexes: Tuple[int]):
    dependencies = []

    for dst_index in range(len(data.columns)):
        if dst_index in source_indexes:
            continue

        if check_functional_dependency(data, source_indexes, dst_index):
            dependencies.append(FunctionalDependency(tuple(data.columns[i] for i in source_indexes),
                                                     data.columns[dst_index]))
    return dependencies


def find_functional_dependencies(data: pandas.DataFrame, max_key_size: int, cached=True) -> List[FunctionalDependency]:
    if exists(CACHE_FILE) and cached:
        with open(CACHE_FILE, 'rb') as f:
            return pickle.load(f)

    dependencies = []
    possible_source_indexes = [list(data.columns).index(c) for c in set(data.columns) - set(NON_KEY_COLUMNS)]
    source_tuples = itertools.product(possible_source_indexes, repeat=max_key_size)
    for source_indexes in progressbar(set(source_tuples)):
        dependencies += find_sources_functional_dependencies(data, source_indexes)

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


def best_bcnf_breaking_dependency(dependant_columns: Dict[Tuple[str], List[str]]) -> Optional[Tuple[str]]:
    """
    Retrieves a dependency that breaks BCNF - aka a dependency between a group of columns A to a group of columns B
    such that A->B.
    We prioritize the rules in this order:
    1. Minimal left side size (len(A) should be as small as possible).
    2. Maximal right hand size (len(B) should be as big as possible).
    """
    for source_size in range(1, 5):
        max_right_hand = (None, 0)
        for source_cols in dependant_columns:
            if len(set(source_cols)) != source_size:
                continue

            if len(dependant_columns[source_cols]) > max_right_hand[1]:
                max_right_hand = (source_cols, len(dependant_columns[source_cols]))

        if max_right_hand[0] is not None:
            return max_right_hand[0]

    return None


def split_table_according_to_dependency(dependant_columns: Dict[Tuple[str], List[str]], tables: List[BCNFTable],
                                        source_cols: Tuple[str]) -> Dict[Tuple[str], Tuple[str]]:
    tables.append(BCNFTable(set(source_cols + tuple(dependant_columns[source_cols])), set(source_cols)))

    destination_cols = set(dependant_columns[source_cols])

    # remove destination from all dependencies
    new_dependencies = {}
    for source in dependant_columns:
        #  the removed columns are in the source, remove it
        if set(source).intersection(destination_cols):
            continue

        #  the right hand side of the dependency is a subset of the removed cols, remove it
        if set(dependant_columns[source]).issubset(destination_cols):
            continue

        new_dependencies[source] = tuple(dst for dst in dependant_columns[source] if dst not in destination_cols)

    return new_dependencies


def split_to_bcnf(dependant_columns: Dict[Tuple[str], List[str]], tables: List[BCNFTable]):
    dependency = best_bcnf_breaking_dependency(dependant_columns)
    if dependency is None:
        return

    dependant_columns = split_table_according_to_dependency(dependant_columns, tables, dependency)
    split_to_bcnf(dependant_columns, tables)


def perform_queries(poverty_flat: pandas.DataFrame, output_directory: str):
    table1 = duckdb.query("select distinct country_name, country_code, region_code from poverty_flat")
    table3 = duckdb.query(
        "select distinct survey_year, country_name, survey_coverage, reporting_gdp, distribution_type from poverty_flat")
    table6 = duckdb.query(
        "select distinct survey_year, reporting_level, country_name, reporting_pce, reporting_pop, cpi from poverty_flat")
    table7 = duckdb.query(
        "select distinct welfare_type, survey_year, country_name, survey_comparability, survey_acronym from poverty_flat")

    res1 = duckdb.query(
        "select distinct table1.country_name, table3.survey_year, table7.survey_acronym from table1 join table3 on"
        " (table1.country_name = table3.country_name) join table7 on (table1.country_name = table7.country_name)"
        " where table1.region_code = 'LAC' and table3.survey_coverage = 'national'")
    res1.write_csv(join(output_directory, 'out2.csv'))

    res2 = duckdb.query(
        "select distinct table1.country_name, table6.reporting_pce from table1 join table6 on (table1.country_name = table6.country_name)"
        " where table1.region_code = 'SSA' and table6.reporting_pce <= ALL"
        "(select table6.reporting_pce from "
        "table1 join table6 on (table1.country_name = table6.country_name)"
        "where table1.region_code = 'SSA' and table6.reporting_pce is not NULL )")

    res1.write_csv(join(output_directory, 'out3.csv'))


def main():
    poverty_flat = pandas.read_csv(CSV_PATH)
    const_columns = constant_columns(poverty_flat)

    poverty_flat = poverty_flat.drop(columns=const_columns.keys())
    dependencies = find_functional_dependencies(poverty_flat, max_key_size=4)
    dependant_columns = {}
    for dependency in dependencies:
        if dependency.source not in dependant_columns:
            dependant_columns[dependency.source] = []
        dependant_columns[dependency.source].append(dependency.dst)

    tables = []
    split_to_bcnf(dependant_columns, tables)

    print("BCNF form tables (primary keys are shown surrounded with '*'):")
    for table in tables:
        print(table)

    perform_queries(poverty_flat, r'C:\temp')


if __name__ == '__main__':
    main()
