#ifndef MISC_DROP_GROUP_BOX_H
#define MISC_DROP_GROUP_BOX_H

#include <QGroupBox>

#include <QPoint>
#include <QMouseEvent>
#include <QMimeData>

namespace Misc {

class DropGroupBox : public QGroupBox
{
    Q_OBJECT
public:
    explicit DropGroupBox(QString const& title = "", QWidget *parent = 0);

    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);

signals:
    void receivedDrop(QObject* source, const QMimeData*);

public slots:
};

} // namespace Misc

#endif // MISC_DROP_GROUP_BOX_H
