#ifndef IMAGE_FULL_BROWSER_H
#define IMAGE_FULL_BROWSER_H

#include <QWidget>
#include <QTreeView>
#include <QSplitter>

#include "list_browser.h"
#include "view.h"

namespace Image {

class FullBrowser : public QWidget
{
    Q_OBJECT
public:
    explicit FullBrowser(QWidget *parent = 0);

signals:

public slots:

private slots:
    void onNewDirectory(const QString&);
    void onRecentDirClicked(const QModelIndex&);

private:
    void initWidgets();
    void initLayout();

    QTreeView* recent_directories_;
    ListBrowser* list_view_;
    QSplitter* v_splitter_;
};

} // namespace Image

#endif // IMAGE_FULL_BROWSER_H
