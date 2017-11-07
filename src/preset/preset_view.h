#ifndef PRESET_PRESET_VIEW_H
#define PRESET_PRESET_VIEW_H

#include <QListView>
#include <QPoint>
#include <QMouseEvent>

#include "misc/standard_item_model.h"

namespace Preset {

class PresetView : public QListView
{
    Q_OBJECT
public:
    explicit PresetView(QWidget *parent = 0);
    ~PresetView();

    void setEditable(bool);
    bool getEditable();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);

signals:

public slots:

protected:
    void performDrag();

    QPoint start_pos_;
    Misc::StandardItemModel* model_;
};

} // namespace Preset

#endif // PRESET_PRESET_VIEW_H
