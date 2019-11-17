#ifndef COMPANION_WIDGET_H
#define COMPANION_WIDGET_H

#include <QWidget>
#include <QProgressBar>
#include <QComboBox>

#include "sound/sound_list_playback_view.h"
#include "tile/canvas.h"
#include "spotify/spotify_control_panel.h"
#include "web/socket_host_widget.h"
#include "tuio/tuio_control_panel.h"
#include "sound/sound_file_player.h"
#include "api/cloud_control_panel.h"

class CompanionWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CompanionWidget(QWidget *parent = nullptr);
    ~CompanionWidget();

    QMenu* getMenu();
    QString const& getStatusMessage() const;
    QProgressBar* getProgressBar() const;

signals:
    void statusMessageUpdated(QString const&);

public slots:

private slots:
    void onProgressChanged(int);
    void onSaveProjectAs();
    void onSaveProject();
    void onOpenProject();
    void onCloseProject();
    void onSaveViewAsLayout();
    void onLoadLayout();
    void onStartSpotifyControlWidget();
    void onStartTuioControlPanel();
    void onStartSocketServer();
    void onStartCloudControlPanel();
    void onLayoutAdded(const QString& name);

private:
    void clearAll();
    void setProjectPath(QString const& path);

    void initWidgets();
    void initLayout();
    void initActions();
    void initMenu();

    // PROJECT
    QString project_name_;

    // STATUS
    QString status_message_;
    QProgressBar* progress_bar_;

    // MENU & ACTIONS
    QMap<QString, QAction*> actions_;
    QMenu* main_menu_;
    QMenu* spotify_menu_;

    // MAIN WIDGETS
    QComboBox* server_selection_;
    QLabel* server_selection_label_;
    SoundListPlaybackView* sound_file_view_;
    SoundFilePlayer* global_player_;
    Tile::Canvas* graphics_view_;

    // TOOLS
    SocketHostWidget* socket_host_;
    SpotifyControlPanel *spotify_authenticator_widget_;
    TuioControlPanel *tuio_control_panel_;
    CloudControlPanel* cloud_control_panel_;
};

#endif // DSAMEDIACONTROLKIT_H
