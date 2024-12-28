#include "sql/operator/update_logical_operator.h"

UpdateLogicalOperator::UpdateLogicalOperator(Table *table, Value *values, std::string field_name) : table_(table), values_(values), field_name_(field_name)
{}