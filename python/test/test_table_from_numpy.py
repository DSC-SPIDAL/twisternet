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


import numpy as np
import pyarrow as pa
import pandas as pd
import pycylon as cn
from pycylon import CylonContext

ctx: CylonContext = CylonContext(config=None, distributed=False)

columns = 2

data1 = np.array([0, 1, 2, 3, 4, 5], dtype=np.int32)
data2 = np.array([10, 11, 12, 13, 14, 15], dtype=np.float32)

nd_array_list = [data1, data2]

ar_array: pa.array = pa.array(nd_array_list)

ar_table: pa.Table = pa.Table.from_arrays(nd_array_list, names=['x0', 'x1'])

print(ar_table)

ar1 = pa.array([1, 2, 3, 4])
ar2 = pa.array(['a', 'b', 'c', 'd'])

ar_tb2: pa.Table = pa.Table.from_arrays([ar1, ar2], names=['col1', 'col2'])

print(ar_tb2)

col_names = ['col1', 'col2']

cn_tb1 = cn.Table.from_numpy(ctx, col_names, nd_array_list)

cn_tb1.show()

data_list = [[1, 2, 3, 4], ['p', 'q', 'r', 's']]

cn_tb2 = cn.Table.from_list(ctx, col_names, data_list)

cn_tb2.show()

dict1 = {'col1': [1, 2], 'col2': ['a', 'b']}

ar_tb3: pa.Table = pa.Table.from_pydict(dict1)

print(ar_tb3)

cn_tb3: cn.Table = cn.Table.from_pydict(ctx, dict1)

cn_tb3.show()

pdf = pd.DataFrame(dict1)

# df, Schema schema=None, preserve_index=None, nthreads=None, columns=None, bool safe=True

cn_tb4: cn.Table = cn.Table.from_pandas(ctx, pdf)

cn_tb4.show()

print(cn_tb4.to_pandas())

dict2 = {'col1': [1, 2], 'col2': [2, 4]}

cn_tb5: cn.Table = cn.Table.from_pydict(ctx, dict2)

npy = cn_tb5.to_numpy()
print(npy, npy.dtype)

dict3 = cn_tb5.to_pydict()

print(dict3)

print(cn_tb5.schema)

print(cn_tb5.column_names)

print(ar_tb2)

print(cn_tb5.to_numpy())