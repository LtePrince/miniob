#include "sql/operator/update_physical_operator.h"
#include "common/log/log.h"
#include "storage/table/table.h"
#include "storage/trx/trx.h"

RC UpdatePhysicalOperator::open(Trx *trx)
{
  if (children_.empty()) {
  	return RC::SUCCESS;
  }

	std::unique_ptr<PhysicalOperator> &child = children_[0];

  RC rc = child->open(trx);
  if (rc != RC::SUCCESS) {
			LOG_WARN("failed to open child operator: %s", strrc(rc));
			return rc;
	}

	trx_ = trx;

	std::vector<std::function<RC()>> updateIndexTasks;
	while (OB_SUCC(rc = child->next())) {
		Tuple *tuple = child->current_tuple();
		if (nullptr == tuple) {
		LOG_WARN("failed to get current record: %s", strrc(rc));
		return rc;
		}

    RowTuple *row_tuple = static_cast<RowTuple *>(tuple);
    //Record   &record    = row_tuple->record();
		//const TableMeta table_meta = table_->table_meta();
		//const FieldMeta * field_meta = table_meta.field(field_.c_str());

		table_->visit_record(row_tuple->record().rid(), [this, row_tuple, trx, &updateIndexTasks](Record &record) {
			row_tuple->set_value_at(field_, values_, record.data());
			updateIndexTasks.push_back([this, record] {
				return table_->update_index(record, field_);
      });
			return true;
		});
		//trx->visit_record(table_, record, ReadWriteMode::READ_WRITE);
		//row_tuple->set_value_at(field_meta, values_);
		
  }
	child->close();
  for (auto task : updateIndexTasks) {
    rc = task();
    if (OB_FAIL(rc)) {
      LOG_WARN("update index failed: %s", strrc(rc));
      return rc;
    }
  }

	return RC::SUCCESS;
}

RC UpdatePhysicalOperator::next()
{
  return RC::RECORD_EOF;
}

RC UpdatePhysicalOperator::close()
{
	return RC::SUCCESS;
}