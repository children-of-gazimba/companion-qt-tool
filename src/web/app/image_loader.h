#ifndef WEB_APP_IMAGE_LOADER_H
#define WEB_APP_IMAGE_LOADER_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>

namespace Web {
namespace App {

class ImageLoader : public QWidget
{
    Q_OBJECT
public:
    explicit ImageLoader(QWidget *parent = 0);

    const QString loadImage();
    const QString getLastLoadedImage() const;
signals:
    void newImageLoaded(const QString);

private slots:
    void onButtonClicked();

public slots:

private:
    void initWidgets();
    void initLayout();

    QLineEdit* line_edit_;
    QPushButton* button_;
};

} // namespace App
} // namespace Web

#endif // WEB_APP_IMAGE_LOADER_H
