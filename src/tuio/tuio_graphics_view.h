#ifndef TUIO_GRAPHICS_VIEW_H
#define TUIO_GRAPHICS_VIEW_H

#include <QGraphicsView>

class TuioGraphicsView : public QGraphicsView
{
        Q_OBJECT
    public:
        explicit TuioGraphicsView(QWidget *parent = nullptr);

    signals:

    public slots:

    protected:
        virtual void resizeEvent(QResizeEvent* e);
};

#endif // TUIO_GRAPHICS_VIEW_H
