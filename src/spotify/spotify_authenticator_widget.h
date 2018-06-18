#ifndef SPOTIFY_AUTHENTICATORWIDGET_H
#define SPOTIFY_AUTHENTICATORWIDGET_H

#include <QWidget>

#include <QPushButton>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QSlider>

//#include <QWebEngineView>

#include <spotify/spotify_remote_controller.h>

class SpotifyAuthenticatorWidget : public QWidget
{
        Q_OBJECT
    public:

        explicit SpotifyAuthenticatorWidget(QWidget *parent = nullptr);

    signals:

    public slots:
        void onGranted();
        //void onAuthorize(const QUrl &url);
        void onResponse();

        void onPlayPlaylist();
        void onPlayTrack();

        void onPlay();
        void onPause();
        void onNext();
        void onPrev();
        void onEnableShuffle();
        void onDisableShuffle();

        void onSetRepeatTrack();
        void onSetRepeatContext();
        void onSetRepeatOff();
        void onSetVolume(int value);

    private:
        QPushButton * createButton(const QString &title, bool enabled, QWidget *parent);
        void initLayout();

    private:

        QPushButton *btn_play_playlist_;
        QPushButton *btn_playlist_info;
        QPushButton *btn_playlist_tracks;
        QPushButton *btn_play_track_;

        QPushButton *btn_ctrl_play_;
        QPushButton *btn_ctrl_pause_;
        QPushButton *btn_ctrl_next_;
        QPushButton *btn_ctrl_prev_;
        QPushButton *btn_ctrl_enable_shuffle;
        QPushButton *btn_ctrl_disable_shuffle;
        QPushButton *btn_ctrl_set_repeat_track;
        QPushButton *btn_ctrl_set_repeat_context;
        QPushButton *btn_ctrl_set_repeat_off;

        QLineEdit *edit_playlist_id_;
        QLineEdit *edit_track_id;

        QSlider *volume_slider_;

        QPlainTextEdit *response_plain_text_;

        //QWebEngineView *web_view_;
};

#endif // SPOTIFY_AUTHENTICATORWIDGET_H
