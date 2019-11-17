#ifndef SOUND_LIST_PLAYBACK_VIEW_H
#define SOUND_LIST_PLAYBACK_VIEW_H

#include <QTableView>

#include <QPoint>
#include <QMouseEvent>
#include <QStandardItemModel>
#include <QPushButton>
#include <QMenu>

#include "data_types.h"
#include "misc/standard_item_model.h"
#include "model/sound_table_model.h"

class SoundListPlaybackView : public QTableView
{
    Q_OBJECT
public:
    explicit SoundListPlaybackView(QList<SoundData> const& sound_files, QWidget *parent = nullptr);
    explicit SoundListPlaybackView(QWidget *parent = nullptr);
    virtual ~SoundListPlaybackView();

    void setSounds(QList<SoundData> const&);
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
    void play(const SoundData&, const QString& server);
    void deleteSoundRequested(const QString& uuid);

public slots:
    void setServerName(const QString& name);
    void addSound(SoundData rec);
    void onSoundAboutToBeDeleted(SoundData rec);
    void onDropSuccessful();
    void clear();

private slots:
    void addSound(const QString &uuid, QString const& name, QString const& path);
    void onEntered(const QModelIndex&);
    void showCustomContextMenu(const QPoint&);
    void onDeleteAction();

protected:
    void applyServerConfig();
    void performDrag();
    void initContextMenu();
    void init();

    QPoint start_pos_;
    StandardItemModel* model_;
    bool skip_select_;
    QPersistentModelIndex playable_index_;
    QIcon play_icon_;
    QMenu* context_menu_;
    SoundTableModel* table_model_;
    QString server_name_;
};

#endif // SOUND_LIST_PLAYBACK_VIEW_H
