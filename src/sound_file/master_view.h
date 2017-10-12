#ifndef SOUND_FILE_MASTER_VIEW_H
#define SOUND_FILE_MASTER_VIEW_H

#include "list_view.h"

#include <QMenu>

namespace SoundFile {

class MasterView : public ListView
{
    Q_OBJECT
public:
    explicit MasterView(QList<DB::SoundFileRecord*> const& sound_files = QList<DB::SoundFileRecord*>(), QWidget *parent = 0);
    ~MasterView();

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

} // namespace SoundFile

#endif // SOUND_FILE_MASTER_VIEW_H
