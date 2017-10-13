#include "dsa_media_control_kit.h"

#include <QDebug>
#include <QDir>
#include <QKeySequence>
#include <QJsonDocument>
#include <QFileDialog>
#include <QMessageBox>
#include <QCoreApplication>

#include "db/core/api.h"
#include "resources/lib.h"
#include "misc/json_mime_data_parser.h"

DsaMediaControlKit::DsaMediaControlKit(QWidget *parent)
    : QWidget(parent)
    , progress_bar_(0)
    , actions_()
    , main_menu_(0)
    , sound_file_view_(0)
    , category_view_(0)
    , preset_view_(0)
    , sound_file_importer_(0)
    , center_h_splitter_(0)
    , left_v_splitter_(0)
    , left_box_(0)
    , right_box_(0)
    , web_host_(0)
    , image_list_(0)
    , db_handler_(0)
{
    initDB();
    initWidgets();
    initLayout();
    initActions();
    initMenu();
}

DsaMediaControlKit::~DsaMediaControlKit()
{
    delete web_host_;
    web_host_ = 0;
}

QMenu *DsaMediaControlKit::getMenu()
{
    return main_menu_;
}

const QString &DsaMediaControlKit::getStatusMessage() const
{
    return status_message_;
}

QProgressBar *DsaMediaControlKit::getProgressBar() const
{
    return progress_bar_;
}


void DsaMediaControlKit::onProgressChanged(int value)
{
    if(value != 100) {
        if(progress_bar_->isHidden()) {
            progress_bar_->show();
        }
    }
    else {
        progress_bar_->hide();
    }
    progress_bar_->setValue(value);
}

void DsaMediaControlKit::onSelectedCategoryChanged(DB::CategoryRecord *rec)
{
    int id = -1;
    if(rec != 0)
        id = rec->id;

    sound_file_view_->setSoundFiles(db_handler_->getSoundFileRecordsByCategoryId(id));
}

void DsaMediaControlKit::onDeleteDatabase()
{
    db_handler_->deleteAll();
    category_view_->selectRoot();
}

void DsaMediaControlKit::onSaveProjectAs()
{
    QString file_name = QFileDialog::getSaveFileName(
        this, tr("Save Project"),
        "",
        tr("JSON (*.json)")
    );

    if(file_name.size() > 0) {
        QJsonDocument doc;

        doc.setObject(preset_view_->toJsonObject());

        QFile json_file(file_name);
        json_file.open(QFile::WriteOnly);
        json_file.write(doc.toJson());
    }
}

void DsaMediaControlKit::onOpenProject()
{
    QString file_name = QFileDialog::getOpenFileName(
        this, tr("Open Project"),
        "",
        tr("JSON (*.json)")
    );

    if(file_name.size() > 0) {
        QFile json_file(file_name);

        // opening failed
        if(!json_file.open(QFile::ReadOnly)) {
            QMessageBox b;
            b.setText(tr("The selected file could not be opened."));
            b.setInformativeText(tr("Do you wish to select a different file?"));
            b.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            b.setDefaultButton(QMessageBox::Yes);
            if(b.exec() == QMessageBox::Yes)
                onOpenProject();
            else
                return;
        }

        QJsonDocument doc = QJsonDocument::fromJson(json_file.readAll());

        // graphics view could not be set from json
        if(!preset_view_->setFromJsonObject(doc.object())) {
            QMessageBox b;
            b.setText(tr("The selected file does not seem to contain valid project data."));
            b.setInformativeText(tr("Do you wish to select a different file?"));
            b.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            b.setDefaultButton(QMessageBox::Yes);
            if(b.exec() == QMessageBox::Yes)
                onOpenProject();
        }
    }
}

void DsaMediaControlKit::onStartWebServer()
{
    if(web_host_ == 0)
        web_host_ = new Web::Host;
    web_host_->setPresetView(preset_view_);
    web_host_->show();
}

void DsaMediaControlKit::initWidgets()
{
    sound_file_view_ = new SoundFile::MasterView(
        db_handler_->getSoundFileTableModel()->getSoundFiles(),
        this
    );

    progress_bar_ = new QProgressBar;
    progress_bar_->setMaximum(100);
    progress_bar_->setMinimum(0);
    progress_bar_->setValue(100);
    progress_bar_->hide();

    preset_view_ = new TwoD::GraphicsView(this);
    preset_view_->setSoundFileModel(db_handler_->getSoundFileTableModel());

    sound_file_importer_ = new Resources::Importer(
        db_handler_->getResourceDirTableModel(),
        this
    );

    category_view_ = new Category::TreeView(this);
    category_view_->setCategoryTreeModel(db_handler_->getCategoryTreeModel());

    left_box_ = new QGroupBox(this);
    right_box_ = new QGroupBox(this);

    left_v_splitter_ = new QSplitter(Qt::Vertical, this);
    left_v_splitter_->addWidget(category_view_);
    left_v_splitter_->addWidget(sound_file_view_);

    left_v_splitter_->setStretchFactor(0, 2);
    left_v_splitter_->setStretchFactor(1, 8);

    center_h_splitter_ = new QSplitter(Qt::Horizontal);
    center_h_splitter_->addWidget(left_box_);
    center_h_splitter_->addWidget(right_box_);
    center_h_splitter_->setStretchFactor(0, 0);
    center_h_splitter_->setStretchFactor(1, 10);

    image_list_ = new Image::FileTable;
    image_list_->setWindowFlags(Qt::Window);
    image_list_->hide();

    connect(sound_file_importer_, SIGNAL(folderImported(QList<Resources::SoundFile> const&)),
            db_handler_, SLOT(insertSoundFilesAndCategories(QList<Resources::SoundFile> const&)));
    connect(sound_file_importer_, SIGNAL(folderImported()),
            category_view_, SLOT(selectRoot()));
    connect(db_handler_, SIGNAL(progressChanged(int)),
            this, SLOT(onProgressChanged(int)));
    connect(category_view_, SIGNAL(categorySelected(DB::CategoryRecord*)),
            this, SLOT(onSelectedCategoryChanged(DB::CategoryRecord*)));
    connect(sound_file_view_, SIGNAL(deleteSoundFileRequested(int)),
            db_handler_->getSoundFileTableModel(), SLOT(deleteSoundFile(int)));
    connect(db_handler_->getSoundFileTableModel(), SIGNAL(aboutToBeDeleted(DB::SoundFileRecord*)),
            sound_file_view_, SLOT(onSoundFileAboutToBeDeleted(DB::SoundFileRecord*)));
    connect(preset_view_, SIGNAL(dropAccepted()),
            sound_file_view_, SLOT(onDropSuccessful()));
}

void DsaMediaControlKit::initLayout()
{
    QHBoxLayout* layout = new QHBoxLayout;

    // left layout
    QVBoxLayout* l_layout = new QVBoxLayout;
    l_layout->addWidget(left_v_splitter_);
    left_box_->setLayout(l_layout);

    // right layout
    QVBoxLayout* r_layout = new QVBoxLayout;
    r_layout->addWidget(preset_view_);
    right_box_->setLayout(r_layout);

    layout->addWidget(center_h_splitter_);

    setLayout(layout);
}

void DsaMediaControlKit::initActions()
{
    actions_["Import Resource Folder..."] = new QAction(tr("Import Resource Folder..."), this);
    actions_["Import Resource Folder..."]->setToolTip(tr("Imports a folder of resources into the program."));
    actions_["Import Resource Folder..."]->setShortcut(QKeySequence(tr("Ctrl+Shift+O")));

    actions_["Delete Database Contents..."] = new QAction(tr("Delete Database Contents..."), this);
    actions_["Delete Database Contents..."]->setToolTip(tr("Deletes all contents from application database."));

    actions_["Save Project As..."] = new QAction(tr("Save Project As..."), this);
    actions_["Save Project As..."]->setToolTip(tr("Saves the current work state to a file."));
    actions_["Save Project As..."]->setShortcut(QKeySequence(tr("Ctrl+S")));

    actions_["Open Project..."] = new QAction(tr("Open Project..."), this);
    actions_["Open Project..."]->setToolTip(tr("Opens a previously saved state from a file."));
    actions_["Open Project..."]->setShortcut(QKeySequence(tr("Ctrl+O")));

    actions_["Run Web Host..."] = new QAction(tr("Run Web Host..."), this);
    actions_["Run Web Host..."]->setToolTip(tr("Opens a local web application to control current project."));

    actions_["Run Image Viewer..."] = new QAction(tr("Run Image Viewer..."), this);
    actions_["Run Image Viewer..."]->setToolTip(tr("Opens an image browser with capabilities to show browsed images."));

    connect(actions_["Import Resource Folder..."] , SIGNAL(triggered(bool)),
            sound_file_importer_, SLOT(startBrowseFolder(bool)));
    connect(actions_["Delete Database Contents..."], SIGNAL(triggered()),
            this, SLOT(onDeleteDatabase()));
    connect(actions_["Save Project As..."], SIGNAL(triggered()),
            this, SLOT(onSaveProjectAs()));
    connect(actions_["Open Project..."], SIGNAL(triggered()),
            this, SLOT(onOpenProject()));
    connect(actions_["Run Web Host..."], SIGNAL(triggered()),
            this, SLOT(onStartWebServer()));
    connect(actions_["Run Image Viewer..."], SIGNAL(triggered()),
            image_list_, SLOT(show()));
}

void DsaMediaControlKit::initMenu()
{
    main_menu_ = new QMenu(tr("DsaMediaControlKit"));

    QMenu* file_menu = main_menu_->addMenu(tr("File"));
    file_menu->addAction(actions_["Save Project As..."]);
    file_menu->addAction(actions_["Open Project..."]);
    file_menu->addSeparator();
    file_menu->addAction(actions_["Import Resource Folder..."]);
    file_menu->addSeparator();
    file_menu->addAction(actions_["Delete Database Contents..."]);
    QMenu* tool_menu = main_menu_->addMenu(tr("Tools"));
    tool_menu->addAction(actions_["Run Web Host..."]);
    tool_menu->addAction(actions_["Run Image Viewer..."]);

    main_menu_->addMenu(file_menu);
    main_menu_->addMenu(tool_menu);
}

void DsaMediaControlKit::initDB()
{
    DB::Core::Api* db_api = new DB::Core::Api(QCoreApplication::applicationDirPath() + Resources::Lib::DATABASE_PATH, this);
    db_handler_ = new DB::Handler(db_api, this);
}
