#include "common/lang/comparator.h"
#include "common/log/log.h"
#include "common/type/date_type.h"
#include "common/value.h"

int DateType::compare(const Value &left, const Value &right) const
{
  ASSERT(left.attr_type() == AttrType::DATES && right.attr_type() == AttrType::DATES, "invalid type");
  return common::compare_int((void *)&left.value_.int_value_, (void *)&right.value_.int_value_);
}

RC DateType::max(const Value &left, const Value &right, Value &result) const
{
  int cmp = common::compare_int((void *)&left.value_.int_value_, (void *)&right.value_.int_value_);
  if (cmp > 0) {
    result.set_int(left.value_.int_value_);
  } else {
    result.set_int(right.value_.int_value_);
  }
  return RC::SUCCESS;
}
RC DateType::min(const Value &left, const Value &right, Value &result) const
{
  int cmp = common::compare_int((void *)&left.value_.int_value_, (void *)&right.value_.int_value_);
  if (cmp < 0) {
    result.set_int(left.value_.int_value_);
  } else {
    result.set_int(right.value_.int_value_);
  }
  return RC::SUCCESS;
}

RC DateType::cast_to(const Value &val, AttrType type, Value &result) const
{
  // 日期类型不支持转换
  switch (type) {
    default: return RC::UNIMPLEMENTED;
  }
  return RC::SUCCESS;
}
int DateType::cast_cost(AttrType type)
{
  if (type == AttrType::DATES) {
    return 0;
  }
  return INT32_MAX;
}

RC DateType::set_value_from_str(Value &val, const string &data) const
{
  std::string date = data.substr(0, 4) + data.substr(5, 2) + data.substr(8, 2);
  val.set_date(atoi(date.c_str()));
  return RC::SUCCESS;
}

RC DateType::to_string(const Value &val, string &result) const
{
  int year  = val.value_.int_value_ / 10000;
  int month = (val.value_.int_value_ / 100) % 100;
  int day   = val.value_.int_value_ % 100;
  result    = std::to_string(year) + "-" + (month < 10 ? "0" : "") + std::to_string(month) + "-" + (day < 10 ? "0" : "") + std::to_string(day);
  return RC::SUCCESS;
}