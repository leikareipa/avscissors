/*
 * Tarpeeksi Hyvae Soft 2018 /
 * AV Scissors.
 *
 * Overloads QLabel to produce an 'activity strip'. The strip is effectively a
 * graphic (embedded in the label as a QPixmap) that shows which corresponding
 * portions of audio or video have activity and which don't.
 *
 * Think of the strip like this:
 *
 *      ::::::|||:::::||::::||||||||||||:::::::::::||:
 *
 * where the :s are areas without activity, and the |s are areas with activity.
 *
 * This class will generate, given relevant activity data, the strip's graphic,
 * and embed it into the label that has been promoted to it.
 *
 */

#include <QResizeEvent>
#include <QImage>
#include <QDebug>
#include "../../src/gui_qt/qt_activity_strip.h"
#include "../../src/common.h"

ActivityStrip::ActivityStrip(QWidget *parent) : QLabel(parent)
{
    this->setStyleSheet("background-color: #434343;");

    // Trust that the activity strip's parent widget handles mouse events, so that
    // the strip doesn't have to.
    this->setAttribute(Qt::WA_TransparentForMouseEvents);

    // Require that the widget always be at least this size. In practice, the
    // program's layout will probably cause the strip to always be no larger,
    // either, so this is effectively a fixed size.
    this->resize(this->width(), 45);
    this->setMinimumHeight(this->height());

    /// Kludge. Without this, can't resize window properly horizontally.
    this->setMinimumWidth(1);

    return;
}

// Assign the colors that will be used to draw the activity strip.
//
void ActivityStrip::set_colors(const QColor activeColor, const QColor inactiveColor, const QColor unknownColor)
{
    this->activeColor = activeColor;
    this->inactiveColor = inactiveColor;
    this->unknownColor = unknownColor;
    this->noDataColor = inactiveColor;

    return;
}

// Give the strip a pointer to the activity data from which it will create its
// graphic.
//
void ActivityStrip::set_strip_data_ptr(const QVector<video_activity_c::activity_type_e> *const data)
{
    this->activityData = data;
    if (this->activityData == nullptr)
    {
        this->clear();  // Remove the strip's previous QPixmap graphic.
    }

    // Create and embed a new QPixmap, based on the (supposedly) new data pointer.
    this->regenerate_activity_strip();

    return;
}

void ActivityStrip::resizeEvent(QResizeEvent *)
{
    this->regenerate_activity_strip();

    return;
}

// Looks at the current activity data, and generates a corresponding activity strip
// as an embedded QPixmap.
//
void ActivityStrip::regenerate_activity_strip(void)
{
    if (this->activityData == nullptr)
    {
        INFO(("Ignoring regeneration of activity strip; not data to regenerate."));
        return;
    }

    // Generate the strip image. Note that we generate it with a height of 1 pixel
    // only - it'll be scaled to fit the strip vertically when assigned as a QPixmap.
    {
        QImage stripImage(this->size().width(), 1, QImage::Format_ARGB32_Premultiplied);

        const real binWidth = (activityData->size() / real(stripImage.size().width()));

        for (int x = 0; x < stripImage.size().width(); x++)
        {
            const uint frameIdx = (x * binWidth);

            // Decide the color to draw the current vertical stripe in the strip with,
            // based on whether there's activity in the data at the corresponding index.
            QColor c = this->unknownColor;
            {
                c = (activityData->at(frameIdx) == video_activity_c::activity_type_e::Uninitialized)? this->unknownColor
                                                                                                    : this->inactiveColor;

                if (activityData->at(0) == video_activity_c::activity_type_e::NoData)
                {
                    c = noDataColor;
                }
                else
                {
                    for (uint i = 0; i < binWidth; i++)
                    {
                        if (activityData->at(frameIdx + i) == video_activity_c::activity_type_e::Active)
                        {
                            c = this->activeColor;
                            break;
                        }
                    }
                }
            }

            stripImage.setPixel(x, 0, qRgb(c.red(), c.green(), c.blue()));
        }

        this->setPixmap(QPixmap::fromImage(stripImage).scaled(this->size()));
    }

    return;
}

QColor ActivityStrip::activity_color() const
{
    return this->activeColor;
}
