#include "spotify_tile_configure_dialog.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>

#include <QPushButton>

#include <QJsonDocument>
#include <QJsonObject>

#include "spotify/spotify_handler.h"

SpotifyTileConfigureDialog::SpotifyTileConfigureDialog(QWidget *parent)
    : QDialog(parent)
    , edit_uri_(0)
    , name_label_(0)
    , repeat_label_(0)
    , repeat_button_group_(0)
    , radio_repeat_off(0)
    , radio_repeat_track_(0)
    , radio_repeat_context_(0)
    , shuffle_checkbox_(0)
    , btn_submit_(0)
    , btn_cancel_(0)
    , settings_()

{
    initWidgets();
    initLayout();
    setAcceptDrops(true);
}

SpotifyTileConfigureDialog::SpotifyTileConfigureDialog(const SpotifyRemoteController::Settings &settings, QWidget *parent)
    : QDialog(parent)
    , edit_uri_(0)
    , name_label_(0)
    , repeat_label_(0)
    , repeat_button_group_(0)
    , radio_repeat_off(0)
    , radio_repeat_track_(0)
    , radio_repeat_context_(0)
    , shuffle_checkbox_(0)
    , btn_submit_(0)
    , btn_cancel_(0)
    , settings_(settings)
{
    initWidgets();
    initLayout();
    setAcceptDrops(true);
}

const SpotifyRemoteController::Settings &SpotifyTileConfigureDialog::getSettings() const
{
    return settings_;
}

void SpotifyTileConfigureDialog::setSettings(const SpotifyRemoteController::Settings &settings)
{
    settings_ = settings;

    if(settings.mode != SpotifyRemoteController::Settings::Playlist ||
            settings.mode != SpotifyRemoteController::Settings::Track) {
        qDebug().nospace() << Q_FUNC_INFO << " :" << __LINE__;
        qDebug() << "  >" << "Error: There was no Mode specified... exiting!";
        return;

    } else {
        if(settings.playlist_uri != "") {
            if(settings.track_uri != "") {
                qDebug().nospace() << Q_FUNC_INFO << " :" << __LINE__;
                qDebug() << "  >" << "Error: Cannot set spotify tile with two modes!";
            } else {
                settings_.playlist_uri = settings.playlist_uri;
                edit_uri_->setText(settings.playlist_uri);
            }
        } else if(settings.track_uri != "") {
            if(settings.playlist_uri !="") {
                qDebug().nospace() << Q_FUNC_INFO << " :" << __LINE__;
                qDebug() << "  >" << "Error: Cannot set spotify tile with two modes!";
            } else {
                settings_.track_uri = settings.track_uri;
                edit_uri_->setText(settings.track_uri);
            }
        } else {
            qDebug().nospace() << Q_FUNC_INFO << " :" << __LINE__;
            qDebug() << "  >" << "Error: There was neither a track nor a playlist URL given.";
        }
        switch (settings.repeat_mode) {
            case SpotifyRemoteController::Context:
                radio_repeat_context_->setChecked(true);
                break;
            case SpotifyRemoteController::Track:
                radio_repeat_track_->setChecked(true);
                break;
            case SpotifyRemoteController::Off:
                radio_repeat_off->setChecked(true);
            default:
                qDebug().nospace() << Q_FUNC_INFO << " :" << __LINE__;
                qDebug() << "  >" << "Error: There was no repeat mode set in the given configuration";
                qDebug() << "     >" << "setting context as default mode!";
                radio_repeat_context_->setChecked(true);
                break;
        }
        shuffle_checkbox_->setChecked(settings.shuffle_enabled);
    }
}

void SpotifyTileConfigureDialog::onSubmit()
{
    switch (settings_.mode) {
        case SpotifyRemoteController::Settings::Playlist:
            settings_.playlist_uri = edit_uri_->text();
            break;
        case SpotifyRemoteController::Settings::Track:
            settings_.track_uri = edit_uri_->text();
            break;
        default:
            break;
    }
    settings_.repeat_mode = (SpotifyRemoteController::RepeatMode) repeat_button_group_->checkedId();
    settings_.shuffle_enabled = shuffle_checkbox_->isChecked();
    accept();
}

void SpotifyTileConfigureDialog::onTextChanged(const QString &t)
{
    if(t.startsWith("https://"))
        settings_.setFromWebLink(t);
    else
        settings_.setFromURI(t);
    updateUI();
    updatePlaybackInfo();
}

void SpotifyTileConfigureDialog::updateUI()
{
    if(settings_.playlist_uri.size() > 0)
        edit_uri_->setText(settings_.playlist_uri);
    else if(settings_.track_uri.size() > 0)
        edit_uri_->setText(settings_.track_uri);
    radio_repeat_off->setChecked(settings_.repeat_mode == SpotifyRemoteController::Off);
    radio_repeat_track_->setChecked(settings_.repeat_mode == SpotifyRemoteController::Track);
    radio_repeat_context_->setChecked(settings_.repeat_mode == SpotifyRemoteController::Context);
    shuffle_checkbox_->setChecked(settings_.shuffle_enabled);
}

void SpotifyTileConfigureDialog::updatePlaybackInfo()
{
    QNetworkReply *reply = nullptr;
    if(settings_.mode == SpotifyRemoteController::Settings::Playlist) {
        reply = SpotifyHandler::instance()->playlistInfo(settings_.playlist_uri);
    }
    else if(settings_.mode == SpotifyRemoteController::Settings::Track) {
        QStringList track_uri_components = settings_.track_uri.split(":");
        reply = SpotifyHandler::instance()->trackInfo(track_uri_components.last());
    }
    // connect to finished event of reply to get the servers response
    connect(reply, &QNetworkReply::finished,
            this, [=]() {
        QJsonDocument playback_info = QJsonDocument::fromJson(reply->readAll());
        QString name(playback_info.object()["name"].toString());
        if(name.size() == 0)
            name_label_->setText("INVALID RESOURCE");
        else
            name_label_->setText(name);
        reply->deleteLater();
    });
}

void SpotifyTileConfigureDialog::initWidgets()
{
    name_label_ = new QLabel(this);
    QFont f(name_label_->font());
    f.setPointSize(f.pointSize()*3);
    name_label_->setFont(f);

    edit_uri_ = new QLineEdit(this);
    edit_uri_->setReadOnly(true);
    edit_uri_->setPlaceholderText(tr("Drag and drop playlist or track URI/link here"));
    connect(edit_uri_, &QLineEdit::textChanged,
            this, &SpotifyTileConfigureDialog::onTextChanged);

    repeat_label_ = new QLabel("Repeat Mode:", this);
    radio_repeat_off = new QRadioButton("Off");
    radio_repeat_track_ = new QRadioButton("Track");
    radio_repeat_context_ = new QRadioButton("Context");
    repeat_button_group_ = new QButtonGroup;
    repeat_button_group_->addButton(radio_repeat_context_);
    repeat_button_group_->setId(radio_repeat_context_, SpotifyRemoteController::Context);
    repeat_button_group_->addButton(radio_repeat_track_);
    repeat_button_group_->setId(radio_repeat_track_, SpotifyRemoteController::Track);
    repeat_button_group_->addButton(radio_repeat_off);
    repeat_button_group_->setId(radio_repeat_off, SpotifyRemoteController::Off);

    shuffle_checkbox_ = new QCheckBox("Shuffle", this);

    btn_submit_ = new QPushButton("Submit", this);
    connect(btn_submit_, &QPushButton::clicked,
            this, &SpotifyTileConfigureDialog::onSubmit);

    btn_cancel_ = new QPushButton("Cancel", this);
    connect(btn_cancel_, &QPushButton::clicked,
            this, &SpotifyTileConfigureDialog::reject);

    updateUI();
}

void SpotifyTileConfigureDialog::initLayout()
{
    // mode group
    QHBoxLayout *edit_layout = new QHBoxLayout;
    edit_layout->addWidget(edit_uri_);
    QGroupBox *edit_group = new QGroupBox("URI");
    edit_group->setLayout(edit_layout);

    QGroupBox *option_group = new QGroupBox("Options");
    QHBoxLayout *repeat_layout = new QHBoxLayout;
    repeat_layout->addWidget(repeat_label_);
    repeat_layout->addWidget(radio_repeat_context_);
    repeat_layout->addWidget(radio_repeat_track_);
    repeat_layout->addWidget(radio_repeat_off);

    QVBoxLayout  *inner_option = new QVBoxLayout;
    inner_option->addItem(repeat_layout);
    inner_option->addWidget(shuffle_checkbox_);
    option_group->setLayout(inner_option);

    QHBoxLayout *finalize_layout = new QHBoxLayout;
    finalize_layout->addStretch(2);
    finalize_layout->addWidget(btn_submit_, 1);
    finalize_layout->addWidget(btn_cancel_, 1);

    QVBoxLayout *root = new QVBoxLayout;
    root->addWidget(name_label_);
    root->addWidget(edit_group);
    root->addWidget(option_group);
    root->addLayout(finalize_layout, -1);

    setLayout(root);
    setMinimumWidth(400);
    setFixedSize(minimumSize());
}

void SpotifyTileConfigureDialog::dragEnterEvent(QDragEnterEvent *event)
{
    QString mime_text(event->mimeData()->text());
    qDebug().nospace() << Q_FUNC_INFO << " @ line " << __LINE__;
    qDebug() << "  > " << mime_text;
    if (event->source() != this && mime_text.contains("spotify")) {
        event->setDropAction(Qt::CopyAction);
        event->accept();
        return;
    }
}

void SpotifyTileConfigureDialog::dragMoveEvent(QDragMoveEvent *event)
{
    QString mime_text(event->mimeData()->text());
    qDebug().nospace() << Q_FUNC_INFO << " @ line " << __LINE__;
    qDebug() << "  > " << mime_text;
    if (event->source() != this && mime_text.contains("spotify")) {
        event->setDropAction(Qt::CopyAction);
        event->accept();
        return;
    }
}

void SpotifyTileConfigureDialog::dropEvent(QDropEvent *event)
{
    QString mime_text(event->mimeData()->text());
    qDebug().nospace() << Q_FUNC_INFO << " @ line " << __LINE__;
    qDebug() << "  > " << mime_text;
    if (event->source() != this && mime_text.contains("spotify")) {
        event->setDropAction(Qt::CopyAction);
        event->accept();
        edit_uri_->setText(mime_text);
        return;
    }
}
