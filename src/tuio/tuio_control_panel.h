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
#include <QMenuBar>
#include <QSplitter>

#include "tuio_graphics_view.h"
#include "udp_client.h"
#include "tuio_model_handler.h"
#include "register_token_dialog.h"

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
    void onNewHostName();
    void onCursorChanged(int id, TuioCursorTableModel::CursorChange c);
    void onTokenChanged(int id, TuioTokenTableModel::TokenChange c);
    void onBlobChanged(int id, TuioBlobTableModel::BlobChange c);

    void onTokenFieldSelected(const QModelIndex&);

    void onRegisterCursorTracker();
    void onRegisterTokenTracker();
    void onRegisterBlobTracker();

    void onSceneSelectionChanged();
    void onTrackerAdded(const TuioTokenTracker&);

private:
    bool isTrackingToken(const QTuioToken&) const;
    void updateInteractiveImageToken(const QTuioToken& active_token);

private:
    void initTuio(const QHostAddress& ip, unsigned port);
    void initWidgets();
    void initActions();
    void initLayout();

    QMap<int, QGraphicsItem*> marker_list_;
    QMap<int, QGraphicsItem*> token_list_;
    QMap<int, QGraphicsItem*> blob_list_;

    RegisterTokenDialog* token_registry_;

    Image::View *image_view_;

    QLineEdit* host_name_;
    QPushButton* host_submit_;
    QLabel* host_label_;
    TuioGraphicsView *view_;
    TuioModelHandler *tuio_handler_;
    QTableView* cursor_table_;
    QTableView* token_table_;
    QTableView* blob_table_;
    QMenuBar* menu_bar_;
    QSplitter* main_splitter_;
    QMap<QString, TuioTokenTracker*> token_tracker_;
};

#endif // TUIO_CONTROL_PANEL_H
