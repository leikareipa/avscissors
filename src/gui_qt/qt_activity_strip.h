/*
 * Tarpeeksi Hyvae Soft 2018 /
 * AV Scissors.
 *
 */

#ifndef ACTIVITY_STRIP_H
#define ACTIVITY_STRIP_H

#include <QImage>
#include <QLabel>
#include "../../src/video/video_activity.h"

class ActivityStrip : public QLabel
{
    Q_OBJECT

public:
    explicit ActivityStrip(QWidget *parent = 0);

    void set_colors(const QColor activeColor, const QColor inactiveColor, const QColor unknownColor);

    void regenerate_activity_strip(void);

    QColor activity_color(void) const;

signals:

public slots:
    void set_strip_data_ptr(const QVector<video_activity_c::activity_type_e> *const _data);

private:
    void resizeEvent(QResizeEvent *event);

    // A pointer to the activity data from which this strip will generate its
    // user-facing graphic.
    const QVector<video_activity_c::activity_type_e> *activityData = nullptr;

    // Graph colors. These may be changed by the user, later.
    QColor activeColor = QColor("green");
    QColor inactiveColor = QColor("dimgray");
    QColor unknownColor = QColor("black");      // For areas of the strip that haven't yet been built.
    QColor noDataColor = QColor("#4D292D");     // For when we don't have valid data.
};

#endif
