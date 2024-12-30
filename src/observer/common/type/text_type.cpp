#include "common/lang/comparator.h"
#include "common/log/log.h"
#include "common/type/text_type.h"
#include "common/value.h"

int TextType::compare(const Value &left, const Value &right) const
{
  ASSERT(left.attr_type() == AttrType::TEXT && (right.attr_type() == AttrType::CHARS || right.attr_type() == AttrType::TEXT), "invalid type");
  return common::compare_string(
  (void *)left.value_.pointer_value_, left.length_, (void *)right.value_.pointer_value_, right.length_);
}

RC TextType::cast_to(const Value &val, AttrType type, Value &result) const
{
  switch (type) {
    case AttrType::CHARS: {
      result.set_string(val.text_value_.str, val.length());
    }break;
    default: return RC::UNIMPLEMENTED;
  }
  return RC::SUCCESS;
}

RC TextType::set_value_from_str(Value &val, const string &data) const
{
  val.set_text(data.c_str(), data.length());
  return RC::SUCCESS;
}

int TextType::cast_cost(AttrType type)
{
  if (type == AttrType::CHARS) {
    return 0;
  }
  return INT32_MAX;
}

RC TextType::to_string(const Value &val, string &result) const
{
  stringstream ss;
  ss << val.value_.pointer_value_;
  result = ss.str();
  return RC::SUCCESS;
}