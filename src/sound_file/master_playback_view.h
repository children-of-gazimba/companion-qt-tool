#ifndef MASTER_PLAYBACK_VIEW_H
#define MASTER_PLAYBACK_VIEW_H

#include "playback_view.h"

#include <QMenu>

class MasterPlaybackView : public PlaybackView
{
    Q_OBJECT
public:
    explicit MasterPlaybackView(QList<DB::SoundFileRecord*> const& sound_files = QList<DB::SoundFileRecord*>(), QWidget *parent = 0);
    ~MasterPlaybackView();

signals:
    void deleteSoundFileRequested(int id);

public slots:

protected slots:
    void showCustomContextMenu(const QPoint&);
    void onDeleteAction();

protected:
    virtual void initContextMenu();

    QMenu* context_menu_;
};

#endif // MASTER_PLAYBACK_VIEW_H
