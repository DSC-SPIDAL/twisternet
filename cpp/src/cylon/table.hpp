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

#ifndef CYLON_SRC_IO_TABLE_H_
#define CYLON_SRC_IO_TABLE_H_

#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <glog/logging.h>
#include "io/csv_read_config.hpp"

#include "status.hpp"
#include "util/uuid.hpp"
#include "column.hpp"
#include "join/join_config.hpp"
#include "arrow/arrow_join.hpp"
#include "join/join.hpp"
#include "io/csv_write_config.hpp"
#include "row.hpp"

namespace cylon {

/**
 * Table provides the main API for using cylon for data processing.
 */
class Table {
 public:
  /**
   * Tables can only be created using the factory methods, so the constructor is private
   */
  Table(std::shared_ptr<arrow::Table> &tab, std::shared_ptr<cylon::CylonContext> &ctx)
      : ctx(ctx), table_(tab),
        columns_(std::vector<std::shared_ptr<Column>>(tab->num_columns())) {
    const int num_cols = table_->num_columns();
    for (int i = 0; i < num_cols; i++) {
      const auto f = table_->field(i);
      columns_.at(i) =
          cylon::Column::Make(f->name(), cylon::tarrow::ToCylonType(f->type()), table_->column(i));
    }
  }

  Table(std::shared_ptr<arrow::Table> &tab, std::shared_ptr<cylon::CylonContext> &ctx,
        const std::vector<std::shared_ptr<Column>> &cols)
      : ctx(ctx), table_(tab) {
    this->columns_ = cols;
  }

  virtual ~Table();

  /**
   * Create a table from an arrow table,
   * @param table
   * @return
   */
  static Status FromArrowTable(std::shared_ptr<cylon::CylonContext> &ctx,
                               std::shared_ptr<arrow::Table> &table,
                               std::shared_ptr<Table> &tableOut);

  /**
   * Create a table from cylon columns
   * @param ctx
   * @param columns
   * @param tableOut
   * @return
   */
  static Status FromColumns(std::shared_ptr<cylon::CylonContext> &ctx,
                            std::vector<std::shared_ptr<Column>> &&columns,
                            std::shared_ptr<Table> &tableOut);

  /**
   * Write the table as a CSV
   * @param path file path
   * @return the status of the operation
   */
  Status WriteCSV(const std::string &path,
                  const cylon::io::config::CSVWriteOptions &options = cylon::io::config::CSVWriteOptions());

  /**
   * Create a arrow table from this data structure
   * @param output arrow table
   * @return the status of the operation
   */
  Status ToArrowTable(std::shared_ptr<arrow::Table> &output);

  /**
   * Print the col range and row range
   * @param col1 start col
   * @param col2 end col
   * @param row1 start row
   * @param row2 end row
   * @param out the stream
   * @param delimiter delimiter between values
   * @param use_custom_header custom header
   * @param headers the names of custom header
   * @return true if print is successful
   */
  Status PrintToOStream(
      int col1,
      int col2,
      int row1,
      int row2,
      std::ostream &out,
      char delimiter = ',',
      bool use_custom_header = false,
      const std::vector<std::string> &headers = {});

  /*END OF TRANSFORMATION FUNCTIONS*/

  /**
   * Get the number of columns in the table
   * @return numbre of columns
   */
  int32_t Columns();

  /**
   * Get the number of rows in this table
   * @return number of rows in the table
   */
  int64_t Rows();

  /**
   * Print the complete table
   */
  void Print();

  /**
   * Print the table from row1 to row2 and col1 to col2
   * @param row1 first row to start printing (including)
   * @param row2 end row to stop printing (including)
   * @param col1 first column to start printing (including)
   * @param col2 end column to stop printing (including)
   */
  void Print(int row1, int row2, int col1, int col2);

  /**
   * Get the underlying arrow table
   * @return the arrow table
   */
  std::shared_ptr<arrow::Table> get_table();

  /**
   * Clears the table
   */
  void Clear();

  /**
   * Returns the cylon Context
   * @return
   */
  std::shared_ptr<cylon::CylonContext> GetContext();

  /**
   * Get column names of the table
   * @return vector<string>
   */
  std::vector<std::string> ColumnNames();

  /**
   * Set to true to free the memory of this table when it is not needed
   */
  void retainMemory(bool retain) {
    retain_ = retain;
  }

  bool IsRetain() const;

  /**
   * Get the i'th column from the table
   * @param index
   * @return
   */
  std::shared_ptr<Column> GetColumn(int32_t index) const;

  /**
   * Get the column vector of the table
   * @return
   */
  std::vector<std::shared_ptr<cylon::Column>> GetColumns() const;

 private:
  /**
   * Every table should have an unique id
   */
  std::string id_;
  std::shared_ptr<cylon::CylonContext> ctx;
  std::shared_ptr<arrow::Table> table_;
  bool retain_ = true;
  std::vector<std::shared_ptr<cylon::Column>> columns_;
};

/**
   * Create a table by reading a csv file
   * @param path file path
   * @return a pointer to the table
   */
Status FromCSV(std::shared_ptr<cylon::CylonContext> &ctx, const std::string &path,
               std::shared_ptr<Table> &tableOut,
               const cylon::io::config::CSVReadOptions &options = cylon::io::config::CSVReadOptions());

/**
 * Read multiple CSV files into multiple tables. If threading is enabled, the tables will be read
 * in parallel
 * @param ctx
 * @param paths
 * @param tableOuts
 * @param options
 * @return
 */
Status FromCSV(std::shared_ptr<cylon::CylonContext> &ctx, const std::vector<std::string> &paths,
               const std::vector<std::shared_ptr<Table> *> &tableOuts,
               io::config::CSVReadOptions options = cylon::io::config::CSVReadOptions());

/**
   * Merge the set of tables to create a single table
   * @param tables
   * @return new merged table
   */
Status Merge(std::shared_ptr<cylon::CylonContext> &ctx,
             const std::vector<std::shared_ptr<cylon::Table>> &tables,
             std::shared_ptr<Table> &tableOut);

/**
   * Do the join with the right table
   * @param right the right table
   * @param joinConfig the join configurations
   * @param output the final table
   * @return success
   */
Status Join(std::shared_ptr<Table> &left, std::shared_ptr<Table> &right,
            cylon::join::config::JoinConfig join_config,
            std::shared_ptr<Table> &output);

/**
 * Similar to local join, but performs the join in a distributed fashion
 * @param right
 * @param join_config
 * @param output
 * @return <cylon::Status>
 */
Status DistributedJoin(std::shared_ptr<Table> &left, std::shared_ptr<Table> &right,
                       cylon::join::config::JoinConfig join_config,
                       std::shared_ptr<Table> &output);

/**
 * Performs union with the passed table
 * @param other right table
 * @param output output table
 * @return <cylon::Status>
 */
Status Union(std::shared_ptr<Table> &first, std::shared_ptr<Table> &second,
             std::shared_ptr<Table> &output);

/**
 * Similar to local union, but performs the union in a distributed fashion
 * @param other
 * @param output
 * @return
 */
Status DistributedUnion(std::shared_ptr<Table> &left, std::shared_ptr<Table> &right,
                        std::shared_ptr<Table> &out);

/**
 * Performs subtract/difference with the passed table
 * @param right right table
 * @param output output table
 * @return <cylon::Status>
 */
Status Subtract(std::shared_ptr<Table> &first,
                std::shared_ptr<Table> &second, std::shared_ptr<Table> &out);

/**
 * Similar to local subtract/difference, but performs in a distributed fashion
 * @param other
 * @param output
 * @return
 */
Status DistributedSubtract(std::shared_ptr<Table> &left, std::shared_ptr<Table> &right,
                           std::shared_ptr<Table> &out);

/**
 * Performs intersection with the passed table
 * @param other right table
 * @param output output table
 * @return <cylon::Status>
 */
Status Intersect(std::shared_ptr<Table> &first,
                 std::shared_ptr<Table> &second, std::shared_ptr<Table> &output);

/**
 * Similar to local intersection, but performs in a distributed fashion
 * @param other
 * @param output
 * @return
 */
Status DistributedIntersect(std::shared_ptr<Table> &left, std::shared_ptr<Table> &right,
                            std::shared_ptr<Table> &out);

Status Shuffle(std::shared_ptr<cylon::Table> &table, const std::vector<int> &hash_columns,
               std::shared_ptr<cylon::Table> &output);

/**
   * Partition the table based on the hash
   * @param hash_columns the columns use for has
   * @param no_of_partitions number partitions
   * @return new set of tables each with the new partition
   */
Status HashPartition(std::shared_ptr<cylon::Table> &table,
                     const std::vector<int> &hash_columns,
                     int no_of_partitions,
                     std::unordered_map<int, std::shared_ptr<cylon::Table>> *output);

/**
 * Sort the table according to the given column, this is a local sort (if the table has chunked columns, they will
 * be merged in the output table)
 * @param sort_column
 * @return new table sorted according to the sort column
 */
Status Sort(std::shared_ptr<cylon::Table> &table, int sort_column, std::shared_ptr<Table> &output);

/**
 * Filters out rows based on the selector function
 * @param selector lambda function returning a bool
 * @param output
 * @return
 */
Status Select(std::shared_ptr<cylon::Table> &table, const std::function<bool(cylon::Row)> &selector, std::shared_ptr<Table> &output);

/**
 * Creates a View of an existing table by dropping one or more columns
 * @param project_columns
 * @param output
 * @return
 */
Status Project(std::shared_ptr<cylon::Table> &table, const std::vector<int64_t> &project_columns, std::shared_ptr<Table> &output);

}  // namespace cylon



#endif //CYLON_SRC_IO_TABLE_H_
