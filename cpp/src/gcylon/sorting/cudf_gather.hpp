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

#ifndef GCYLON_CUDF_GATHER_HPP
#define GCYLON_CUDF_GATHER_HPP

#include <cylon/ctx/cylon_context.hpp>
#include <cylon/status.hpp>
#include <cylon/net/buffer.hpp>
#include <gcylon/net/cudf_serialize.hpp>

namespace gcylon {

class CudfTableGatherer {
public:
    CudfTableGatherer(std::shared_ptr<cylon::CylonContext> ctx,
                  const int gather_root);

    cylon::Status Gather(cudf::table_view &tv,
                         bool gather_from_root,
                         std::vector<std::unique_ptr<cudf::table>> &gathered_tables);

    bool AmIRoot();


private:
    std::shared_ptr<cylon::CylonContext> ctx_;
    const int root_;

    std::vector<std::vector<int32_t>> bufferSizesPerTable(int32_t *all_buffer_sizes, int number_of_buffers);
};


} // end of namespace gcylon

#endif //GCYLON_CUDF_GATHER_HPP
