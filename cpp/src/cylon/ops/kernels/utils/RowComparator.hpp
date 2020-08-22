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
  RowComparator(std::shared_ptr<CylonContext> ctx,
                std::shared_ptr<std::vector<std::shared_ptr<arrow::Table>>> tables,
                std::shared_ptr<arrow::Schema> schema);

  // equality
  bool operator()(const std::pair<int32_t, int64_t> &record1,
                  const std::pair<int32_t, int64_t> &record2) const;

  // hashing
  size_t operator()(const std::pair<int32_t, int64_t> &record) const;
};
}
}

#endif //CYLON_SRC_CYLON_OPS_KERNELS_UTILS_ROWCOMPARATOR_HPP_
