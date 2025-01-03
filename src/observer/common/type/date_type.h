#pragma once

#include "common/rc.h"
#include "common/type/data_type.h"

/**
 * @brief 日期类型
 * @details 使用 int 类型存储, 单位为天，格式为 YYYYMMDD，默认用户输入格式为YYYYMMDD
 * @ingroup DataType
 */

class DateType : public DataType
{
public:
    DateType() : DataType(AttrType::DATES) {}

    virtual ~DateType() = default;

    int compare(const Value &left, const Value &right) const override;
    RC max(const Value &left, const Value &right, Value &result) const override;
  RC min(const Value &left, const Value &right, Value &result) const override;

    RC cast_to(const Value &val, AttrType type, Value &result) const override;
    int cast_cost(AttrType type) override;

    RC set_value_from_str(Value &val, const string &data) const override;

    RC to_string(const Value &val, string &result) const override;
};