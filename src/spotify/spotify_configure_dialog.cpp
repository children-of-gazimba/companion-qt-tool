#include "spotify_configure_dialog.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>

#include <QPushButton>

#include <QJsonDocument>
#include <QJsonObject>

#include <QNetworkReply>

#include "spotify/spotify_handler.h"

SpotifyConfigureDialog::SpotifyConfigureDialog(QWidget *parent)
    : QDialog(parent)
    , mode_button_group_(0)
    , radio_playlist_(0)
    , radio_track_(0)
    , edit_uri_(0)
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

    resize(QSize(400,300));
}

SpotifyConfigureDialog::SpotifyConfigureDialog(const SpotifyRemoteController::Settings &settings, QWidget *parent)
    : QDialog(parent)
    , mode_button_group_(0)
    , radio_playlist_(0)
    , radio_track_(0)
    , edit_uri_(0)
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

    resize(QSize(400,300));
}

const SpotifyRemoteController::Settings &SpotifyConfigureDialog::getSettings() const
{
    return settings_;
}

void SpotifyConfigureDialog::setSettings(const SpotifyRemoteController::Settings &settings)
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

const QJsonDocument &SpotifyConfigureDialog::getPlaybackInfo() const
{
    return  playback_info_;
}


void SpotifyConfigureDialog::onSubmit()
{
    QNetworkReply *reply;
    switch (mode_button_group_->checkedId()) {
        case SpotifyRemoteController::Settings::Playlist:
            settings_.mode = SpotifyRemoteController::Settings::Playlist;
            settings_.playlist_uri = edit_uri_->text();
            reply = SpotifyHandler::instance()->playlistInfo(settings_.playlist_uri);
            break;
        case SpotifyRemoteController::Settings::Track:
        {
            settings_.mode =  SpotifyRemoteController::Settings::Track;
            settings_.track_uri = edit_uri_->text();

            // filter for id
            QStringList track_uri_components = settings_.track_uri.split(":");
            reply = SpotifyHandler::instance()->trackInfo(track_uri_components.last());
            break;
        }
        default:
            break;
    }


    settings_.repeat_mode = (SpotifyRemoteController::RepeatMode) repeat_button_group_->checkedId();
    settings_.shuffle_enabled = shuffle_checkbox_->isChecked();


    // connect to finished event of reply to get the servers response
    connect(reply, &QNetworkReply::finished,
            this, [=]() {
        playback_info_ = QJsonDocument::fromJson(reply->readAll());
        qDebug().nospace() << Q_FUNC_INFO << " :" << __LINE__;
        qDebug() << "  >" << "gotit boi";
        accept();
    });

}


void SpotifyConfigureDialog::initWidgets()
{
    edit_uri_ = new QLineEdit(this);
    edit_uri_->setPlaceholderText(tr("Playlist or Track URI"));

    radio_playlist_ = new QRadioButton("Playlist");
    radio_playlist_->setChecked(true);
    radio_track_ = new QRadioButton("Track");

    radio_repeat_off = new QRadioButton("Off");
    radio_repeat_off->setChecked(true);

    radio_repeat_track_ = new QRadioButton("Track");
    radio_repeat_context_ = new QRadioButton("Context");

    shuffle_checkbox_ = new QCheckBox("Shuffle", this);

    btn_submit_ = new QPushButton("Submit", this);
    connect(btn_submit_, &QPushButton::clicked,
            this, &SpotifyConfigureDialog::onSubmit);

    btn_cancel_ = new QPushButton("Cancel", this);
    connect(btn_cancel_, &QPushButton::clicked,
            this, &SpotifyConfigureDialog::reject);
}

void SpotifyConfigureDialog::initLayout()
{
    // mode group
    mode_button_group_ = new QButtonGroup;
    mode_button_group_->addButton(radio_playlist_);
    mode_button_group_->setId(radio_playlist_, SpotifyRemoteController::Settings::Playlist);
    mode_button_group_->addButton(radio_track_);
    mode_button_group_->setId(radio_track_, SpotifyRemoteController::Settings::Track);

    QGroupBox *mode_group = new QGroupBox(tr("Mode"));
    QHBoxLayout *mode_layout = new QHBoxLayout;
    mode_layout->addWidget(radio_playlist_);
    mode_layout->addWidget(radio_track_);
    mode_group->setLayout(mode_layout);


    QHBoxLayout *edit_layout = new QHBoxLayout;
    edit_layout->addWidget(edit_uri_);
    QGroupBox *edit_group = new QGroupBox("URI");
    edit_group->setLayout(edit_layout);

    repeat_button_group_ = new QButtonGroup;
    repeat_button_group_->addButton(radio_repeat_context_);
    repeat_button_group_->setId(radio_repeat_context_, SpotifyRemoteController::Context);
    repeat_button_group_->addButton(radio_repeat_track_);
    repeat_button_group_->setId(radio_repeat_track_, SpotifyRemoteController::Track);
    repeat_button_group_->addButton(radio_repeat_off);
    repeat_button_group_->setId(radio_repeat_off, SpotifyRemoteController::Off);

    QGroupBox *option_group = new QGroupBox("Options");
    QHBoxLayout *repeat_layout = new QHBoxLayout;
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
    root->addWidget(mode_group);
    root->addWidget(edit_group);
    root->addWidget(option_group);
    root->addLayout(finalize_layout, -1);

    setLayout(root);
}
