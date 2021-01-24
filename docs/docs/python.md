---
id: python
title: PyCylon
---

PyCylon is the Python binding for LibCylon (C++ Cylon). The uniqueness of PyCylon 
is that it can be used as a library or a framework. As a library, PyCylon seamlessly
integrates with PyArrow. This brings us the capability of providing the user the 
compatibility with Pandas, Numpy and Tensors. As a framework we support distributed 
relational algebra operations using MPI as the distributed backend. 

## Table API

PyCylon Table API currently offers a set of relational algebra operators to 
preprocess the data. 

### Initialize

In a Cylon programme, if you use Cylon with `MPI` backend, the initialization 
must be done as follows;

```python
ctx: CylonContext = CylonContext("mpi")
```

Without MPI, 

```python
ctx: CylonContext = CylonContext()
```

```txt
Note: In the current release, Cylon only supports MPI as a distributed backend 
```

### Load a Table

Using Cylon 

```python
from pycylon Table
from pycylon.io import read_csv

tb1: Table = read_csv(ctx, '/tmp/csv.csv', ',')
```

Using PyArrow and convert to PyCylon Table

```python
from pyarrow import csv
from pycylon.data.table import Table
from pyarrow import Table as PyArrowTable

pyarrow_tb: PyArrowTable = csv.read_csv('/tmp/csv.csv')
cylon_tb = Table.from_arrow(pyarrow_tb)
```

Also a Cylon Table can be converted to a PyArrow Table

```python
pyarrow_tb: PyArrowTable = cylon_tb.to_arrow()
```
### Join

Join API supports `left, right, inner, outer' joins` with
`Hash` or `Sort` algorithms. User can specify these configs
as using Python `str`. 

Sequential Join

```python
csv_read_options = CSVReadOptions().use_threads(True).block_size(1 << 30)

tb1: Table = read_csv(ctx, table1_path, csv_read_options)

tb2: Table = read_csv(ctx, table2_path, csv_read_options)

tb3: Table = tb1.join(table=tb2, join_type='inner', algorithm='hash', left_on=[0],
                      right_on=[0])
```

Distributed Join

```python
csv_read_options = CSVReadOptions().use_threads(True).block_size(1 << 30)

tb1: Table = read_csv(ctx, table1_path, csv_read_options)

tb2: Table = read_csv(ctx, table2_path, csv_read_options)

tb3: Table = tb1.distributed_join(table=tb2, join_type='inner', algorithm='hash', left_on=[0],
                                  right_on=[0])
```

### Union

Sequential Union

```python
tb4: Table = tb1.union(tb2)
```

Distributed Union

```python
tb5: Table = tb1.distributed_union(table=tb2)
```

### Intersect

Sequential Intersect

```python
tb4: Table = tb1.intersect(table=tb2)
```

Distributed Intersect

```python
tb5: Table = tb1.distributed_intersect(table=tb2)
```

### Subtract 

Sequential Subtract

```python
tb4: Table = tb1.subtract(table=tb2)
```

Distributed Subtract

```python
tb5: Table = tb1.distributed_subtract(table=tb2)
```


### Select

```Note
This is not yet supported from PyCylon API, but LibCylon supports this.
```

## Python Examples

1. [Relational Algebra Examples](https://github.com/cylondata/cylon/blob/master/python/examples
/table_relational_algebra.py)
2. [Compute Examples](https://github.com/cylondata/cylon/blob/master/python/examples
/table_compute_examples.py)