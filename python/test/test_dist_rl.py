##
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
##

'''
Run test:
>> mpirun -n 4 python -m pytest --with-mpi -q python/test/test_dist_rl.py
'''

import os
import pytest
from pycylon import Table
from pycylon.net import MPIConfig
from pycylon import CylonContext
from pycylon.io import CSVReadOptions
from pycylon.io import read_csv


@pytest.mark.mpi
def test_distributed_ra():
    mpi_config = MPIConfig()
    ctx: CylonContext = CylonContext(config=mpi_config, distributed=True)

    rank = ctx.get_rank()
    size = ctx.get_world_size()

    assert size == 4

    table1_path = f'/tmp/user_usage_tm_{rank + 1}.csv'
    table2_path = f'/tmp/user_usage_tm_{rank + 1}.csv'

    assert os.path.exists(table1_path)
    assert os.path.exists(table2_path)

    csv_read_options = CSVReadOptions().use_threads(True).block_size(1 << 30)

    tb1: Table = read_csv(ctx, table1_path, csv_read_options)

    tb2: Table = read_csv(ctx, table2_path, csv_read_options)

    print("First Hello World From Rank {}, Size {}".format(ctx.get_rank(), ctx.get_world_size()))

    tb3: Table = tb1.distributed_join(table=tb2, join_type='inner', algorithm='hash', left_on=[0],
                                      right_on=[0])

    tb4: Table = tb1.distributed_union(tb2)

    tb5: Table = tb1.distributed_subtract(tb2)

    tb6: Table = tb1.distributed_intersect(tb2)

    ctx.barrier()

    join_row_count = tb3.row_count
    join_column_count = tb3.column_count

    subtract_row_count = tb5.row_count
    subtract_column_count = tb5.column_count

    union_row_count = tb4.row_count
    union_column_count = tb4.column_count

    intersect_row_count = tb6.row_count
    intersect_column_count = tb6.column_count

    if rank == 0:
        assert join_row_count == 1424 and join_column_count == 8
        assert subtract_row_count == 0 and subtract_column_count == 4
        assert union_row_count == 56 and union_column_count == 4
        assert intersect_row_count == 56 and intersect_column_count == 4

    if rank == 1:
        assert join_row_count == 1648 and join_column_count == 8
        assert subtract_row_count == 0 and subtract_column_count == 4
        assert union_row_count == 61 and union_column_count == 4
        assert intersect_row_count == 61 and intersect_column_count == 4

    if rank == 2:
        assert join_row_count == 2704 and join_column_count == 8
        assert subtract_row_count == 0 and subtract_column_count == 4
        assert union_row_count == 51 and union_column_count == 4
        assert intersect_row_count == 51 and intersect_column_count == 4

    if rank == 3:
        assert join_row_count == 1552 and join_column_count == 8
        assert subtract_row_count == 0 and subtract_column_count == 4
        assert union_row_count == 72 and union_column_count == 4
        assert intersect_row_count == 72 and intersect_column_count == 4

    # Note: Not needed when using PyTest with MPI
    #ctx.finalize()
