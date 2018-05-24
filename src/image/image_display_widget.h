#ifndef IMAGE_DISPLAY_WIDGET_H
#define IMAGE_DISPLAY_WIDGET_H

#include <QWidget>
#include <QMenuBar>

namespace Image {
class View;
}

class ImageDisplayWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ImageDisplayWidget(QWidget *parent = nullptr);

    Image::View* getView() const;

signals:

public slots:

private slots:
    void onItemSet();
    void onToggleFullscreen();
    void onZoomIn();
    void onZoomOut();

private:
    void keyPressEvent(QKeyEvent *event);

    void initWidgets();
    void initMenu();
    void initLayout();

    QMenuBar* menu_bar_;
    QMenu* view_menu_;
    Image::View* view_;
};

#endif // IMAGE_DISPLAY_WIDGET_H
