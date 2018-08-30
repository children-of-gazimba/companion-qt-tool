#ifndef TAG_VIEW_H
#define TAG_VIEW_H

#include <QWidget>
#include <QTableView>
#include <QPushButton>
#include <QLineEdit>

#include "db/model/tag_table_model.h"

class TagView : public QWidget
{
    Q_OBJECT
public:
    explicit TagView(QWidget *parent = nullptr);

    void setTagModel(DB::Model::TagTableModel *model);

signals:

public slots:

private slots:
    void verifyTag();
    void onAddTagClicked();

private:
    void initWidgets();
    void initLayout();

    QTableView* tag_view_;
    QPushButton* add_button_;
    QLineEdit* tag_name_edit_;
    DB::Model::TagTableModel* model_;
};

#endif // TAG_VIEW_H
