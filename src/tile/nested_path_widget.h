#ifndef NESTED_PATH_WIDGET_H
#define NESTED_PATH_WIDGET_H

#include <QWidget>

#include <QLabel>
#include <QPushButton>
#include <QStack>
#include <QMap>
#include <QGraphicsScene>

class NestedPathWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NestedPathWidget(QWidget *parent = nullptr);

    void setPathText(const QStack<QGraphicsScene*>& scene_stack,
                       const QMap<QGraphicsScene*, QString> scene_names);
    const QString getPathText() const;

signals:
    void backButtonClicked();

public slots:

private:
    void initWidgets();
    void initLayout();

    QPushButton* back_button_;
    QLabel* path_label_;
};

#endif // NESTED_PATH_WIDGET_H
