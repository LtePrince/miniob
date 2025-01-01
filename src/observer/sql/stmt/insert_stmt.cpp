/* Copyright (c) 2021OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by Wangyunlai on 2022/5/22.
//

#include "sql/stmt/insert_stmt.h"
#include "common/log/log.h"
#include "storage/db/db.h"
#include "storage/table/table.h"

InsertStmt::InsertStmt(Table *table, const std::vector<RawTuple> tuples)
    : table_(table), tuples_(tuples)
{}

RC InsertStmt::create(Db *db, const InsertSqlNode &inserts, Stmt *&stmt)
{
  const char *table_name = inserts.relation_name.c_str();
  if (nullptr == db || nullptr == table_name || inserts.tuples.empty()) {
    LOG_WARN("invalid argument. db=%p, table_name=%p, value_num=%d",
        db, table_name, static_cast<int>(inserts.tuples.size()));
    return RC::INVALID_ARGUMENT;
  }

  // check whether the table exists
  Table *table = db->find_table(table_name);
  if (nullptr == table) {
    LOG_WARN("no such table. db=%s, table_name=%s", db->name(), table_name);
    return RC::SCHEMA_TABLE_NOT_EXIST;
  }
  for(auto values : inserts.tuples){
    // check the fields number
    //const Value     *values     = inserts.values.data();
    const int        value_num  = static_cast<int>(values.size());
    const TableMeta &table_meta = table->table_meta();
    const int        field_num  = table_meta.field_num() - table_meta.sys_field_num();
    if (field_num != value_num) {
      LOG_WARN("schema mismatch. value num=%d, field num in schema=%d", value_num, field_num);
      return RC::SCHEMA_FIELD_MISSING;
    }

    //如果是日期类型，检查日期是否合法
    for (int i = table_meta.sys_field_num(); i < table_meta.field_num(); i++)
    {
      if (values[i].attr_type() == AttrType::DATES && !values[i].is_date_valid()) {
        LOG_WARN("invalid date value: %s", values[i].to_string().c_str());
        return RC::SCHEMA_FIELD_TYPE_MISMATCH;
      }
      // if(table_meta.field(i)->type() == AttrType::TEXT && values[i].attr_type() == AttrType::CHARS)
      // {
      //   Value *tmp_values = (Value*)values;
      //   tmp_values[i].set_type(AttrType::TEXT);
      //   tmp_values[i].get_text();
      //   values = (const Value*)tmp_values;
      // }
    }
  }
  // everything alright
  stmt = new InsertStmt(table, inserts.tuples);
  return RC::SUCCESS;
}
