#ifndef TRACKER_PICKER_H
#define TRACKER_PICKER_H

#include <QComboBox>

class TrackerPicker : public QComboBox
{
    Q_OBJECT
public:
    TrackerPicker(QWidget* parent = 0);

    void setCurrentTracker(const QString&);

protected slots:
    void onTrackerAdded(const QString&);
    void onTrackerRemoved(const QString&);
};

#endif // TRACKER_PICKER_H
