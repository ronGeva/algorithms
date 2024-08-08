import pandas
import pickle
import itertools
import duckdb
import math
from typing import List, Tuple, Any, Dict, Optional, Set
from collections import Counter
import numpy as np
from os.path import abspath, join, dirname, exists
from progressbar import progressbar

CURRENT_DIRECTORY = dirname(abspath(__file__))
CACHE_FILE = join(CURRENT_DIRECTORY, 'ex2_cache')
CSV_PATH = join(CURRENT_DIRECTORY, 'poverty_raw_data.csv')
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


def perform_queries(poverty_flat: pandas.DataFrame):
    Countries = duckdb.query("select distinct country_name, country_code, region_code from poverty_flat")
    CountryYearlyResults = duckdb.query(
        "select distinct survey_year, country_name, survey_coverage, reporting_gdp, distribution_type from poverty_flat")
    GeneralSurveyResults = duckdb.query(
        "select distinct survey_year, reporting_level, country_name, reporting_pce, reporting_pop, cpi from poverty_flat")
    SurveyDescription = duckdb.query(
        "select distinct welfare_type, survey_year, country_name, survey_comparability, survey_acronym from poverty_flat")

    res1 = duckdb.query(
        "select distinct Countries.country_name, CountryYearlyResults.survey_year, SurveyDescription.survey_acronym from"
        " Countries join CountryYearlyResults on"
        " (Countries.country_name = CountryYearlyResults.country_name) join SurveyDescription on "
        "(CountryYearlyResults.country_name = SurveyDescription.country_name and"
        " CountryYearlyResults.survey_year = SurveyDescription.survey_year)"
        " where Countries.region_code = 'LAC' and CountryYearlyResults.survey_coverage = 'national'")
    res_out_path = join(CURRENT_DIRECTORY, 'res1.csv')
    print(f"Writing the result of the first query into {res_out_path}")
    res1.write_csv(res_out_path)

    res2 = duckdb.query(
        "select distinct Countries.country_name, GeneralSurveyResults.reporting_pce from Countries join GeneralSurveyResults on (Countries.country_name = GeneralSurveyResults.country_name)"
        " where Countries.region_code = 'SSA' and GeneralSurveyResults.reporting_pce <= ALL"
        "(select GeneralSurveyResults.reporting_pce from "
        "Countries join GeneralSurveyResults on (Countries.country_name = GeneralSurveyResults.country_name)"
        "where Countries.region_code = 'SSA' and GeneralSurveyResults.reporting_pce is not NULL )")

    res_out_path = join(CURRENT_DIRECTORY, 'res2.csv')
    print(f"Writing the result of the first query into {res_out_path}")
    res2.write_csv(res_out_path)


def is_null(val):
    return val is np.nan or (isinstance(val, float) and math.isnan(val))


def get_null_columns(poverty_flat: pandas.DataFrame) -> Set[int]:
    null_columns = set()
    for row in poverty_flat.values:
        for col_index in range(len(poverty_flat.columns)):
            val = row[col_index]
            if is_null(val):
                null_columns.add(col_index)

    return null_columns


def get_functional_mappings(poverty_flat: pandas.DataFrame, tables: List[BCNFTable],
                            null_columns: Set[int]) -> list:
    """
    Return a mapping of <column indexes>: <destination indexes> such that the columns at <column indexes> determine
    the value of the column at <destination index>.
    We only compute such pairings when <destination index> is of a column containing a NULL value, and we only use
    the dependencies captures in the BCNF tables list.
    """
    mappings = []
    for index in null_columns:
        column_name = list(poverty_flat.columns)[index]
        for table in tables:
            if column_name in table.columns and column_name not in table.primary_key:
                mappings.append(
                    (set(list(poverty_flat.columns).index(source) for source in table.primary_key), index))
                break
    return mappings


def get_functional_completion(poverty_flat: pandas.DataFrame, functional_mappings: list):
    """
    Return a mapping of <values>: <value> such that when the appropriate source columns from "functional_mappings" are
    assigned <values> the appropriate destination column from "functional_mappings" is assigned <value>.
    """
    functional_completion = {}
    for row in poverty_flat.values:
        for completion in functional_mappings:
            source_indexes, dst_index = completion
            dst_value = row[dst_index]

            if not is_null(dst_value):
                functional_completion[tuple(row[item] for item in source_indexes)] = dst_value
    return functional_completion


def fill_nulls_using_functional_dependency(poverty_flat: pandas.DataFrame, tables: List[BCNFTable]):
    null_columns = get_null_columns(poverty_flat)
    functional_mapping = get_functional_mappings(poverty_flat, tables, null_columns)
    functional_completion = get_functional_completion(poverty_flat, functional_mapping)
    fixed_table = []
    for row in poverty_flat.values:
        for completion in functional_mapping:
            source_indexes, dst_index = completion
            source_values = tuple(row[index] for index in source_indexes)
            if source_values in functional_completion and is_null(row[dst_index]):
                dst_value = functional_completion[source_values]
                row[dst_index] = dst_value
        fixed_table.append(row)

    fixed_table_frame = pandas.DataFrame(fixed_table, columns=poverty_flat.columns)
    out_path = join(CURRENT_DIRECTORY, 'null_fix_using_fd.csv')
    print(f"writing the result of fixing nulls using function dependencies into {out_path}")
    duckdb.query(r'select * from fixed_table_frame').write_csv(out_path)


def fix_null_by_removal(poverty_flat: pandas.DataFrame):
    null_columns = get_null_columns(poverty_flat)
    fixed_table = []
    for row in poverty_flat.values:
        for null_col in null_columns:
            if is_null(row[null_col]):
                continue
            fixed_table.append(row)
    fixed_table_frame = pandas.DataFrame(fixed_table, columns=poverty_flat.columns)
    out_path = join(CURRENT_DIRECTORY, 'null_fix_using_removal.csv')
    print(f"writing the result of fixing nulls using removal into {out_path}")
    duckdb.query(r'select * from fixed_table_frame').write_csv(out_path)


def get_most_common_values(poverty_flat: pandas.DataFrame, column_indexes: List[int]) -> Dict[int, Any]:
    counters = {index: Counter() for index in column_indexes}
    for row in poverty_flat.values:
        for column_index in column_indexes:
            counters[column_index].update({row[column_index]})
    return {index: counters[index].most_common(n=1)[0][0] for index in counters}


def fix_nulls_by_most_frequent(poverty_flat: pandas.DataFrame):
    null_columns = get_null_columns(poverty_flat)
    common_values = get_most_common_values(poverty_flat, null_columns)
    fixed_table = []
    for row in poverty_flat.values:
        for column_index in null_columns:
            if is_null(row[column_index]):
                row[column_index] = common_values[column_index]

        fixed_table.append(row)
    fixed_table_frame = pandas.DataFrame(fixed_table, columns=poverty_flat.columns)
    out_path = join(CURRENT_DIRECTORY, 'null_fix_using_most_common.csv')
    print(f"writing the result of fixing nulls using most common value into {out_path}")
    duckdb.query(r'select * from fixed_table_frame').write_csv(out_path)


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

    perform_queries(poverty_flat)

    fill_nulls_using_functional_dependency(poverty_flat, tables)
    fix_null_by_removal(poverty_flat)
    fix_nulls_by_most_frequent(poverty_flat)


if __name__ == '__main__':
    main()
