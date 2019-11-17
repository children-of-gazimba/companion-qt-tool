#include "companion_widget.h"

#include <QDebug>
#include <QKeySequence>
#include <QJsonDocument>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QVBoxLayout>
#include <QGroupBox>

#include "resources/lib.h"
#include "json/json_mime_data_parser.h"
#include "spotify/spotify_handler.h"

CompanionWidget::CompanionWidget(QWidget *parent)
    : QWidget(parent)
    , project_name_("")
    , progress_bar_(nullptr)
    , actions_()
    , main_menu_(nullptr)
    , spotify_menu_(nullptr)
    , server_selection_(nullptr)
    , sound_file_view_(nullptr)
    , global_player_(nullptr)
    , graphics_view_(nullptr)
    , socket_host_(nullptr)
    , spotify_authenticator_widget_(nullptr)
    , tuio_control_panel_(nullptr)
    , cloud_control_panel_(nullptr)
{
    initWidgets();
    initLayout();
    initActions();
    initMenu();
}

CompanionWidget::~CompanionWidget()
{
    QList<QWidget*> control_panels;
    control_panels << spotify_authenticator_widget_
                   << tuio_control_panel_
                   << cloud_control_panel_
                   << socket_host_;
    for(auto cp: control_panels)
        if(cp) cp->deleteLater();
}

QMenu *CompanionWidget::getMenu()
{
    return main_menu_;
}

const QString &CompanionWidget::getStatusMessage() const
{
    return status_message_;
}

QProgressBar *CompanionWidget::getProgressBar() const
{
    return progress_bar_;
}


void CompanionWidget::onProgressChanged(int value)
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

void CompanionWidget::onSaveProjectAs()
{
    QString file_name = QFileDialog::getSaveFileName(
        this, tr("Save Project"),
        Resources::Lib::DEFAULT_PROJECT_PATH,
        tr("JSON (*.json)")
    );

    if(file_name.size() > 0) {
        setProjectPath(file_name);
        onSaveProject();
    }
}

void CompanionWidget::onSaveProject()
{
    if(project_name_.size() > 0) {
        QJsonDocument doc;

        doc.setObject(graphics_view_->toJsonObject());

        QFile json_file(project_name_);
        json_file.open(QFile::WriteOnly);
        json_file.write(doc.toJson());
    }
    else {
        onSaveProjectAs();
    }
}

void CompanionWidget::onOpenProject()
{
    if(project_name_.size() > 0 || graphics_view_->getLayoutNames().size() > 0) {
        QMessageBox b;
        b.setText(tr("You are about to open another project. All unsaved work will be lost."));
        b.setInformativeText(tr("Do you want to continue?"));
        b.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        b.setDefaultButton(QMessageBox::Yes);
        if(b.exec() == QMessageBox::No)
            return;
    }

    QString file_name = QFileDialog::getOpenFileName(
        this, tr("Open Project"),
        Resources::Lib::DEFAULT_PROJECT_PATH,
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

        clearAll();
        QJsonDocument doc = QJsonDocument::fromJson(json_file.readAll());

        // graphics view could not be set from json
        if(!graphics_view_->setFromJsonObject(doc.object())) {
            QMessageBox b;
            b.setText(tr("The selected file does not seem to contain valid project data."));
            b.setInformativeText(tr("Do you wish to select a different file?"));
            b.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            b.setDefaultButton(QMessageBox::Yes);
            if(b.exec() == QMessageBox::Yes)
                onOpenProject();
            else
                return;
        }

        setProjectPath(file_name);
    }
}

void CompanionWidget::onCloseProject()
{
    QMessageBox b;
    b.setText(tr("You are about to close the current project. All unsaved work will be lost."));
    b.setInformativeText(tr("Do you want to continue?"));
    b.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    b.setDefaultButton(QMessageBox::No);
    if(b.exec() == QMessageBox::No)
        return;
    clearAll();
}

void CompanionWidget::onSaveViewAsLayout()
{
    bool ok;
    QString layout_name = QInputDialog::getText(this, tr("Save View as Layout"),
                                         tr("Layout Name"), QLineEdit::Normal,
                                         "", &ok);

    if (!ok || layout_name.isEmpty())
        return;

    if(graphics_view_->hasLayout(layout_name)) {
        QMessageBox b;
        b.setText(tr("Layout '") + layout_name + tr("' already exists."));
        b.setInformativeText(tr("Do you want to override layout definition?"));
        b.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        b.setDefaultButton(QMessageBox::No);
        if(b.exec() == QMessageBox::Yes)
            onOpenProject();
        else
            onSaveViewAsLayout();
    }

    graphics_view_->storeAsLayout(layout_name);

    if(project_name_.size() == 0) {
        QMessageBox b;
        b.setText(tr("No project file has been set for your current session. Layouts are stored in your project file"));
        b.setInformativeText(tr("Do you wish to save the current project now?"));
        b.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        b.setDefaultButton(QMessageBox::Yes);
        if(b.exec() == QMessageBox::Yes)
            onOpenProject();
    }
}

void CompanionWidget::onLoadLayout()
{
    QStringList layouts = graphics_view_->getLayoutNames();
    if(layouts.size() == 0) {
        QMessageBox b;
        b.setText(tr("No layouts defined on view."));
        b.setInformativeText(tr("Load a project that defines layouts or create some using 'File' > 'Save View as Layout'"));
        b.setStandardButtons(QMessageBox::Ok);
        b.setDefaultButton(QMessageBox::Ok);
        b.exec();
        return;
    }

    QString layout = QInputDialog::getItem(this, tr("Load Layout"), tr("Layouts"), layouts);
    if(layout.size() == 0)
        return;

    if(!graphics_view_->loadLayout(layout)) {
        QMessageBox b;
        b.setText(tr("Layout could not be loaded."));
        b.setInformativeText(tr("Unknown error occured. Sorry for that. =("));
        b.setStandardButtons(QMessageBox::Ok);
        b.exec();
        return;
    }
}

void CompanionWidget::onStartSpotifyControlWidget()
{
    if(spotify_authenticator_widget_ == nullptr) {
        spotify_authenticator_widget_ = new SpotifyControlPanel;
    }
    if(spotify_authenticator_widget_->isVisible()) {
        spotify_authenticator_widget_->raise();
        spotify_authenticator_widget_->activateWindow();
    }
    else {
        spotify_authenticator_widget_->show();
    }
}

void CompanionWidget::onStartTuioControlPanel()
{
    if(tuio_control_panel_ == nullptr) {
        tuio_control_panel_ = new TuioControlPanel;
    }
    if(tuio_control_panel_->isVisible()) {
        tuio_control_panel_->raise();
        tuio_control_panel_->activateWindow();
    }
    else {
        tuio_control_panel_->show();
    }
}

void CompanionWidget::onStartSocketServer()
{
    if(socket_host_ == nullptr)
        socket_host_ = new SocketHostWidget(graphics_view_);
    socket_host_->show();
}

void CompanionWidget::onStartCloudControlPanel()
{
    if(cloud_control_panel_ == nullptr) {
        cloud_control_panel_ = new CloudControlPanel;
    }
    if(cloud_control_panel_->isVisible()) {
        cloud_control_panel_->raise();
        cloud_control_panel_->activateWindow();
    }
    else {
        cloud_control_panel_->show();
    }

}

void CompanionWidget::onLayoutAdded(const QString &name)
{
    QMessageBox b;
    b.setText(tr("Layout '") + name + tr("' has been added or updated."));
    b.setInformativeText(tr("Do you wish to save the project."));
    b.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    if(b.exec() == QMessageBox::Yes)
        onSaveProject();
}

void CompanionWidget::clearAll()
{
    graphics_view_->clear();
    setProjectPath("");
}

void CompanionWidget::setProjectPath(const QString &path)
{
    if (path.size() == 0) {
        project_name_ = "";
        actions_["Save Project"]->setText("Save Project");
    }
    else {
        project_name_ = path;
        QString file_name = path.split("/").back();
        actions_["Save Project"]->setText("Save Project '"+file_name.split(".").front()+"'");
    }
}

void CompanionWidget::initWidgets()
{
    global_player_ = new SoundFilePlayer(this);

    sound_file_view_ = new SoundListPlaybackView(this);
    connect(sound_file_view_, &SoundListPlaybackView::play,
            this, [=](const SoundData& rec, const QString& server) {
        global_player_->setSound(rec, server, true);
    });

    server_selection_ = new QComboBox(this);
    server_selection_->setModel(Resources::Lib::API_CONFIG_MODEL);
    server_selection_label_ = new QLabel(tr("Server"), this);
    connect(server_selection_, &QComboBox::currentTextChanged,
            sound_file_view_, &SoundListPlaybackView::setServerName);

    progress_bar_ = new QProgressBar;
    progress_bar_->setMaximum(100);
    progress_bar_->setMinimum(0);
    progress_bar_->setValue(100);
    progress_bar_->hide();

    graphics_view_ = new Tile::Canvas(this);
    connect(graphics_view_, &Tile::Canvas::layoutAdded,
            this, &CompanionWidget::onLayoutAdded);
}

void CompanionWidget::initLayout()
{
    auto tab_widget = new QTabWidget(this);

    // Sound tab
    QWidget* sound_container = new QWidget(this);
    QVBoxLayout* sound_container_layout = new QVBoxLayout;
    sound_container_layout->addWidget(sound_file_view_, 5);
    sound_container_layout->addWidget(global_player_, -1);
    sound_container_layout->setContentsMargins(0,0,0,0);
    sound_container_layout->setSpacing(0);
    sound_container->setLayout(sound_container_layout);

    tab_widget->addTab(sound_container, tr("Sounds"));

    auto server_selection_layout = new QHBoxLayout;
    server_selection_layout->addWidget(server_selection_label_, -1);
    server_selection_layout->addWidget(server_selection_, 10);

    // left layout
    auto left_box = new QGroupBox(this);
    QVBoxLayout* left_layout = new QVBoxLayout;
    left_layout->addLayout(server_selection_layout);
    left_layout->addWidget(tab_widget);
    left_box->setLayout(left_layout);

    // right layout
    auto right_box = new QGroupBox(this);
    QVBoxLayout* right_layout = new QVBoxLayout;
    right_layout->addWidget(graphics_view_);
    right_box->setLayout(right_layout);

    auto center_splitter = new QSplitter(Qt::Horizontal);
    center_splitter->addWidget(left_box);
    center_splitter->addWidget(right_box);
    center_splitter->setStretchFactor(0, 0);
    center_splitter->setStretchFactor(1, 10);

    // root layout
    QHBoxLayout* layout = new QHBoxLayout;
    layout->addWidget(center_splitter);
    setLayout(layout);
}

void CompanionWidget::initActions()
{
    actions_["Close Project"] = new QAction(tr("Close Project"), this);
    actions_["Close Project"]->setToolTip(tr("Closes the current project, clearing the canvas and clearing image display."));

    actions_["Save Project As..."] = new QAction(tr("Save Project As..."), this);
    actions_["Save Project As..."]->setToolTip(tr("Saves the current work state to a file."));
    actions_["Save Project As..."]->setShortcut(QKeySequence(tr("Ctrl+Shift+S")));

    actions_["Save Project"] = new QAction(tr("Save Project"), this);
    actions_["Save Project"]->setToolTip(tr("Saves the current work state to a file."));
    actions_["Save Project"]->setShortcut(QKeySequence(tr("Ctrl+S")));

    actions_["Open Project..."] = new QAction(tr("Open Project..."), this);
    actions_["Open Project..."]->setToolTip(tr("Opens a previously saved state from a file."));
    actions_["Open Project..."]->setShortcut(QKeySequence(tr("Ctrl+O")));

    actions_["Load Layout..."] = new QAction(tr("Load Layout..."), this);
    actions_["Load Layout..."]->setToolTip(tr("Loads a previously saved layout from the current project."));
    //actions_["Load Layout..."]->setShortcut(QKeySequence(tr("Ctrl+O")));

    actions_["Save View as Layout..."] = new QAction(tr("Save View as Layout..."), this);
    actions_["Save View as Layout..."]->setToolTip(tr("Saves the current view as a layout within the current project."));
    //actions_["Load Layout..."]->setShortcut(QKeySequence(tr("Ctrl+O")));

    actions_["Connect to Spotify"] = new QAction(tr("Connect to Spotify"), this);
    actions_["Connect to Spotify"]->setToolTip(tr("Requests connection to the Spotify music streaming service."));
    actions_["Connect to Spotify"]->setCheckable(true);
    actions_["Connect to Spotify"]->setChecked(false);
    actions_["Spotify Control Panel..."] = new QAction(tr("Spotify Control Panel..."), this);
    actions_["Spotify Control Panel..."]->setToolTip(tr("Shows the Spotify control panel."));
    actions_["Spotify Control Panel..."]->setEnabled(false);
    actions_["Spotify Control Panel..."]->setShortcut(QKeySequence(tr("Ctrl+Y")));
    connect(&SpotifyHandler::instance()->remote, &SpotifyRemoteController::accessGranted,
            this, [=]() {
        actions_["Connect to Spotify"]->setChecked(true);
        actions_["Connect to Spotify"]->setEnabled(false);
        actions_["Spotify Control Panel..."]->setEnabled(true);
    });

    actions_["Run Socket Host..."] = new QAction(tr("Run Socket Host..."), this);
    actions_["Run Socket Host..."]->setToolTip(tr("Opens a local socket application to control current project."));

    actions_["Tuio Control Panel..."] = new QAction(tr("Tuio Control Panel..."), this);
    actions_["Tuio Control Panel..."]->setToolTip(tr("Shows the Tuio control panel."));

    actions_["Cloud Control Panel..."] = new QAction(tr("Cloud Control Panel..."), this);
    actions_["Cloud Control Panel..."]->setToolTip(tr("Shows the Cloud control panel."));

    connect(actions_["Close Project"], &QAction::triggered,
            this, &CompanionWidget::onCloseProject);
    connect(actions_["Save Project As..."], &QAction::triggered,
            this, &CompanionWidget::onSaveProjectAs);
    connect(actions_["Save Project"], &QAction::triggered,
            this, &CompanionWidget::onSaveProject);
    connect(actions_["Open Project..."], &QAction::triggered,
            this, &CompanionWidget::onOpenProject);
    connect(actions_["Load Layout..."], &QAction::triggered,
            this, &CompanionWidget::onLoadLayout);
    connect(actions_["Save View as Layout..."], &QAction::triggered,
            this, &CompanionWidget::onSaveViewAsLayout);
    connect(actions_["Connect to Spotify"], &QAction::triggered,
            this, &CompanionWidget::onStartSpotifyControlWidget);
    connect(actions_["Spotify Control Panel..."], &QAction::triggered,
            this, &CompanionWidget::onStartSpotifyControlWidget);
    connect(actions_["Run Socket Host..."], &QAction::triggered,
            this, &CompanionWidget::onStartSocketServer);
    connect(actions_["Tuio Control Panel..."], &QAction::triggered,
            this, &CompanionWidget::onStartTuioControlPanel);
    connect(actions_["Cloud Control Panel..."], &QAction::triggered,
            this, &CompanionWidget::onStartCloudControlPanel);
}

void CompanionWidget::initMenu()
{
    main_menu_ = new QMenu(tr("Companion"));

    QMenu* file_menu = main_menu_->addMenu(tr("File"));
    file_menu->addAction(actions_["Open Project..."]);
    file_menu->addAction(actions_["Load Layout..."]);
    file_menu->addSeparator();
    file_menu->addAction(actions_["Close Project"]);
    file_menu->addSeparator();
    file_menu->addAction(actions_["Save Project"]);
    file_menu->addAction(actions_["Save Project As..."]);
    file_menu->addAction(actions_["Save View as Layout..."]);
    file_menu->addSeparator();
    QMenu* tool_menu = main_menu_->addMenu(tr("Tools"));
    spotify_menu_ = tool_menu->addMenu(tr("Spotify"));
    spotify_menu_->addAction(actions_["Connect to Spotify"]);
    spotify_menu_->addAction(actions_["Spotify Control Panel..."]);
    tool_menu->addAction(actions_["Run Socket Host..."]);
    tool_menu->addAction(actions_["Tuio Control Panel..."]);
    tool_menu->addAction(actions_["Cloud Control Panel..."]);

    main_menu_->addMenu(file_menu);
    main_menu_->addMenu(tool_menu);
}
