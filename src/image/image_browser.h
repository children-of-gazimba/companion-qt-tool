#ifndef IMAGE_BROWSER_H
#define IMAGE_BROWSER_H

#include <QWidget>
#include <QTreeView>
#include <QSplitter>

#include "thumbnail_list.h"
#include "image_canvas.h"
#include "image_display_widget.h"
#include "db/model/image_dir_table_model.h"

class ImageBrowser : public QWidget
{
    Q_OBJECT
public:
    explicit ImageBrowser(QWidget *parent = 0);
    virtual ~ImageBrowser();

    void setImageDirTableModel(ImageDirTableModel*);
    ImageDirTableModel* getImageDirTableModel();

    ImageCanvas *getCanvas() const;
    ImageDisplayWidget* getDisplayWidget() const;

signals:

public slots:

private slots:
    void onNewDirectory(const QString&);
    void onRecentDirClicked(const QModelIndex&);

private:
    void initWidgets();
    void initLayout();

    QTreeView* recent_directories_;
//    List* list_view_;
    ThumbnailList *list_view_;
    QSplitter* v_splitter_;
    ImageDirTableModel* model_;
};

#endif // IMAGE_BROWSER_H
