#ifndef IMAGE_FILE_LIST_H
#define IMAGE_FILE_LIST_H

#include <QWidget>
#include <QPushButton>
#include <QTableView>
#include <QStandardItemModel>
#include <QGraphicsView>

#include "view.h"

namespace Image {

class FileTable : public QWidget
{
    Q_OBJECT
public:
    explicit FileTable(QWidget *parent = 0);

signals:

public slots:

private slots:
    /* Opens a file open dialog to choose path for loading images. */
    void onOpen();

    /* Triggers opening of an image view, for respective image user has clicked. */
    void onImageSelected(const QModelIndex&);

    /* Triggers opening of an image view, for respective image user has clicked. */
    void onImageSelected(int row);

private:
    void initWidgets();
    void initLayout();

    QStandardItemModel* model_;
    QTableView* file_view_;
    View* image_view_;
    QPushButton* open_button_;
};

} // namespace Image

#endif // IMAGE_FILE_LIST_H
