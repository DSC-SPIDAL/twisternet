#ifndef TWISTERX_SRC_JOIN_JOIN_UTILS_HPP_
#define TWISTERX_SRC_JOIN_JOIN_UTILS_HPP_

#include <arrow/api.h>
#include <map>

namespace twisterx {
namespace join {
namespace util {

arrow::Status build_final_table(std::shared_ptr<std::vector<int64_t>> left_indices,
                                std::shared_ptr<std::vector<int64_t>> right_indices,
                                const std::shared_ptr<arrow::Table> &left_tab,
                                const std::shared_ptr<arrow::Table> &right_tab,
                                std::shared_ptr<arrow::Table> *final_table,
                                arrow::MemoryPool *memory_pool);
}
}
}
#endif //TWISTERX_SRC_JOIN_JOIN_UTILS_HPP_
