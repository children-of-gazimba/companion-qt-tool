#ifndef DSA_MEDIA_CONTROL_KIT_H
#define DSA_MEDIA_CONTROL_KIT_H

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
#include "2D/graphics_view.h"
#include "web/host.h"

class DsaMediaControlKit : public QWidget
{
    Q_OBJECT
public:
    explicit DsaMediaControlKit(QWidget *parent = 0);
    ~DsaMediaControlKit();

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
    void onOpenProject();
    void onStartWebServer();

private:
    void initWidgets();
    void initLayout();
    void initActions();
    void initMenu();
    void initDB();

    // STATUS
    QString status_message_;
    QProgressBar* progress_bar_;

    // MENU & ACTIONS
    QMap<QString, QAction*> actions_;
    QMenu* main_menu_;

    // WIDGETS
    SoundFile::MasterView* sound_file_view_;
    Category::TreeView* category_view_;

    TwoD::GraphicsView* preset_view_;
    Resources::Importer* sound_file_importer_;
    QSplitter* center_h_splitter_;
    QSplitter* left_v_splitter_;
    QGroupBox* left_box_;
    QGroupBox* right_box_;
    Web::Host* web_host_;

    // DB handler
    DB::Handler* db_handler_;
};

#endif // DSAMEDIACONTROLKIT_H
