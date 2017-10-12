#include "drop_group_box.h"

namespace Misc {

DropGroupBox::DropGroupBox(QString const& title, QWidget *parent)
    : QGroupBox(title, parent)
{
    setAcceptDrops(true);
}

void DropGroupBox::dragEnterEvent(QDragEnterEvent *event)
{
    DropGroupBox *source = qobject_cast<DropGroupBox*>(event->source());
    if (event->source() && source != this) {
        event->setDropAction(Qt::CopyAction);
        event->accept();
    }
}

void DropGroupBox::dragMoveEvent(QDragMoveEvent *event)
{
    DropGroupBox *source = qobject_cast<DropGroupBox*>(event->source());
    if (event->source() && source != this) {
        event->setDropAction(Qt::CopyAction);
        event->accept();
    }
}

void DropGroupBox::dropEvent(QDropEvent *event)
{
    DropGroupBox *source = qobject_cast<DropGroupBox*>(event->source());
    if (event->source() && source != this) {
        emit receivedDrop(event->source(), event->mimeData());
        event->setDropAction(Qt::CopyAction);
        event->accept();
    }
}

} // namespace Misc
