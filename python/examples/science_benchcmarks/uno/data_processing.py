import os
import time
import download_util
from pycylon import CylonContext
from pycylon import Table
from pycylon.io import CSVReadOptions
from pycylon.io import read_csv
import numpy as np
import pandas as pd


def load_aggregated_single_response_pandas(target='AUC', min_r2_fit=0.3, max_ec50_se=3,
                                           combo_format=False,
                                           rename=True):
    url = "https://ftp.mcs.anl.gov/pub/candle/public/benchmarks/Pilot1/combo/combined_single_response_agg"
    output_combined_single_response = \
        "/home/vibhatha/data/uno/Pilot1/workload_1/combined_single_response_agg"

    if not os.path.exists(output_combined_single_response):
        download_util.download(url=url, output_file=output_combined_single_response)

    if os.path.exists(output_combined_single_response):
        print(f"Pandas Data file : {output_combined_single_response}")
        t1 = time.time()
        df = pd.read_csv(output_combined_single_response, engine='c', sep='\t',
                         dtype={'SOURCE': str, 'CELL': str, 'DRUG': str, 'STUDY': str,
                                'AUC': np.float32, 'IC50': np.float32,
                                'EC50': np.float32, 'EC50se': np.float32,
                                'R2fit': np.float32, 'Einf': np.float32,
                                'HS': np.float32, 'AAC1': np.float32,
                                'AUC1': np.float32, 'DSS1': np.float32})
        t2 = time.time()
        df = df[(df['R2fit'] >= min_r2_fit)]  # & (df['EC50se'] <= max_ec50_se)]
        filter_time = time.time() - t2
        print("Pandas Data Loading Time ", df.shape, t2 - t1)
        print("Pandas Filter Time 1", df.shape, filter_time)

        print("micro bench Pandas")
        t1 = time.time()
        df1 = df['R2fit']
        t2 = time.time()
        df3 = df1 >= min_r2_fit
        t3 = time.time()
        df4 = df[df3]
        t4 = time.time()
        print("Microbenchs")
        print(t2 - t1, t3 - t2, t4 - t3)


def load_aggregated_single_response_cylon(target='AUC', min_r2_fit=0.3, max_ec50_se=3,
                                          combo_format=False,
                                          rename=True):
    url = "https://ftp.mcs.anl.gov/pub/candle/public/benchmarks/Pilot1/combo/combined_single_response_agg"
    output_combined_single_response = \
        "/home/vibhatha/data/uno/Pilot1/workload_1/combined_single_response_agg"

    if not os.path.exists(output_combined_single_response):
        download_util.download(url=url, output_file=output_combined_single_response)

    if os.path.exists(output_combined_single_response):
        print(f"Data file : {output_combined_single_response}")
        ctx: CylonContext = CylonContext(config=None, distributed=False)
        csv_read_options = CSVReadOptions().use_threads(True).block_size(1 << 30).with_delimiter(
            "\t")
        t1 = time.time()
        tb: Table = read_csv(ctx, output_combined_single_response, csv_read_options)
        t2 = time.time()
        tb = tb[(tb['R2fit'] >= min_r2_fit)]  # & (tb['EC50se'] <= max_ec50_se)]
        filter_time = time.time() - t2
        print("Cylon ", tb.row_count, tb.column_count, tb.column_names)
        print("Cylon Data Loading Time: ", t2 - t1)
        print("Cylon Data Filter Time: ", filter_time)

        print("micro bench Cylon")
        t1 = time.time()
        tb1 = tb['R2fit']
        t2 = time.time()
        tb3 = tb1 >= min_r2_fit
        t3 = time.time()
        tb4 = tb[tb3]
        t4 = time.time()
        print("Microbenchs")
        print(t2 - t1, t3 - t2, t4 - t3)


load_aggregated_single_response_pandas()
load_aggregated_single_response_cylon()
