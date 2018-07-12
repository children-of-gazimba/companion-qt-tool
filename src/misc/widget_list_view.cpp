#include "widget_list_view.h"

#include "misc/container.h"

WidgetListView::WidgetListView(QWidget *parent)
    : QScrollArea(parent)
    , scroll_layout_(0)
    , contents_(0)
{
    init();
}

WidgetListView::~WidgetListView()
{
    if(contents_)
        contents_->deleteLater();
}

void WidgetListView::addWidget(QWidget* w)
{
    scroll_layout_->addWidget(w);
}

void WidgetListView::removeWidget(QWidget* w)
{
    scroll_layout_->addWidget(w);
}

const QList<QWidget *> WidgetListView::getWidgets() const
{
    QList<QWidget*> child_widgets;
    QWidget* temp;
    for(int i = 0; i < scroll_layout_->count(); ++i) {
        temp = scroll_layout_->itemAt(i)->widget();
        if(temp)
            child_widgets.append(temp);
    }
    return child_widgets;
}

void WidgetListView::init()
{
    scroll_layout_ = new QVBoxLayout;
    scroll_layout_->setSizeConstraint(QLayout::SetMinAndMaxSize);
    contents_ = new Container;
    contents_->setLayout(scroll_layout_);
    setWidget(contents_);
    setWidgetResizable(true);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
}
