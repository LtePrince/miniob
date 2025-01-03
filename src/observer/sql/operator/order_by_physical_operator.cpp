#include "sql/operator/order_by_physical_operator.h"
#include "common/log/log.h"
#include "storage/record/record.h"
#include "storage/table/table.h"
using namespace std;
OrderByPhysicalOperator::OrderByPhysicalOperator(vector<unique_ptr<Expression>> &&expressions,
                                                 vector<bool> &&order_by_descs)
  : order_by_exprs_(std::move(expressions)), order_by_descs_(std::move(order_by_descs))
{
}
RC OrderByPhysicalOperator::open(Trx *trx)
{
  tuple_idx_ = 0;
  if (children_.empty()) {
    return RC::SUCCESS;
  }
  PhysicalOperator *child = children_[0].get();
  RC                rc    = child->open(trx);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to open child operator: %s", strrc(rc));
    return rc;
  }
  while (child->next() == RC::SUCCESS) {
    ValueListTuple tuple;
    ValueListTuple::make(*child->current_tuple(), tuple);
    tuples_.push_back(std::make_unique<ValueListTuple>(tuple));
  }
  std::sort(tuples_.begin(), tuples_.end(), [this](const std::unique_ptr<ValueListTuple> &left, const std::unique_ptr<ValueListTuple> &right) {
    for (size_t i = 0; i < order_by_exprs_.size(); i++) {
      Value left_value;
      order_by_exprs_[i]->get_value(*left, left_value);
      Value right_value;
      order_by_exprs_[i]->get_value(*right, right_value);
      if (left_value.compare(right_value) != 0) {
        return order_by_descs_[i] ? left_value.compare(right_value) > 0 : left_value.compare(right_value) < 0;
      }
    }
    return false;
  });
  return RC::SUCCESS;
}
RC OrderByPhysicalOperator::next()
{
  tuple_idx_++;
  return tuple_idx_ <= tuples_.size() ? RC::SUCCESS : RC::RECORD_EOF;
}
RC OrderByPhysicalOperator::close()
{
  if (children_.empty()) {
    return RC::SUCCESS;
  }
  children_[0]->close();
  tuples_.clear();
  tuple_idx_ = 0;
  return RC::SUCCESS;
}
Tuple *OrderByPhysicalOperator::current_tuple()
{
  return tuples_[tuple_idx_ - 1].get();
}
RC OrderByPhysicalOperator::tuple_schema(TupleSchema &schema) const
{
  for (const unique_ptr<Expression> &expression : order_by_exprs_) {
    schema.append_cell(expression->name());
  }
  return RC::SUCCESS;
}