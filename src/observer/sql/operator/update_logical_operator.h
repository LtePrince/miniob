#pragma once

#include <vector>

#include "sql/operator/logical_operator.h"
#include "sql/parser/parse_defs.h"

/**
 * @brief 修改逻辑算子
 * @ingroup LogicalOperator
 */
class UpdateLogicalOperator : public LogicalOperator
{
public:
  UpdateLogicalOperator(Table *table, Value *values, std::string field_name);
  virtual ~UpdateLogicalOperator() = default;

  LogicalOperatorType type() const override { return LogicalOperatorType::UPDATE; }

  Table                    *table() const { return table_; }
  Value*       &values() { return values_; }
  std::string field_name() const { return field_name_; }

private:
  Table             *table_ = nullptr;
  Value             *values_;
  std::string field_name_;
};