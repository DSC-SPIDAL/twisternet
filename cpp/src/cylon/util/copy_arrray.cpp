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

#include <arrow/api.h>
#include <arrow/compute/api.h>

#include <cylon/util/macros.hpp>

namespace cylon {
namespace util {

arrow::Status copy_array_by_indices(const std::vector<int64_t> &indices,
                                    const std::shared_ptr<arrow::Array> &data_array,
                                    std::shared_ptr<arrow::Array> *copied_array,
                                    arrow::MemoryPool *memory_pool) {
  CYLON_UNUSED(memory_pool);

  auto indices_data = arrow::Buffer::Wrap(indices.data(), indices.size());
  const auto &array_data = arrow::ArrayData::Make(arrow::int64(), (int64_t) indices.size(),
                                                  {nullptr, std::move(indices_data)});
  const auto &indices_array = std::make_shared<arrow::Int64Array>(array_data);

  ARROW_ASSIGN_OR_RAISE(*copied_array, arrow::compute::Take(*data_array, *indices_array))
  return arrow::Status::OK();
}

}  // namespace util
}  // namespace cylon
