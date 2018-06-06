#include "spotify_authenticator_widget.h"

#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QLabel>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPlainTextEdit>
#include <QWidget>
#include <QSlider>

#include "spotify/spotify_handler.h"

SpotifyAuthenticatorWidget::SpotifyAuthenticatorWidget(QWidget *parent)
    : QWidget(parent)
{
    initLayout();

    web_view_ = new QWebEngineView;

    // handler connections
    connect(&SpotifyHandler::instance()->remote, &SpotifyRemoteController::accessGranted,
            this, &SpotifyAuthenticatorWidget::onGranted);

    connect(&SpotifyHandler::instance()->remote, &SpotifyRemoteController::authorizeWithBrowser,
            this, &SpotifyAuthenticatorWidget::onAuthorize);

//    connect(&SpotifyHandler::instance()->remote, &SpotifyRemoteController::authorizeWithBrowser,
//            QDesktopS
//           ;

//    connect(&SpotifyHandler::instance()->remote, &SpotifyRemoteController::serverResponse,
//            this, &SpotifyAuthenticatorWidget::onResponse);

    // ui connections
    connect(btn_play_playlist_, &QPushButton::clicked,
            this, &SpotifyAuthenticatorWidget::onPlayPlaylist);

    connect(btn_play_track_, &QPushButton::clicked,
            this, &SpotifyAuthenticatorWidget::onPlayTrack);

    connect(btn_ctrl_play_, &QPushButton::clicked,
            this, &SpotifyAuthenticatorWidget::onPlay);

    connect(btn_ctrl_pause_, &QPushButton::clicked,
            this, &SpotifyAuthenticatorWidget::onPause);

    connect(btn_ctrl_next_, &QPushButton::clicked,
            this, &SpotifyAuthenticatorWidget::onNext);

    connect(btn_ctrl_prev_, &QPushButton::clicked,
            this, &SpotifyAuthenticatorWidget::onPrev);

    connect(btn_ctrl_enable_shuffle, &QPushButton::clicked,
            this, &SpotifyAuthenticatorWidget::onEnableShuffle);

    connect(btn_ctrl_disable_shuffle, &QPushButton::clicked,
            this, &SpotifyAuthenticatorWidget::onDisableShuffle);

    connect(btn_ctrl_set_repeat_context, &QPushButton::clicked,
            this, &SpotifyAuthenticatorWidget::onSetRepeatContext);

    connect(btn_ctrl_set_repeat_track, &QPushButton::clicked,
            this, &SpotifyAuthenticatorWidget::onSetRepeatTrack);

    connect(btn_ctrl_set_repeat_off, &QPushButton::clicked,
            this, &SpotifyAuthenticatorWidget::onSetRepeatOff);

    connect(volume_slider_, &QSlider::valueChanged,
            this, &SpotifyAuthenticatorWidget::onSetVolume);


    // grant access to spotify
    SpotifyHandler::instance()->remote.grantAccess();

}

void SpotifyAuthenticatorWidget::initLayout() {

    QHBoxLayout *playlist_edit_layout = new QHBoxLayout;
    edit_playlist_id_ = new QLineEdit(this);
    btn_play_playlist_ = createButton("Play Playlist", false, this);
    playlist_edit_layout->addWidget(edit_playlist_id_);
    playlist_edit_layout->addWidget(btn_play_playlist_);

    QHBoxLayout *track_edit_layout = new QHBoxLayout;
    edit_track_id = new QLineEdit(this);
    btn_play_track_ = createButton("Play Track", false, this);
    track_edit_layout->addWidget(edit_track_id);
    track_edit_layout->addWidget(btn_play_track_);

    QVBoxLayout *button_layout = new QVBoxLayout;
    btn_ctrl_play_ = createButton("Play", false, this);
    btn_ctrl_pause_ = createButton("Pause", false, this);
    btn_ctrl_next_= createButton(">>", false, this);
    btn_ctrl_prev_= createButton("<<" , false, this);
    btn_ctrl_enable_shuffle= createButton("Enable Shuffle", false, this);
    btn_ctrl_disable_shuffle= createButton("Disable Shuffle", false, this);
    btn_ctrl_set_repeat_track= createButton("Repeat Track", false, this);
    btn_ctrl_set_repeat_context= createButton("Repeat Context", false, this);
    btn_ctrl_set_repeat_off= createButton("Repeat Off", false, this);

    button_layout->addWidget(btn_ctrl_play_);
    button_layout->addWidget(btn_ctrl_pause_);
    button_layout->addWidget(btn_ctrl_next_);
    button_layout->addWidget(btn_ctrl_prev_);
    button_layout->addWidget(btn_ctrl_enable_shuffle);
    button_layout->addWidget(btn_ctrl_disable_shuffle);
    button_layout->addWidget(btn_ctrl_set_repeat_track);
    button_layout->addWidget(btn_ctrl_set_repeat_context);
    button_layout->addWidget(btn_ctrl_set_repeat_off);

    QVBoxLayout *volume_layout = new QVBoxLayout;
    volume_slider_ = new QSlider(Qt::Orientation::Horizontal, this);
    volume_layout->addWidget(volume_slider_, 10);

    QVBoxLayout *ui_layout = new QVBoxLayout;
    ui_layout->addLayout(playlist_edit_layout);
    ui_layout->addLayout(track_edit_layout);
    ui_layout->addLayout(button_layout);
    ui_layout->addLayout(volume_layout);

    response_plain_text_ = new QPlainTextEdit(this);

    QVBoxLayout *root = new QVBoxLayout;
    root->addLayout(ui_layout);
    root->addWidget(response_plain_text_, 10);

    setLayout(root);
}


QPushButton* SpotifyAuthenticatorWidget::createButton(const QString &title, bool enabled, QWidget *parent) {
    QPushButton *button = new QPushButton(title, parent);
    button->setEnabled(enabled);
    return button;
}

void SpotifyAuthenticatorWidget::onGranted() {
    btn_play_playlist_->setEnabled(true);
    btn_play_track_->setEnabled(true);
    btn_ctrl_play_->setEnabled(true);
    btn_ctrl_pause_->setEnabled(true);
    btn_ctrl_next_->setEnabled(true);
    btn_ctrl_prev_->setEnabled(true);
    btn_ctrl_enable_shuffle->setEnabled(true);
    btn_ctrl_disable_shuffle->setEnabled(true);
    btn_ctrl_set_repeat_track->setEnabled(true);
    btn_ctrl_set_repeat_context->setEnabled(true);
    btn_ctrl_set_repeat_off->setEnabled(true);
}

void SpotifyAuthenticatorWidget::onAuthorize(const QUrl &url) {
    web_view_->load(url);
    web_view_->show();
}

void SpotifyAuthenticatorWidget::onResponse() {
//    QString type_str;
//    switch(response.type) {
//    case SpotifyRemoteController::PLAYLIST_INFO   : type_str="PLAYLIST_INFO"; break;
//    case SpotifyRemoteController::PLAYLIST_TRACKS : type_str="PLAYLIST_TRACKS"; break;
//    case SpotifyRemoteController::TRACK_INFO      : type_str="TRACK_INFO"; break;
//    case SpotifyRemoteController::ERROR           : type_str="ERROR"; break;
//    case SpotifyRemoteController::OTHER           : type_str="OTHER"; break;
//    case SpotifyRemoteController::PLAY            : type_str="PLAY"; break;
//    case SpotifyRemoteController::PAUSE           : type_str="PAUSE"; break;
//    case SpotifyRemoteController::NEXT            : type_str="NEXT"; break;
//    case SpotifyRemoteController::PREVIOUS        : type_str="PREVIOUS"; break;
//    case SpotifyRemoteController::SHUFFLE         : type_str="SHUFFLE"; break;
//    case SpotifyRemoteController::SET_VOLUME      : type_str="SET_VOLUME"; break;
//    case SpotifyRemoteController::PLAY_TRACK      : type_str="PLAY_TRACK"; break;
//    case SpotifyRemoteController::PLAY_PLAYLIST   : type_str="PLAY_PLAYLIST"; break;
//    case SpotifyRemoteController::REPEAT_TRACK    : type_str="REPEAT_TRACK"; break;
//    case SpotifyRemoteController::REPEAT_CONTEXT  : type_str="REPEAT_CONTEXT"; break;
//    case SpotifyRemoteController::REPEAT_OFF      : type_str="REPEAT_OFF"; break;
//    default: break;
//    }
//    qDebug().nospace() << Q_FUNC_INFO << " :" << __LINE__;
//    qDebug() << "  >" << "response type:" << response.type << ":" << type_str;
//    qDebug() << "  >" << "response content:" << response.message;

//    response_plain_text_->setPlainText(QString::number(response.type));
//    response_plain_text_->setPlainText("yeah");
//    response_plain_text_->setPlainText(response.message.toJson());
    //    response_plain_text_->setPlainText("==================================");
}

void SpotifyAuthenticatorWidget::onPlayPlaylist() {
    QString uri = edit_playlist_id_->text();

    SpotifyHandler::instance()->remote.getPlaylistInfo(uri);
    SpotifyHandler::instance()->remote.getPlaylistTracks(uri);

    SpotifyHandler::instance()->remote.playUserPlaylist(uri);
}

void SpotifyAuthenticatorWidget::onPlayTrack() {
    QString uri = edit_track_id->text();
    SpotifyHandler::instance()->remote.playTrack(uri);
}

void SpotifyAuthenticatorWidget::onPlay() {
    SpotifyHandler::instance()->remote.play();
    //    SpotifyHandler::instance()->request_handler_->play();
}

void SpotifyAuthenticatorWidget::onPause() {
    SpotifyHandler::instance()->remote.pause();
}

void SpotifyAuthenticatorWidget::onNext() {
    SpotifyHandler::instance()->remote.next();
}

void SpotifyAuthenticatorWidget::onPrev() {
    SpotifyHandler::instance()->remote.prev();
}

void SpotifyAuthenticatorWidget::onEnableShuffle() {
    SpotifyHandler::instance()->remote.setShuffle(true);
}

void SpotifyAuthenticatorWidget::onDisableShuffle() {
    SpotifyHandler::instance()->remote.setShuffle(false);
}

void SpotifyAuthenticatorWidget::onSetRepeatContext() {
    SpotifyHandler::instance()->remote.setRepeat(SpotifyRemoteController::RepeatMode::Context);
}

void SpotifyAuthenticatorWidget::onSetRepeatTrack() {
    SpotifyHandler::instance()->remote.setRepeat(SpotifyRemoteController::RepeatMode::Track);
}

void SpotifyAuthenticatorWidget::onSetRepeatOff() {
    SpotifyHandler::instance()->remote.setRepeat(SpotifyRemoteController::RepeatMode::Off);
}

void SpotifyAuthenticatorWidget::onSetVolume(int value) {
    SpotifyHandler::instance()->remote.setVolume(value);
}
