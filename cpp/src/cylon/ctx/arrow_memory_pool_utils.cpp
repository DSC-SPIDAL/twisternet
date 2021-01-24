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

#include "arrow_memory_pool_utils.hpp"

arrow::Status cylon::ArrowStatus(cylon::Status status) {
  return arrow::Status(static_cast<arrow::StatusCode>(status.get_code()), status.get_msg());
}
arrow::MemoryPool *cylon::ToArrowPool(std::shared_ptr<cylon::CylonContext> &ctx) {
  if (ctx->GetMemoryPool() == nullptr) {
    return arrow::default_memory_pool();
  } else {
    return new ProxyMemoryPool(ctx->GetMemoryPool());
  }
}
