#ifndef TUIO_CONTROL_PANEL_H
#define TUIO_CONTROL_PANEL_H

#include <QObject>
#include <QWidget>

#include <QMap>
#include <QPushButton>
#include <QLineEdit>

#include <QGraphicsEllipseItem>
#include <QGraphicsRectItem>

#include "tuio_graphics_view.h"
#include "udp_client.h"
#include "tuio_handler.h"

#include "image/view.h"


class TuioControlPanel : public QWidget
{
        Q_OBJECT
    public:
        explicit TuioControlPanel(QWidget *parent = nullptr);
        virtual ~TuioControlPanel();

        void setImageView(Image::View *view);

    public slots:
        void onCursorEvent(QMap<int, QTuioCursor> active_cursors, QVector<QTuioCursor> dead_cursors);
        void onTokenEvent(QMap<int, QTuioToken> active_cursors, QVector<QTuioToken> dead_cursors);

    protected slots:
        void onImageInteractiveEnabled(bool enabled);
        void onNewHostName();

    private:
        void initWidgets();
        void initLayout();

        void updateInteractiveImageTokens(const QMap<int, QTuioToken> &active_token);

    private:
        QMap<int, QGraphicsEllipseItem*> marker_list_;
        QMap<int, QGraphicsRectItem*> token_list_;

        Image::View *image_view_;
        bool image_interactive_;

        QLineEdit* host_name_;
        QPushButton* host_submit_;
        TuioGraphicsView *view_;
        TuioHandler *tuio_handler_;
};

#endif // TUIO_CONTROL_PANEL_H
