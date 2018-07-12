#include "tracker_picker.h"

#include "resources/lib.h"

TrackerPicker::TrackerPicker(QWidget* parent)
    : QComboBox(parent)
{
    addItem("");
    foreach(auto t, Resources::Lib::TRACKER_MODEL->getTrackers())
        addItem(t->getName());
    connect(Resources::Lib::TRACKER_MODEL, &TrackerTableModel::trackerAdded,
            this, &TrackerPicker::onTrackerAdded);
    connect(Resources::Lib::TRACKER_MODEL, &TrackerTableModel::trackerRemoved,
            this, &TrackerPicker::onTrackerRemoved);
}

void TrackerPicker::setCurrentTracker(const QString &n)
{
    for(int i = 0; i < count(); ++i) {
        if(itemText(i).compare(n) == 0) {
            setCurrentIndex(i);
            return;
        }
    }
}

void TrackerPicker::onTrackerAdded(const QString &n)
{
    addItem(n);
}

void TrackerPicker::onTrackerRemoved(const QString &n)
{
    for(int i = 0; i < count(); ++i) {
        if(itemText(i).compare(n) == 0) {
            removeItem(i);
            return;
        }
    }
}
