#ifndef SOUND_LIST_PLAYBACK_VIEW_H
#define SOUND_LIST_PLAYBACK_VIEW_H

#include <QTableView>

#include <QPoint>
#include <QMouseEvent>
#include <QStandardItemModel>
#include <QPushButton>
#include <QMenu>

#include "db/table_records.h"
#include "misc/standard_item_model.h"

class SoundListPlaybackView : public QTableView
{
    Q_OBJECT
public:
    explicit SoundListPlaybackView(QList<SoundFileRecord*> const& sound_files, QWidget *parent = nullptr);
    explicit SoundListPlaybackView(QWidget *parent = nullptr);
    virtual ~SoundListPlaybackView();

    void setSoundFiles(QList<SoundFileRecord*> const&);
    void setEditable(bool);
    bool getEditable();

protected:
    QItemSelectionModel::SelectionFlags selectionCommand(const QModelIndex &index, const QEvent *event = 0) const;
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);

protected:
    virtual void setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command);

protected slots:
    void onPlayButtonClicked();

signals:
    void play(const SoundFileRecord&);
    void deleteSoundFileRequested(int id);

public slots:
    void addSoundFile(SoundFileRecord* rec);
    void onSoundFileAboutToBeDeleted(SoundFileRecord* rec);
    void onDropSuccessful();

private slots:
    void addSoundFile(int id, QString const& name, QString const& path);
    void onEntered(const QModelIndex&);
    void showCustomContextMenu(const QPoint&);
    void onDeleteAction();

protected:
    void performDrag();
    void initContextMenu();
    void init();

    QPoint start_pos_;
    StandardItemModel* model_;
    bool skip_select_;
    QPersistentModelIndex playable_index_;
    QIcon play_icon_;
    QMenu* context_menu_;
};

#endif // SOUND_LIST_PLAYBACK_VIEW_H
