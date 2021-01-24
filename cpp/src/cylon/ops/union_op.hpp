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

#ifndef CYLON_SRC_CYLON_OPS_UNION_OP_HPP_
#define CYLON_SRC_CYLON_OPS_UNION_OP_HPP_

#include "parallel_op.hpp"
#include <ops/kernels/union.hpp>


namespace cylon {
class UnionOpConfig {
 public:
  explicit UnionOpConfig(int64_t expected_rows = 100000) : expected_rows(expected_rows) {}

  int64_t expected_rows;
};

class UnionOp : public Op {
 public:
  UnionOp(const std::shared_ptr<CylonContext> &ctx,
          const std::shared_ptr<arrow::Schema> &schema,
          int id,
          const ResultsCallback &callback,
          const UnionOpConfig &config);

  ~UnionOp() override;

  bool Execute(int tag, std::shared_ptr<Table> &table) override;

  void OnParentsFinalized() override;
  bool Finalize() override;

 private:
  cylon::kernel::Union *union_kernel;
};

}
#endif //CYLON_SRC_CYLON_OPS_UNION_OP_HPP_
