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

#ifndef CYLON_SRC_CYLON_COMM_UCXCOMMUNICATOR_H_
#define CYLON_SRC_CYLON_COMM_UCXCOMMUNICATOR_H_

#include "../comm_config.hpp"
#include "../communicator.hpp"

namespace cylon {
namespace net {

class UCXConfig : public CommConfig {
  // TODO Sandeepa UCX has a context? Init as config? Later, just get he base running
  void DummyConfig(int dummy);

  int GetDummyConfig();

  CommType Type() override;

 public:
  static std::shared_ptr<UCXConfig> Make();
};

class UCXCommunicator : public Communicator {
 public:
  void Init(const std::shared_ptr<CommConfig> &config) override;
  Channel *CreateChannel() override;
  int GetRank() override;
  int GetWorldSize() override;
  void Finalize() override;
  void Barrier() override;
  CommType GetCommType() override;
};
}
}
#endif //CYLON_SRC_CYLON_COMM_UCXCOMMUNICATOR_H_
