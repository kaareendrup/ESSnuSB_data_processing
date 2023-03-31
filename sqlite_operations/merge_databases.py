
import os
from typing import List, Tuple, Optional
from collections import OrderedDict

import numpy as np
import sqlite3
import pandas as pd
from tqdm import tqdm
import sqlalchemy

from utils import run_sql_code


def merge_databases(file_dir: str, output_name: str, n_files: int, input_name: Optional[str] = None, n_chunks: Optional[int] = 1):

    if input_name is None:
        input_name = output_name

    input_files = [file_dir+'temp/'+input_name+'_'+str(i)+'.db' for i in range(n_files)]
    output_file = file_dir+output_name+'.db'

    assert (not os.path.exists(output_file)), f"Target path for merged database, {output_file}, already exists."

    if len(input_files) > 0:
        print(f"Merging {len(input_files)} database files")

        # Create one empty database table for each extraction
        for table_name in extract_table_names(input_files[0]):
            column_names = extract_column_names(
                input_files, table_name
            )
            if len(column_names) > 1:
                is_pulse_map = is_pulsemap_check(table_name)
                create_table(
                    output_file,
                    table_name,
                    column_names,
                    is_pulse_map=is_pulse_map,
                )

        # Merge temporary databases into newly created one
        merge_temporary_databases(output_file, input_files, n_chunks)
    else:
        print("No temporary database files found!")


# Internal methods
def extract_column_names(db_paths: str, table_name: str) -> pd.Series:
    for db_path in db_paths:
        with sqlite3.connect(db_path) as con:
            query = f"select * from {table_name} limit 1"
            columns = pd.read_sql(query, con).columns
        if len(columns):
            return columns
    return []


def is_pulsemap_check(table_name: str) -> bool:
    if "fitqun" in table_name.lower() or "truth" in table_name.lower():
        return False
    else:
        return True


def extract_table_names(db_path: str):

    with sqlite3.connect(db_path) as con:
        query = f"SELECT name FROM sqlite_master WHERE type='table';"
        tables = pd.read_sql(query, con).name
    if len(tables):
        return tables
    return []


def create_table(database: str, table_name: str, columns: List[str], is_pulse_map: bool=False):

    query_columns = list()
    for column in columns:
        if column == "event_no":
            if not is_pulse_map:
                type_ = "INTEGER PRIMARY KEY NOT NULL"
            else:
                type_ = "NOT NULL"
        else:
            type_ = "FLOAT"
        query_columns.append(f"{column} {type_}")
    query_columns = ", ".join(query_columns)

    code = (
        "PRAGMA foreign_keys=off;\n"
        f"CREATE TABLE {table_name} ({query_columns});\n"
        "PRAGMA foreign_keys=on;"
    )
    run_sql_code(database, code)

    if is_pulse_map:
        attach_index(database, table_name)


def attach_index(database: str, table_name: str):

        code = (
            "PRAGMA foreign_keys=off;\n"
            "BEGIN TRANSACTION;\n"
            f"CREATE INDEX event_no_{table_name} ON {table_name} (event_no);\n"
            "COMMIT TRANSACTION;\n"
            "PRAGMA foreign_keys=on;"
        )
        run_sql_code(database, code)


def submit_to_database(database: str, key: str, data: pd.DataFrame):

    if len(data) == 0:
        print(f"No data provided for {key}.")
        return
    engine = sqlalchemy.create_engine("sqlite:///" + database)
    data.to_sql(key, engine, index=False, if_exists="append")
    engine.dispose()


def extract_table_limits(conn, table_name, chunk, n_chunks) -> Tuple[int, int]:

    query = f"SELECT COUNT(1) from {table_name}"
    n_rows = float(pd.read_sql(query, conn)['COUNT(1)'])

    limit = np.ceil(n_rows/n_chunks)
    offset = chunk * limit
    return int(limit), int(offset)


def extract_everything(db: str, chunk: int, n_chunks: int) -> "OrderedDict[str, pd.DataFrame]":

    results = OrderedDict()
    table_names = extract_table_names(db)
    with sqlite3.connect(db) as conn:
        for table_name in table_names:

            limit, offset = extract_table_limits(conn, table_name, chunk, n_chunks)
            query = f"select * from {table_name} LIMIT {limit} OFFSET {offset}"
            try:
                data = pd.read_sql(query, conn)
            except:
                data = []
            results[table_name] = data
    return results


def merge_temporary_databases(output_file: str, input_files: List[str], n_chunks: int):
    
    for input_file in tqdm(input_files, colour="green"):
        for chunk in tqdm(range(n_chunks)):

            results = extract_everything(input_file, chunk, n_chunks)
            for table_name, data in results.items():
                submit_to_database(output_file, table_name, data)


if __name__=='__main__':


    file_dir = "/home/kaare/snic2022-23-570/ESSnuSB/databases/"
    database_name = "ESSnuSB_chlep02"
    n_files = 4
    n_chunks = 10

    merge_databases(file_dir, database_name, n_files, n_chunks=n_chunks)
