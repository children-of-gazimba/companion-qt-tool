#ifndef SOUND_FILE_LIST_VIEW_DIALOG_H
#define SOUND_FILE_LIST_VIEW_DIALOG_H

#include <QDialog>
#include <QPushButton>

#include "list_view.h"

namespace SoundFile {

class ListViewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ListViewDialog(const QList<DB::SoundFileRecord*>& records, QWidget *parent = 0);
    explicit ListViewDialog(QWidget *parent = 0);

signals:

public slots:

private:
    void initWidgets(const QList<DB::SoundFileRecord*>& records = QList<DB::SoundFileRecord*>());
    void initLayout();

    ListView* list_view_;
    QPushButton* ok_;
    QPushButton* cancel_;
};

} // namespace SoundFile

#endif // SOUND_FILE_LIST_VIEW_DIALOG_H
