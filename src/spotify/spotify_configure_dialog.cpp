#include "spotify_configure_dialog.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QPushButton>

SpotifyConfigureDialog::SpotifyConfigureDialog(QWidget *parent)
    : QDialog(parent)
    , settings_()
    , btn_submit_(0)
    , btn_cancel_(0)
    , btn_playlist_info_(0)
    , btn_track_info_(0)

{
    initWidgets();
    initLayout();
}

SpotifyConfigureDialog::SpotifyConfigureDialog(const SpotifyRemoteController::Settings &settings, QWidget *parent)
    : QDialog(parent)
    , settings_(settings)
    , btn_submit_(0)
    , btn_cancel_(0)
    , btn_playlist_info_(0)
    , btn_track_info_(0)
{
    initWidgets();
    initLayout();
}

const SpotifyRemoteController::Settings &SpotifyConfigureDialog::getSettings() const
{
    return settings_;
}

void SpotifyConfigureDialog::setSettings(const SpotifyRemoteController::Settings &settings)
{
    settings_ = settings;
    edit_playlist_id_->setText(settings.playlist_uri);
    edit_track_id_->setText(settings.track_uri);
}


void SpotifyConfigureDialog::onAddPlaylist()
{
    settings_.playlist_uri =edit_playlist_id_->text();

    settings_.track_uri = "";
    edit_track_id_->setText("");

    settings_.repeat_mode = SpotifyRemoteController::Context;
    settings_.shuffle_enabled = false;
}

void SpotifyConfigureDialog::onAddTrack()
{
    settings_.track_uri = edit_track_id_->text();

    settings_.playlist_uri = "";
    edit_playlist_id_->setText("");

    settings_.repeat_mode = SpotifyRemoteController::Context;
    settings_.shuffle_enabled = false;
}


void SpotifyConfigureDialog::initWidgets()
{
    edit_playlist_id_ = new QLineEdit(this);
    edit_playlist_id_->setPlaceholderText("Playlist URI");

    edit_track_id_ = new QLineEdit(this);
    edit_track_id_->setPlaceholderText("Track URI");

    btn_playlist_info_ = new QPushButton("Get Playlist Info", this);
    connect(btn_playlist_info_, &QPushButton::clicked,
            this, &SpotifyConfigureDialog::onAddPlaylist);

    btn_track_info_ = new QPushButton("Get Track Info");
    connect(btn_track_info_, &QPushButton::clicked,
            this, &SpotifyConfigureDialog::onAddTrack);

    btn_submit_ = new QPushButton("Submit", this);
    connect(btn_submit_, &QPushButton::clicked,
            this, &SpotifyConfigureDialog::accept);

    btn_cancel_ = new QPushButton("Cancel", this);
    connect(btn_cancel_, &QPushButton::clicked,
            this, &SpotifyConfigureDialog::reject);
}

void SpotifyConfigureDialog::initLayout()
{
    QHBoxLayout *add_playlist_layout = new QHBoxLayout;
    add_playlist_layout->addWidget(edit_playlist_id_);
    add_playlist_layout->addWidget(btn_playlist_info_);

    QHBoxLayout *add_track_layout = new QHBoxLayout;
    add_track_layout->addWidget(edit_track_id_);
    add_track_layout->addWidget(btn_track_info_);

    QHBoxLayout *finalize_layout = new QHBoxLayout;
    finalize_layout->addStretch(2);
    finalize_layout->addWidget(btn_submit_, 1);
    finalize_layout->addWidget(btn_cancel_, 1);

    QVBoxLayout *root = new QVBoxLayout;
    root->addLayout(add_playlist_layout);
    root->addLayout(add_track_layout);
    root->addLayout(finalize_layout, -1);

    setLayout(root);
}
