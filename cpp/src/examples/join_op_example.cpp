/*
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <glog/logging.h>
#include <chrono>

#include <net/mpi/mpi_communicator.hpp>
#include <ctx/cylon_context.hpp>
#include <table.hpp>
#include <ops/dis_join_op.hpp>
#include <mpi.h>

void create_binary_table(char *const *argv,
                         cylon::CylonContext *ctx,
                         arrow::MemoryPool *pool,
                         std::shared_ptr<arrow::Table> &left_table,
                         std::shared_ptr<arrow::Table> &right_table);

void create_int64_table(char *const *argv,
                        std::shared_ptr<cylon::CylonContext> &ctx,
                        arrow::MemoryPool *pool,
                        std::shared_ptr<arrow::Table> &left_table,
                        std::shared_ptr<arrow::Table> &right_table);

uint64_t next_random() {
  uint64_t randnumber = 0;
  for (int i = 19; i >= 1; i--) {
    uint64_t power = pow(10, i - 1);
    if (power % 2 != 0 && power != 1) {
      power++;
    }
    randnumber += power * (rand() % 10);
  }
  return randnumber;
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    LOG(ERROR) << "There should be an argument with number of records to generate";
    return 1;
  }

  auto start_start = std::chrono::steady_clock::now();
  auto mpi_config = cylon::net::MPIConfig::Make();
  auto ctx = cylon::CylonContext::InitDistributed(mpi_config);

  arrow::MemoryPool *pool = arrow::default_memory_pool();
  std::shared_ptr<arrow::Table> left_table;
  std::shared_ptr<arrow::Table> right_table;
//  create_binary_table(argv, ctx, pool, left_table, right_table);
  create_int64_table(argv, ctx, pool, left_table, right_table);
  MPI_Barrier(MPI_COMM_WORLD);

  std::shared_ptr<cylon::Table> first_table, second_table, joined;
  auto status = cylon::Table::FromArrowTable(ctx, left_table, first_table);
  if (!status.is_ok()) {
    LOG(INFO) << "Table reading failed " << argv[1];
    ctx->Finalize();
    return 1;
  }

  status = cylon::Table::FromArrowTable(ctx, right_table, second_table);
  if (!status.is_ok()) {
    LOG(INFO) << "Table reading failed " << argv[2];
    ctx->Finalize();
    return 1;
  }
  right_table.reset();
  left_table.reset();

  auto read_end_time = std::chrono::steady_clock::now();
  LOG(INFO) << "Read tables in "
            << std::chrono::duration_cast<std::chrono::milliseconds>(
                read_end_time - start_start).count() << "[ms]";

  first_table->retainMemory(false);
  second_table->retainMemory(false);
  LOG(INFO) << "Read tables in "
            << std::chrono::duration_cast<std::chrono::milliseconds>(
                read_end_time - start_start).count() << "[ms]";

  const cylon::ResultsCallback &callback = [&](int tag, const std::shared_ptr<cylon::Table> &table) {
    LOG(INFO) << tag << " Result received " << table->Rows();
  };

  const cylon::join::config::JoinConfig &join_config = cylon::join::config::JoinConfig::InnerJoin(0, 0);
  const cylon::PartitionOpConfig &part_config = cylon::PartitionOpConfig(ctx->GetWorldSize(), {0});
  const cylon::DisJoinOpConfig &dist_join_config = cylon::DisJoinOpConfig(part_config, join_config);

  auto op = cylon::DisJoinOP(ctx, first_table->get_table()->schema(), 0, callback, dist_join_config);

  op.InsertTable(100, first_table);
  op.InsertTable(200, second_table);
  first_table.reset();
  second_table.reset();
  auto execution = op.GetExecution();
  execution->WaitForCompletion();
  auto join_end_time = std::chrono::steady_clock::now();

  LOG(INFO) << "Join done in "
            << std::chrono::duration_cast<std::chrono::milliseconds>(
                join_end_time - read_end_time).count() << "[ms]";
  ctx->Finalize();
  return 0;
}

void create_binary_table(char *const *argv,
                         cylon::CylonContext *ctx,
                         arrow::MemoryPool *pool,
                         std::shared_ptr<arrow::Table> &left_table,
                         std::shared_ptr<arrow::Table> &right_table) {
  arrow::FixedSizeBinaryBuilder left_id_builder(arrow::fixed_size_binary(8), pool);
  arrow::FixedSizeBinaryBuilder right_id_builder(arrow::fixed_size_binary(8), pool);
  arrow::FixedSizeBinaryBuilder cost_builder(arrow::fixed_size_binary(8), pool);

  uint64_t count = std::stoull(argv[1]);
  uint64_t range = count * ctx->GetWorldSize();
  srand(time(NULL) + ctx->GetRank());

  arrow::Status st = left_id_builder.Reserve(count);
  st = right_id_builder.Reserve(count);
  st = cost_builder.Reserve(count);
  for (uint64_t i = 0; i < count; i++) {
    uint64_t l = next_random() % range;
    uint64_t r = next_random() % range;
    uint64_t v = next_random() % range;
    left_id_builder.UnsafeAppend((uint8_t *) (&l));
    right_id_builder.UnsafeAppend((uint8_t *) (&r));
    cost_builder.UnsafeAppend((uint8_t *) (&v));
  }

  std::shared_ptr<arrow::Array> left_id_array;
  std::shared_ptr<arrow::Array> right_id_array;
  std::shared_ptr<arrow::Array> cost_array;

  st = left_id_builder.Finish(&left_id_array);
  st = right_id_builder.Finish(&right_id_array);
  st = cost_builder.Finish(&cost_array);

  std::vector<std::shared_ptr<arrow::Field>> schema_vector = {
      arrow::field("first", arrow::fixed_size_binary(8)),
      arrow::field("second", arrow::fixed_size_binary(8))};
  auto schema = std::make_shared<arrow::Schema>(schema_vector);

  left_table= arrow::Table::Make(schema,
                                 {std::move(left_id_array), cost_array});
  right_table= arrow::Table::Make(schema,
                                  {std::move(right_id_array), std::move(cost_array)});
}

void create_int64_table(char *const *argv,
                        std::shared_ptr<cylon::CylonContext> &ctx,
                        arrow::MemoryPool *pool,
                        std::shared_ptr<arrow::Table> &left_table,
                        std::shared_ptr<arrow::Table> &right_table) {
  arrow::Int64Builder left_id_builder(pool);
  arrow::Int64Builder right_id_builder(pool);
  arrow::Int64Builder cost_builder(pool);

  std::mt19937_64 gen(std::random_device{}());
  uint64_t count = std::stoull(argv[1]);
  uint64_t range = count * ctx->GetWorldSize();
  srand(time(NULL) + ctx->GetRank());

  arrow::Status st = left_id_builder.Reserve(count);
  st = right_id_builder.Reserve(count);
  st = cost_builder.Reserve(count);
  for (uint64_t i = 0; i < count; i++) {
    int64_t l = gen() % range;
    int64_t r = gen() % range;
    int64_t v = gen() % range;
    left_id_builder.UnsafeAppend(l);
    right_id_builder.UnsafeAppend(r);
    cost_builder.UnsafeAppend(v);
  }

  std::shared_ptr<arrow::Array> left_id_array;
  std::shared_ptr<arrow::Array> right_id_array;
  std::shared_ptr<arrow::Array> cost_array;

  st = left_id_builder.Finish(&left_id_array);
  st = right_id_builder.Finish(&right_id_array);
  st = cost_builder.Finish(&cost_array);

  std::vector<std::shared_ptr<arrow::Field>> schema_vector = {
      arrow::field("first", arrow::int64()),
      arrow::field("second", arrow::int64())};
  auto schema = std::make_shared<arrow::Schema>(schema_vector);

  left_table= arrow::Table::Make(schema,
                                 {std::move(left_id_array), cost_array});
  right_table= arrow::Table::Make(schema,
                                  {std::move(right_id_array), std::move(cost_array)});
}

