#ifndef IMAGE_VIEW_H
#define IMAGE_VIEW_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsItem>

namespace Image {

class View : public QWidget
{
    Q_OBJECT
public:
    explicit View(QWidget *parent = 0);
    void setItem(QGraphicsItem*);

signals:

public slots:
    void clear();

private:
    void keyPressEvent(QKeyEvent *event);
    void initWidgets();
    void initLayout();

    QGraphicsView* view_;
};

} // namespace Image

#endif // IMAGE_VIEW_H
