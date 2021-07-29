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

import os
import getpass
import numpy as np
import torch
from pyarrow import Table as PyArrowTable
from pyarrow import Tensor as ArrowTensor
from torch import Tensor as TorchTensor

from pycylon import Table
from pycylon.csv import csv_reader
from pycylon.util.benchutils import benchmark_with_repitions

'''
## Reference

Get the data from [Here](https://www.kaggle.com/oddrationale/mnist-in-csv/)

'''

'''
Configurations
'''

'''
## Place Data

Place the data in the relative path

Assume the data is your 
/home/<your_username>/data/mnist/full/
'''

username = getpass.getuser()

base_path: str = "/home/{}/data/mnist/full/".format(username)
train_file_name: str = "mnist_train.csv"
test_file_name: str = "mnist_test.csv"
train_file_path: str = os.path.join(base_path, train_file_name)
test_file_path: str = os.path.join(base_path, test_file_name)
delimiter: str = ","

'''
Timing Configurations:

'''
reps: int = 10
time_data_loading: int = 0
time_cntb_to_arrowtb: int = 0
time_pyarwtb_to_numpy: int = 0
time_numpy_to_arrowtn: int = 0
time_numpy_to_torchtn: int = 0
time_type: str = "ms"

'''
Check Data Files
'''

print("Train File Path : {}".format(train_file_path))
print("Test File Path : {}".format(test_file_path))

assert os.path.exists(train_file_path) == True
assert os.path.exists(test_file_path) == True

'''
Global Vars
'''

tb_train: Table = None
tb_test: Table = None
tb_train_arw: PyArrowTable = None
tb_test_arw: PyArrowTable = None
train_npy: np.ndarray = None
test_npy: np.ndarray = None
train_arrow_tensor: ArrowTensor = None
test_arrow_tensor: ArrowTensor = None
train_torch_tensor: TorchTensor = None
test_torch_tensor: TorchTensor = None

'''
load To PyCylon Tables
'''


@benchmark_with_repitions(repititions=reps, time_type=time_type)
def load_data_to_cn_tables():
    tb_train: Table = csv_reader.read(train_file_path, delimiter)
    tb_test: Table = csv_reader.read(test_file_path, delimiter)
    return tb_train, tb_test


'''
If some pre-processing to do, do it here...
Join, shuffle, partition, etc
'''


@benchmark_with_repitions(repititions=reps, time_type=time_type)
def convert_cn_table_to_arrow_table():
    tb_train_arw: PyArrowTable = Table.to_arrow(tb_train)
    tb_test_arw: PyArrowTable = Table.to_arrow(tb_test)
    return tb_train_arw, tb_test_arw


@benchmark_with_repitions(repititions=reps, time_type=time_type)
def covert_arrow_table_to_numpy():
    train_npy: np.ndarray = tb_train_arw.to_pandas().to_numpy()
    test_npy: np.ndarray = tb_test_arw.to_pandas().to_numpy()
    return train_npy, test_npy


@benchmark_with_repitions(repititions=reps, time_type=time_type)
def convert_numpy_to_arrow_tensor():
    train_arrow_tensor = ArrowTensor.from_numpy(train_npy)
    test_arrow_tensor = ArrowTensor.from_numpy(test_npy)
    return train_arrow_tensor, test_arrow_tensor


@benchmark_with_repitions(repititions=reps, time_type=time_type)
def convert_numpy_to_torch_tensor():
    train_torch_tensor: TorchTensor = torch.from_numpy(train_npy)
    test_torch_tensor: TorchTensor = torch.from_numpy(test_npy)
    return train_torch_tensor, test_torch_tensor


########################################################################################################################

time_data_loading, (tb_train, tb_test) = load_data_to_cn_tables()
time_cntb_to_arrowtb, (tb_train_arw, tb_test_arw) = convert_cn_table_to_arrow_table()
time_pyarwtb_to_numpy, (train_npy, test_npy) = covert_arrow_table_to_numpy()
time_numpy_to_arrowtn, (train_arrow_tensor, test_arrow_tensor) = convert_numpy_to_arrow_tensor()
time_numpy_to_torchtn, (train_torch_tensor, test_torch_tensor) = convert_numpy_to_torch_tensor()

print("Data Loading Average Time : {} {}".format(time_data_loading, time_type))
print("PyCylon Table to PyArrow Table Average Time : {} {}".format(time_cntb_to_arrowtb, time_type))
print("Pyarrow Table to Numpy Average Time : {} {}".format(time_pyarwtb_to_numpy, time_type))
print("Numpy to Arrow Tensor Average Time : {} {}".format(time_numpy_to_arrowtn, time_type))
print("Numpy to Torch Tensor Average Time : {} {}".format(time_numpy_to_torchtn, time_type))

print("===========================================================================================")
#########################################################################################################

