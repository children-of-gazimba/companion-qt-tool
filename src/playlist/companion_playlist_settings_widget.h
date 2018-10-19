#ifndef PLAYLIST_COMPANION_PLAYLIST_SETTINGS_WIDGET_H
#define PLAYLIST_COMPANION_PLAYLIST_SETTINGS_WIDGET_H

#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QGroupBox>

#include "companion_playlist.h"

class CompanionPlaylistSettingsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CompanionPlaylistSettingsWidget(const CompanionPlaylistSettings& settings, QWidget *parent = 0);
    ~CompanionPlaylistSettingsWidget();

signals:
    void closed();
    void saved(const CompanionPlaylistSettings& settings);
    void volumeSettingsChanged(int val);

public slots:
    void onCloseClicked(bool);
    void onSaveClicked(bool);
    void popOpen();
    void onExternalVolumeChanged(int v);

private slots:
    void onMinIntervalSliderChanged(int val);
    void onMaxIntervalSliderChanged(int val);
    void onVolumeSliderChanged(int val);
    void onOpenImage();

private:
    void initWidgets();
    void initLayout();
    void closeEvent(QCloseEvent*);

    CompanionPlaylistSettings settings_;
    QLineEdit* name_edit_;
    QCheckBox* loop_checkbox_;
    QGroupBox* interval_groupbox_;
    QSlider* min_interval_slider_;
    QSlider* max_interval_slider_;
    QLabel* interval_label_;
    QSlider* volume_slider_;
    QLabel* volume_label_;
    QRadioButton* normal_radio_button_;
    QRadioButton* shuffle_radio_button_;
    QRadioButton* weighted_radio_button_;
    QPushButton* save_button_;
    QPushButton* close_button_;
    QLineEdit* image_path_edit_;
    QPushButton* image_path_button_;
    QPushButton* image_clear_button_;
};

#endif // PLAYLIST_COMPANION_PLAYLIST_SETTINGS_WIDGET_H
