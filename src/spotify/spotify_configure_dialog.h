#ifndef SPOTIFY_CONFIGURE_DIALOG_H
#define SPOTIFY_CONFIGURE_DIALOG_H

#include <QLineEdit>
#include <QSlider>

#include <QDialog>

#include "spotify_remote_controller.h"

class SpotifyConfigureDialog : public QDialog
{
        Q_OBJECT
    public:
        explicit SpotifyConfigureDialog(QWidget *parent = nullptr);
        explicit SpotifyConfigureDialog(const SpotifyRemoteController::Settings &settings,
                                        QWidget *parent = nullptr);

        const SpotifyRemoteController::Settings &getSettings() const;
        void setSettings(const SpotifyRemoteController::Settings & settings);

    signals:

    public slots:
        void onAddPlaylist();
        void onAddTrack();

    private:
        void initWidgets();
        void initLayout();

        QPushButton *btn_submit_;
        QPushButton *btn_cancel_;

        QLineEdit *edit_playlist_id_;
        QLineEdit *edit_track_id_;

        QPushButton *btn_playlist_info_;
        QPushButton *btn_track_info_;

//        QPushButton *btn_ctrl_shuffle_mode_;
//        QPushButton *btn_ctrl_set_repeat_mode_;

//        QSlider *volume_slider_;

        SpotifyRemoteController::Settings settings_;
};

#endif // SPOTIFY_CONFIGURE_DIALOG_H
