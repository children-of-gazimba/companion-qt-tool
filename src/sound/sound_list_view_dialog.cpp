#include "sound_list_view_dialog.h"

#include <QHBoxLayout>

SoundListViewDialog::SoundListViewDialog(const QList<SoundFileRecord *> &records, QWidget *parent)
    : QDialog(parent)
    , list_view_(0)
    , ok_(0)
    , cancel_(0)
{
    setWindowTitle(tr("Playlist Contents"));

    initWidgets(records);
    initLayout();
}

SoundListViewDialog::SoundListViewDialog(QWidget *parent)
    : QDialog(parent)
    , list_view_(0)
    , ok_(0)
    , cancel_(0)
{
    setWindowTitle(tr("Playlist Contents"));

    initWidgets();
    initLayout();
}

void SoundListViewDialog::initWidgets(const QList<SoundFileRecord *> &records)
{
    list_view_ = new SoundListView(records, this);
    ok_ = new QPushButton(tr("OK"), this);
    cancel_ = new QPushButton(tr("Cancel"), this);

    connect(ok_, SIGNAL(pressed()),
            this, SLOT(accept()));
    connect(cancel_, SIGNAL(pressed()),
            this, SLOT(reject()));
}

void SoundListViewDialog::initLayout()
{
    QVBoxLayout* layout = new QVBoxLayout;

    QHBoxLayout* button_layout = new QHBoxLayout;
    button_layout->addStretch(2);
    button_layout->addWidget(ok_, 1);
    button_layout->addWidget(cancel_, 1);

    layout->addWidget(list_view_, 4);
    layout->addLayout(button_layout, 0);

    setLayout(layout);
}
