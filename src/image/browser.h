#ifndef IMAGE_BROWSER_H
#define IMAGE_BROWSER_H

#include <QWidget>
#include <QTreeView>
#include <QSplitter>

#include "list.h"
#include "view.h"

namespace Image {

class Browser : public QWidget
{
    Q_OBJECT
public:
    explicit Browser(QWidget *parent = 0);

signals:

public slots:

private slots:
    void onNewDirectory(const QString&);
    void onRecentDirClicked(const QModelIndex&);

private:
    void initWidgets();
    void initLayout();

    QTreeView* recent_directories_;
    List* list_view_;
    QSplitter* v_splitter_;
};

} // namespace Image

#endif // IMAGE_BROWSER_H
