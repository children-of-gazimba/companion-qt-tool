#include "master_playback_view.h"

MasterPlaybackView::MasterPlaybackView(QList<DB::SoundFileRecord*> const& sound_files, QWidget *parent)
    : PlaybackView(sound_files, parent)
    , context_menu_(0)
{
    setEditable(true);
    setAcceptDrops(false);
    initContextMenu();
}

MasterPlaybackView::~MasterPlaybackView()
{}

void MasterPlaybackView::showCustomContextMenu(const QPoint& p)
{
    context_menu_->exec(mapToGlobal(p));
}

void MasterPlaybackView::onDeleteAction()
{
    QModelIndexList selection = this->selectionModel()->selectedIndexes();
    if(selection.size() == 0)
        return;

    int row = selection.first().row();
    int id = model_->data(model_->index(row, 0), Qt::UserRole).toInt();
    model_->removeRow(row);
    emit deleteSoundFileRequested(id);
}

void MasterPlaybackView::initContextMenu()
{
    setContextMenuPolicy(Qt::CustomContextMenu);

    context_menu_ = new QMenu(this);

    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(showCustomContextMenu(const QPoint&)));

    QList<QAction*> actions;
    actions.append(new QAction(tr("Delete"), context_menu_));
    connect(actions.back(), SIGNAL(triggered()),
            this, SLOT(onDeleteAction()));

    context_menu_->addActions(actions);
}
