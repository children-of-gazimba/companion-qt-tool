#ifndef SOUND_LIST_VIEW_DIALOG_H
#define SOUND_LIST_VIEW_DIALOG_H

#include <QDialog>
#include <QPushButton>

#include "sound_list_view.h"

class SoundListViewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SoundListViewDialog(const QList<SoundFileRecord*>& records, QWidget *parent = 0);
    explicit SoundListViewDialog(QWidget *parent = 0);

signals:

public slots:

private:
    void initWidgets(const QList<SoundFileRecord*>& records = QList<SoundFileRecord*>());
    void initLayout();

    SoundListView* list_view_;
    QPushButton* ok_;
    QPushButton* cancel_;
};

#endif // SOUND_LIST_VIEW_DIALOG_H
