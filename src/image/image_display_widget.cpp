#include "image_display_widget.h"

#include <QVBoxLayout>

#include "view.h"

ImageDisplayWidget::ImageDisplayWidget(QWidget *parent)
    : QWidget(parent)
    , menu_bar_(0)
    , view_menu_(0)
    , view_(0)
{
    initWidgets();
    initMenu();
    initLayout();
}

Image::View *ImageDisplayWidget::getView() const
{
    return view_;
}

void ImageDisplayWidget::onItemSet()
{
    view_menu_->clear();
    view_menu_->addActions(view_->getMenuBarExtension()->actions());
    view_menu_->addSeparator();
    QAction* zoom_in = view_menu_->addAction(tr("Zoom In"));
    connect(zoom_in, &QAction::triggered,
            this, &ImageDisplayWidget::onZoomIn);
    QAction* zoom_out = view_menu_->addAction(tr("Zoom Out"));
    connect(zoom_out, &QAction::triggered,
            this, &ImageDisplayWidget::onZoomOut);
}

void ImageDisplayWidget::onToggleFullscreen()
{
    if (windowState().testFlag(Qt::WindowFullScreen)) {
        menu_bar_->show();
        showNormal();
    }
    else {
        menu_bar_->hide();
        showFullScreen();
    }
}

void ImageDisplayWidget::onZoomIn()
{
    view_->scale(1.1,1.1);
}

void ImageDisplayWidget::onZoomOut()
{
    view_->scale(0.9,0.9);
}

void ImageDisplayWidget::keyPressEvent(QKeyEvent *event)
{
    switch(event->key()) {
        case '-':
            onZoomOut();
            break;
        case '+':
            onZoomIn();
            break;
        case ' ':
            onToggleFullscreen();
            break;
    }
    QWidget::keyPressEvent(event);
}

void ImageDisplayWidget::initWidgets()
{
    view_ = new Image::View(this);
    menu_bar_ = new QMenuBar(this);

    connect(view_, SIGNAL(itemSet()),
            this, SLOT(onItemSet()));
}

void ImageDisplayWidget::initMenu()
{
    view_menu_ = menu_bar_->addMenu(tr("View"));
    QMenu* window_menu = new QMenu(tr("Window"), this);
    QAction* toggle_full = window_menu->addAction(tr("Toggle Fullscreen"));
    connect(toggle_full, &QAction::triggered,
            this, &ImageDisplayWidget::onToggleFullscreen);
}

void ImageDisplayWidget::initLayout()
{
    QVBoxLayout* layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(menu_bar_, -1);
    layout->addWidget(view_, 10);
    setLayout(layout);
}
