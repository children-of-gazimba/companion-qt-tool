#ifndef SPOTIFY_CONFIGURE_DIALOG_H
#define SPOTIFY_CONFIGURE_DIALOG_H

#include <QLineEdit>
#include <QSlider>
#include <QRadioButton>
#include <QButtonGroup>
#include <QCheckBox>

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

        const QJsonDocument &getPlaybackInfo() const;

    signals:

    public slots:
        void onSubmit();

    private slots:
        void onTextChanged(const QString&);

    private:
        void updateUI();

        void initWidgets();
        void initLayout();

        QButtonGroup *mode_button_group_;
        QRadioButton *radio_playlist_;
        QRadioButton *radio_track_;

        QLineEdit *edit_uri_;

        QButtonGroup *repeat_button_group_;
        QRadioButton *radio_repeat_off;
        QRadioButton *radio_repeat_track_;
        QRadioButton *radio_repeat_context_;

        QCheckBox *shuffle_checkbox_;

        QPushButton *btn_submit_;
        QPushButton *btn_cancel_;

//        QSlider *volume_slider_;

        QJsonDocument playback_info_;
        SpotifyRemoteController::Settings settings_;

};

#endif // SPOTIFY_CONFIGURE_DIALOG_H
