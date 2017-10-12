#ifndef TWO_D_PLAYLIST_PLAYER_TILE_H
#define TWO_D_PLAYLIST_PLAYER_TILE_H

#include <QMediaPlayer>
#include <QGraphicsView>
#include <QMimeData>
#include <QDrag>

#include "custom_media_player.h"
#include "tile.h"
#include "playlist/settings_widget.h"
#include "playlist/media_playlist.h"
#include "playlist/settings.h"
#include "misc/json_mime_data_parser.h"
#include "db/model/sound_file_table_model.h"

namespace TwoD {

class PlaylistPlayerTile : public Tile
{
    Q_OBJECT

public:
    PlaylistPlayerTile(QGraphicsItem* parent = 0);
    virtual ~PlaylistPlayerTile();

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    virtual void receiveExternalData(const QMimeData* data);

    bool addMedia(const DB::SoundFileRecord& r);
    bool addMedia(int record_id);

    void setSoundFileModel(DB::Model::SoundFileTableModel* m);
    DB::Model::SoundFileTableModel* getSoundFileModel();

    /**
     * Returns a QJsonObject holding all information about the tile
    */
    virtual const QJsonObject toJsonObject() const;

    /**
     * Set all values held by JSON object.
     * Returns success of parsing JsonObject.
    */
    virtual bool setFromJsonObject(const QJsonObject& obj);

    /**
     * @brief see BC
     */
    QString const getClassName() const;
public slots:
    virtual void setMedia(const QMediaContent& c);
    virtual void play();
    virtual void stop();
    virtual void onActivate();
    void setVolume(int volume);
    int getVolume() const;

    /** adjust playing icon when the player stops playing */
    void changePlayerState(QMediaPlayer::State state);

    /* adjust playing icon when the player stops playing*/
    void changedCustomPlayerActivation(bool state);

protected slots:
    /** slot to open configuration */
    virtual void onConfigurePlaylist();
    /* */
    void closePlaylistSettings();
    void savePlaylistSettings(Playlist::Settings* settings);

    /** slot to open contents view */
    virtual void onContents();

protected:
    /*
     * BC overrides
    */
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* e);

    /**
     * creates context menu
    */
    virtual void createContextMenu();

    /**
     * Returns the image based on the playback state
    */
    virtual const QPixmap getPlayStatePixmap() const;

    void setIsPlaying(bool);

    CustomMediaPlayer* player_;

    Playlist::SettingsWidget* playlist_settings_widget_;
    Playlist::MediaPlaylist* playlist_;
    DB::Model::SoundFileTableModel* model_;

    bool is_playing_;
};

} // namespace TwoD

#endif // TWO_D_PRESET_PLAYER_TILE_H

