#ifndef COMPANION_WIDGET_H
#define COMPANION_WIDGET_H

#include <QWidget>

#include <QMap>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QTreeView>
#include <QTableView>
#include <QAction>
#include <QMenu>
#include <QProgressBar>
#include <QSplitter>
#include <QScrollArea>

#include "misc/drop_group_box.h"
#include "resources/importer.h"
#include "sound/sound_list_playback_view_deprecated.h"
#include "sound/sound_list_playback_view.h"
#include "db/database_handler.h"
#include "category/category_tree_view.h"
#include "tile/canvas.h"
#include "image/image_browser.h"
#include "preset/preset_view.h"
#include "spotify/spotify_control_panel.h"
#include "web/socket_host_widget.h"
#include "tuio/tuio_control_panel.h"
#include "sound/sound_file_player.h"

class CompanionWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CompanionWidget(QWidget *parent = 0);
    ~CompanionWidget();

    QMenu* getMenu();
    QString const& getStatusMessage() const;
    QProgressBar* getProgressBar() const;

signals:
    void statusMessageUpdated(QString const&);

public slots:

private slots:
    void onProgressChanged(int);
    void onSelectedCategoryChanged(CategoryRecord* rec);
    void onDeleteDatabase();
    void onSaveProjectAs();
    void onSaveProject();
    void onOpenProject();
    void onCloseProject();
    void onSaveViewAsLayout();
    void onLoadLayout();
    void onStartSpotifyControlWidget();
    void onStartTuioControlPanel();
    void onStartSocketServer();
    void onLayoutAdded(const QString& name);

private:
    void clearAll();
    void setProjectPath(QString const& path);

    void initWidgets();
    void initLayout();
    void initActions();
    void initMenu();
    void initDB();

    // PROJECT
    QString project_name_;

    // STATUS
    QString status_message_;
    QProgressBar* progress_bar_;

    // MENU & ACTIONS
    QMap<QString, QAction*> actions_;
    QMenu* main_menu_;

    // WIDGETS
    SoundListPlaybackViewDeprecated* sound_file_view_deprecated_;
    SoundListPlaybackView* sound_file_view_;
    SoundFilePlayer* global_player_;
    CategoryTreeView* category_view_;
    PresetView* preset_view_;

    Tile::Canvas* graphics_view_;
    Resources::Importer* sound_file_importer_;
    QSplitter* center_h_splitter_;
    QSplitter* left_v_splitter_;
    QGroupBox* left_box_;
    QGroupBox* right_box_;
    SocketHostWidget* socket_host_;
    ImageBrowser* image_browser_;
    SpotifyControlPanel *spotify_authenticator_widget_;
    TuioControlPanel *tuio_control_panel_;
    QTabWidget* left_tabwidget_;
    QMenu* spotify_menu_;


    // DB handler
    DatabaseHandler* db_handler_;
};

#endif // DSAMEDIACONTROLKIT_H
