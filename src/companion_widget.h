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
#include "sound_file/master_view.h"
#include "db/handler.h"
#include "category/tree_view.h"
#include "tile/graphics_view.h"
#include "image/browser.h"
#include "preset/preset_view.h"
#include "spotify/spotify_control_panel.h"

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
    void onSelectedCategoryChanged(DB::CategoryRecord* rec);
    void onDeleteDatabase();
    void onSaveProjectAs();
    void onSaveProject();
    void onOpenProject();
    void onStartSpotifyControlWidget();

private:
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
    SoundFile::MasterView* sound_file_view_;
    Category::TreeView* category_view_;
    Preset::PresetView* preset_view_;

    Tile::GraphicsView* graphics_view_;
    Resources::Importer* sound_file_importer_;
    QSplitter* center_h_splitter_;
    QSplitter* left_v_splitter_;
    QGroupBox* left_box_;
    QGroupBox* right_box_;
    Image::Browser* image_browser_;
    SpotifyControlPanel *spotify_authenticator_widget_;
    QTabWidget* left_tabwidget_;
    QMenu* spotify_menu_;

    // DB handler
    DB::Handler* db_handler_;
};

#endif // DSAMEDIACONTROLKIT_H
