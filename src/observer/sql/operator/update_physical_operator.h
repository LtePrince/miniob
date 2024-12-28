#pragma once

#include "sql/operator/physical_operator.h"

class Trx;
class UpdateStmt;

/**
 * @brief 物理算子，修改
 * @ingroup PhysicalOperator
 */
class UpdatePhysicalOperator : public PhysicalOperator
{
public:
  UpdatePhysicalOperator(Table *table, Value *values, std::string field) : table_(table), values_(values), field_(field)
  {}

  virtual ~UpdatePhysicalOperator() = default;

  PhysicalOperatorType type() const override { return PhysicalOperatorType::UPDATE; }
  
  RC open(Trx *trx) override;
  RC next() override;
  RC close() override;

  Tuple *current_tuple() override { return nullptr; }
private:
  Table *table_ = nullptr;
  Value *values_ = nullptr;
  std::string field_;
  Trx *trx_   = nullptr;
};
