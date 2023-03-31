
from typing import List
from utils import run_sql_code, print_column_data


def invert_axes(database: str, pulsemap: str, column_names: List[str]):

    # Print initial pulsemap values
    for column in column_names:
        print_column_data(database, pulsemap, column)

    # Delete old pulsemap
    pulsemap_name = pulsemap + '_invert'
    run_sql_code(database, "DROP TABLE IF EXISTS {};".format(pulsemap_name))

    # Create new pulsemap and add index
    string_create = "CREATE TABLE {} AS SELECT * FROM {};".format(pulsemap_name, pulsemap)
    run_sql_code(database, string_create)
    string_index = "CREATE INDEX event_no_{} ON {} (event_no);".format(pulsemap_name, pulsemap_name)
    run_sql_code(database, string_index)

    print('Pulsemap created!')

    # Invert axis columns:
    for column in column_names:

        # Print intermediate pulsemap values
        print_column_data(database, pulsemap_name, column)

        # Invert column
        string_invert = "UPDATE {} SET {} = {} * (-1.0)".format(pulsemap_name, column, column)
        run_sql_code(database, string_invert)

        # Print final pulsemap values
        print_column_data(database, pulsemap_name, column)
        print(' ')


if __name__=='__main__':

    database = "/home/kaare/snic2022-23-570/ESSnuSB/databases/ESSnuSB_z_invert.db"
    pulsemap = "ESSnuSB"
    columns = ['fZ']

    invert_axes(database, pulsemap, columns)