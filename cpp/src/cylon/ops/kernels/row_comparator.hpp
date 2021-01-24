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

#ifndef CYLON_SRC_CYLON_OPS_KERNELS_UTILS_ROWCOMPARATOR_HPP_
#define CYLON_SRC_CYLON_OPS_KERNELS_UTILS_ROWCOMPARATOR_HPP_

#include <arrow/table.h>
#include <arrow/arrow_comparator.hpp>
#include <arrow/arrow_partition_kernels.hpp>
#include <ctx/cylon_context.hpp>
#include <ctx/arrow_memory_pool_utils.hpp>

namespace cylon {
namespace kernel {
class RowComparator {
 private:
  std::shared_ptr<std::vector<std::shared_ptr<arrow::Table>>> tables;
  std::shared_ptr<cylon::TableRowComparator> comparator;
  std::shared_ptr<cylon::RowHashingKernel> row_hashing_kernel;

 public:
  RowComparator(const std::shared_ptr<CylonContext> &ctx,
                const std::shared_ptr<std::vector<std::shared_ptr<arrow::Table>>> &tables,
                const std::shared_ptr<arrow::Schema> &schema);

  // equality
  bool operator()(const std::pair<int32_t, int64_t> &record1,
                  const std::pair<int32_t, int64_t> &record2) const;

  // hashing
  size_t operator()(const std::pair<int32_t, int64_t> &record) const;
};
}
}

#endif //CYLON_SRC_CYLON_OPS_KERNELS_UTILS_ROWCOMPARATOR_HPP_
