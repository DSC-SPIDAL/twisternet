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

#include "common/test_header.hpp"
#include "test_utils.hpp"
#include <cylon/table.hpp>

namespace cylon {
namespace test {

static void verify_test(std::vector<std::vector<std::string>>& expected, std::shared_ptr<Table>& output) {
    std::stringstream ss;
    output->PrintToOStream(ss);
    std::string s;
    int i = 0;
    while(ss>>s) {
        REQUIRE(s == expected[RANK][i++]);
    }
    REQUIRE(i == expected[RANK].size());
}

TEST_CASE("Repartition with custom order", "[repartition]") {
    std::string path1 = "../data/input/repartition_" + std::to_string(RANK) +".csv";
    std::shared_ptr<Table> table1;

    auto read_options = io::config::CSVReadOptions().UseThreads(false);

    CHECK_CYLON_STATUS(FromCSV(ctx, std::vector<std::string>{path1},
                    std::vector<std::shared_ptr<Table> *>{&table1},
                            read_options));

    SECTION("even") {
        bool result;
        CHECK_CYLON_STATUS(DistributedEquals(table1, table1, result));
        REQUIRE(result == true);
        std::shared_ptr<Table> output;
        std::vector<int64_t> rows_per_partition = {3, 3, 3, 3};
        std::vector<int> receive_build_rank_order = {0, 1, 2, 3};
        Repartition(table1, rows_per_partition, receive_build_rank_order, &output);
        REQUIRE(output->Rows() == 3);
        
        std::vector<std::vector<std::string>> expected = {
            {"0,1", "1,2", "3,4", "5,6"},
            {"0,1", "7,8", "9,10", "11,12"},
            {"0,1", "13,14", "15,16", "17,18"},
            {"0,1", "19,20", "21,22", "23,24"},
        };

        verify_test(expected, output);
    }

    SECTION("uneven") {
        bool result;
        CHECK_CYLON_STATUS(DistributedEquals(table1, table1, result));
        REQUIRE(result == true);
        std::shared_ptr<Table> output;
        std::vector<int64_t> rows_per_partition = {1, 2, 3, 6};
        std::vector<int> receive_build_rank_order = {0, 1, 2, 3};
        Repartition(table1, rows_per_partition, receive_build_rank_order, &output);
        REQUIRE(output->Rows() == rows_per_partition[RANK]);
        
        std::vector<std::vector<std::string>> expected = {
            {"0,1", "1,2", },
            {"0,1", "3,4", "5,6"},
            {"0,1", "7,8", "9,10", "11,12"},
            {"0,1", "13,14", "15,16", "17,18", "19,20", "21,22", "23,24"},
        };

        verify_test(expected, output);
    }

    SECTION("uneven 2") {
        bool result;
        CHECK_CYLON_STATUS(DistributedEquals(table1, table1, result));
        REQUIRE(result == true);
        std::shared_ptr<Table> output;
        std::vector<int64_t> rows_per_partition = {6, 3, 1, 2};
        std::vector<int> receive_build_rank_order = {0, 1, 2, 3};
        Repartition(table1, rows_per_partition, receive_build_rank_order, &output);
        REQUIRE(output->Rows() == rows_per_partition[RANK]);
        
        std::vector<std::vector<std::string>> expected = {
            {"0,1", "1,2", "3,4", "5,6", "7,8", "9,10", "11,12"},
            {"0,1", "13,14", "15,16", "17,18"},
            {"0,1", "19,20"},
            {"0,1", "21,22", "23,24"},
        };

        verify_test(expected, output);
    }

    SECTION("very uneven") {
        bool result;
        CHECK_CYLON_STATUS(DistributedEquals(table1, table1, result));
        REQUIRE(result == true);
        std::shared_ptr<Table> output;
        std::vector<int64_t> rows_per_partition = {1, 1, 1, 9};
        std::vector<int> receive_build_rank_order = {0, 1, 2, 3};
        Repartition(table1, rows_per_partition, receive_build_rank_order, &output);
        REQUIRE(output->Rows() == rows_per_partition[RANK]);
        
        std::vector<std::vector<std::string>> expected = {
            {"0,1", "1,2"},
            {"0,1", "3,4"},
            {"0,1", "5,6"},
            {"0,1", "7,8", "9,10", "11,12", "13,14", "15,16", "17,18", "19,20", "21,22", "23,24"},
        };

        verify_test(expected, output);
    }

    SECTION("custom order") {
        bool result;
        CHECK_CYLON_STATUS(DistributedEquals(table1, table1, result));
        REQUIRE(result == true);
        std::shared_ptr<Table> output;
        std::vector<int64_t> rows_per_partition = {3, 3, 3, 3};
        std::vector<int> receive_build_rank_order = {3, 2, 1, 0};
        Repartition(table1, rows_per_partition, receive_build_rank_order, &output);
        REQUIRE(output->Rows() == 3);
        
        std::vector<std::vector<std::string>> expected = {
            {"0,1", "19,20", "21,22", "23,24"},
            {"0,1", "13,14", "15,16", "17,18"},
            {"0,1", "7,8", "9,10", "11,12"},
            {"0,1", "1,2", "3,4", "5,6"},
        };

        verify_test(expected, output);
    }

    SECTION("custom order 2") {
        bool result;
        CHECK_CYLON_STATUS(DistributedEquals(table1, table1, result));
        REQUIRE(result == true);
        std::shared_ptr<Table> output;
        std::vector<int64_t> rows_per_partition = {4, 2, 3, 3};
        std::vector<int> receive_build_rank_order = {3, 0, 1, 2};
        Repartition(table1, rows_per_partition, receive_build_rank_order, &output);

        int mp[4];
        for(int i = 0; i < receive_build_rank_order.size(); i++) {
            mp[receive_build_rank_order[i]] = i;
        }

        REQUIRE(output->Rows() == rows_per_partition[mp[RANK]]);
        
        std::vector<std::vector<std::string>> expected = {
            {"0,1", "9,10", "11,12"},
            {"0,1", "13,14", "15,16", "17,18"},
            {"0,1", "19,20", "21,22", "23,24"},
            {"0,1", "1,2", "3,4", "5,6", "7,8"},
        };

        verify_test(expected, output);
    }
}

TEST_CASE("Repartition with rank order", "[repartition]") {
    std::string path1 = "../data/input/repartition_" + std::to_string(RANK) +".csv";
    std::shared_ptr<Table> table1;

    auto read_options = io::config::CSVReadOptions().UseThreads(false);

    CHECK_CYLON_STATUS(FromCSV(ctx, std::vector<std::string>{path1},
                    std::vector<std::shared_ptr<Table> *>{&table1},
                            read_options));

    SECTION("even") {
        bool result;
        CHECK_CYLON_STATUS(DistributedEquals(table1, table1, result));
        REQUIRE(result == true);
        std::shared_ptr<Table> output;
        std::vector<int64_t> rows_per_partition = {3, 3, 3, 3};
        Repartition(table1, rows_per_partition, &output);
        REQUIRE(output->Rows() == 3);
        
        std::vector<std::vector<std::string>> expected = {
            {"0,1", "1,2", "3,4", "5,6"},
            {"0,1", "7,8", "9,10", "11,12"},
            {"0,1", "13,14", "15,16", "17,18"},
            {"0,1", "19,20", "21,22", "23,24"},
        };

        verify_test(expected, output);
    }

    SECTION("uneven") {
        bool result;
        CHECK_CYLON_STATUS(DistributedEquals(table1, table1, result));
        REQUIRE(result == true);
        std::shared_ptr<Table> output;
        std::vector<int64_t> rows_per_partition = {1, 2, 6, 3};
        Repartition(table1, rows_per_partition, &output);
        REQUIRE(output->Rows() == rows_per_partition[RANK]);
        
        std::vector<std::vector<std::string>> expected = {
            {"0,1", "1,2", },
            {"0,1", "3,4", "5,6"},
            {"0,1", "7,8", "9,10", "11,12", "13,14", "15,16", "17,18"},
            {"0,1", "19,20", "21,22", "23,24"},
        };

        verify_test(expected, output);
    }

    SECTION("very uneven") {
        bool result;
        CHECK_CYLON_STATUS(DistributedEquals(table1, table1, result));
        REQUIRE(result == true);
        std::shared_ptr<Table> output;
        std::vector<int64_t> rows_per_partition = {1, 1, 9, 1};
        Repartition(table1, rows_per_partition, &output);
        REQUIRE(output->Rows() == rows_per_partition[RANK]);
        
        std::vector<std::vector<std::string>> expected = {
            {"0,1", "1,2"},
            {"0,1", "3,4"},
            {"0,1", "5,6", "7,8", "9,10", "11,12", "13,14", "15,16", "17,18", "19,20", "21,22"},
            {"0,1", "23,24"},
        };

        verify_test(expected, output);
    }
}

}
}