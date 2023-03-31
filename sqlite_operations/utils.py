
import sqlite3
import pandas as pd


def run_sql_code(database: str, code: str):

    conn = sqlite3.connect(database)
    c = conn.cursor()
    c.executescript(code)
    c.close()


def print_column_data(database: str, pulsemap: str, column: str, limit: int=10):

    with sqlite3.connect(database) as con:
        query = "select {} from {} limit {}".format(column, pulsemap, limit)
        data = pd.read_sql(query, con)[column].to_list()
        print(column)
        print(data)