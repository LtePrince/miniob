/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
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

#include "sql/stmt/update_stmt.h"
#include "storage/db/db.h"
#include "common/log/log.h"
#include "storage/table/table.h"
#include "sql/stmt/filter_stmt.h"

UpdateStmt::UpdateStmt(Table *table, std::string field_name, Value *values, FilterStmt *filter_stmt)
    : table_(table), values_(values), field_name_(field_name), filter_stmt_(filter_stmt)
{}

UpdateStmt::~UpdateStmt()
{
  delete values_;
  values_ = nullptr;
  if(nullptr != filter_stmt_)
  {
    delete filter_stmt_;
    filter_stmt_ = nullptr;
  }
}

RC UpdateStmt::create(Db *db, const UpdateSqlNode &update, Stmt *&stmt)
{
  // TODO
  const char *table_name = update.relation_name.c_str();
  const char *attribute_name = update.attribute_name.c_str();
  if (nullptr == db || nullptr == table_name || attribute_name == nullptr) {
    LOG_WARN("invalid argument. db=%p, table_name=%p, attribute_name=%p",
        db, table_name, attribute_name);
    return RC::INVALID_ARGUMENT;
  }

  Table *table = db->find_table(table_name);
  if (table == nullptr) {
    LOG_WARN("no such table. db=%s, table_name=%s", db->name(), table_name);
    return RC::SCHEMA_TABLE_NOT_EXIST;
  }

  const TableMeta &table_meta = table->table_meta();
  const FieldMeta * field_meta = table_meta.field(attribute_name);
  if (field_meta == nullptr) {
    LOG_WARN("no such attribute. db=%s, table_name=%s, attribute=%s", db->name(), table_name, attribute_name);
    return RC::SCHEMA_FIELD_NOT_EXIST;
  }

  if(field_meta->type() != AttrType::TEXT && update.value.attr_type() != AttrType::CHARS)
  {
    if (field_meta->type() != update.value.attr_type()) {
    LOG_WARN("field type mismatch. table=%s, field=%s, field type=%d, value_type=%d",
          table_name, field_meta->name(), field_meta->type(), update.value.attr_type());
    return RC::SCHEMA_FIELD_TYPE_MISMATCH;
    }
  }

  if (field_meta->type() == AttrType::DATES && !update.value.is_date_valid())
  {
    LOG_WARN("invalid date value: %s", update.value.to_string().c_str());
    return RC::SCHEMA_FIELD_TYPE_MISMATCH;
  }

  std::unordered_map<string, Table *> table_map;
  table_map.insert(std::pair<std::string, Table *>(std::string(table_name), table));

  FilterStmt *filter_stmt = nullptr;
  RC          rc          = FilterStmt::create(db,
      table,
      &table_map,
      update.conditions.data(),
      static_cast<int>(update.conditions.size()),
      filter_stmt);
  if (rc != RC::SUCCESS) {
    LOG_WARN("cannot construct filter stmt");
    return rc;
  }

  stmt = new UpdateStmt(table, update.attribute_name ,new Value(update.value), filter_stmt);

  return RC::SUCCESS;
}
