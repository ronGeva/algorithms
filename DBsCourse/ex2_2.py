"""
Simulates stuff for the second exercise part 2.
"""

import duckdb
import numpy as np
import pandas as pd

# Create in-memory arrays
array1 = np.array([(1, 2, 2), (1, 2, 2), (2, 3, 3)], dtype=[('A', 'i4'), ('B', 'i4'), ('C', 'i4')])
array2 = np.array([(1, 1, 2), (1, 2, 2), (2, 3, 3)], dtype=[('A', 'i4'), ('C', 'i4'), ('D', 'i4')])
array3 = np.array([(1, 1, 2), (1, 2, 2), (2, 3, 3)], dtype=[('D', 'i4'), ('E', 'i4'), ('F', 'i4')])

# Convert arrays to pandas DataFrames
df1 = pd.DataFrame(array1)
df2 = pd.DataFrame(array2)
df3 = pd.DataFrame(array3)

# Connect to DuckDB in-memory database
con = duckdb.connect(database=':memory:')

# Create DuckDB tables from DataFrames
con.execute("CREATE TABLE R AS SELECT * FROM df1")
con.execute("CREATE TABLE S AS SELECT * FROM df2")
con.execute("CREATE TABLE T AS SELECT * FROM df3")

# Perform a join query on the three tables
query = """
    SELECT DISTINCT * FROM R AS R1, R AS R2, T WHERE R1.B=T.F AND R1.C=R2.C 
"""

# Execute the query and fetch the results
result = con.execute(query).fetchall()

# Print the results
print("(A, B, C, A, C, D, D, E, F)")
for row in result:
    print(row)
