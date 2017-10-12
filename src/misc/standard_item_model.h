#ifndef MISC_STANDARD_ITEM_MODEL_H
#define MISC_STANDARD_ITEM_MODEL_H

#include <QStandardItemModel>
#include <QSet>

namespace Misc {

/*
* Class that extends implementation of QStandardItemModel,
* for added general functionality.
*/
class StandardItemModel : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit StandardItemModel(QObject *parent = 0);
    ~StandardItemModel();

    Qt::ItemFlags flags(const QModelIndex &index) const;

    void setColumnEditable(int col, bool is_editable);
    bool getColumnEditable(int col) const;

signals:

public slots:

private:
    QSet<int> none_editable_columns_;

};

} // Misc

#endif // MISC_STANDARD_ITEM_MODEL_H
