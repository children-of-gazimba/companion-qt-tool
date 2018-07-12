#ifndef INTERACTIVE_IMAGE_SHAPE_WIDGET_H
#define INTERACTIVE_IMAGE_SHAPE_WIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QRadioButton>

#include "interactive_image_shape.h"
#include "tracking/tracker_picker.h"

class InteractiveImageShapeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit InteractiveImageShapeWidget(QWidget *parent = nullptr);
    explicit InteractiveImageShapeWidget(InteractiveImageShape* shape, QWidget *parent = nullptr);
    virtual ~InteractiveImageShapeWidget();

    void setShape(InteractiveImageShape* token);
    InteractiveImageShape* getShape() const;

    void toggleCollapse();

signals:

public slots:
    void onSave();
    void onDelete();

private slots:
    void onCollapseTriggered();

private:
    void hideCollapsibleWidgets();
    void showCollapsibleWidgets();
    void contentsModifiedEvent();
    void updateUI();

    void initWidgets();
    void initLayout();

    InteractiveImageShape* shape_;
    QLabel* title_label_;
    QLabel* name_label_;
    QLineEdit* name_edit_;
    QLabel* tracker_label_;
    TrackerPicker* tracker_picker_;
    QPushButton* delete_button_;
    QPushButton* save_button_;
    QPushButton* collapse_button_;
    QRadioButton* link_select_;
    QRadioButton* grab_select_;

    QList<QWidget*> collapsible_widgets_;
};

#endif // INTERACTIVE_IMAGE_SHAPE_WIDGET_H
