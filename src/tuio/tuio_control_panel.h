#ifndef TUIO_CONTROL_PANEL_H
#define TUIO_CONTROL_PANEL_H

#include <QObject>
#include <QWidget>
#include <QMap>
#include <QPushButton>
#include <QLineEdit>
#include <QGraphicsEllipseItem>
#include <QGraphicsRectItem>
#include <QTableView>
#include <QLabel>
#include <QHostAddress>

#include "tuio_graphics_view.h"
#include "udp_client.h"
#include "tuio_model_handler.h"

#include "image/view.h"
#include "tracking/tracker.h"


class TuioControlPanel : public QWidget
{
    Q_OBJECT
public:
    explicit TuioControlPanel(QWidget *parent = nullptr);
    virtual ~TuioControlPanel();

    void setImageView(Image::View *view);

protected slots:
    void onImageInteractiveEnabled(bool enabled);
    void onNewHostName();
    void onCursorChanged(int id, TuioCursorTableModel::CursorChange c);
    void onTokenChanged(int id, TuioTokenTableModel::TokenChange c);
    void onTokenFieldSelected(const QModelIndex&);

private:
    void setTrackingTokenID(int);
    void setTrackingTokenClassID(int);
    void resetTokenTracking();
    bool isTrackingToken(const QTuioToken&) const;
    void initTuio(const QHostAddress& ip, unsigned port);
    void initWidgets();
    void initLayout();
    void updateInteractiveImageToken(const QTuioToken& active_token);

private:
    QMap<int, QGraphicsItem*> marker_list_;
    QMap<int, QGraphicsItem*> token_list_;

    Image::View *image_view_;
    bool image_interactive_;

    QLineEdit* host_name_;
    QPushButton* host_submit_;
    QLabel* host_label_;
    TuioGraphicsView *view_;
    TuioModelHandler *tuio_handler_;
    QTableView* cursor_table_;
    QTableView* token_table_;
    int tracking_token_id_;
    int tracking_token_class_id_;
    Tracker* tracker_;
};

#endif // TUIO_CONTROL_PANEL_H
