#include "standard_item_model.h"

namespace Misc {

StandardItemModel::StandardItemModel(QObject *parent)
    : QStandardItemModel(parent)
    , none_editable_columns_()
{}

StandardItemModel::~StandardItemModel()
{}

Qt::ItemFlags StandardItemModel::flags(const QModelIndex &index) const
{
    if(!none_editable_columns_.contains(index.column()))
        return QStandardItemModel::flags(index);
    else
        return QStandardItemModel::flags(index) & (~Qt::ItemIsEditable);
}

void StandardItemModel::setColumnEditable(int col, bool is_editable)
{
    if(none_editable_columns_.contains(col) && is_editable)
         none_editable_columns_.remove(col);
    else if(!none_editable_columns_.contains(col) && !is_editable)
        none_editable_columns_.insert(col);
}

bool StandardItemModel::getColumnEditable(int col) const
{
    return !none_editable_columns_.contains(col);
}

} // namespace Misc
