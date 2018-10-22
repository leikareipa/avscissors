/*
 * Tarpeeksi Hyvae Soft 2018 /
 * AV Scissors
 *
 */

#ifndef VIDEO_H_
#define VIDEO_H_

#include <QObject>
#include "../../src/video/video_activity.h"
#include "../../src/video/video_info.h"

class video_player_c;

class video_object_c : public QObject
{
    Q_OBJECT

public:
    video_object_c(const QString filename, const messager_c *const messager);
    ~video_object_c(void);

    void assign_to_player(video_player_c *const player);

    const video_info_c& info(void) const;

    const video_activity_c& activity(void) const;

    const QVector<video_activity_c::activity_type_e>& video_activity_data(void) const;
    const QVector<video_activity_c::activity_type_e>& audio_activity_data(void) const;

signals:
    void message_to_user(const QString message);

private:
    const QString videoFilename;
    const video_info_c videoInfo;
    const video_activity_c videoActivity;

    // Which player this video has been assigned to.
    video_player_c *associatedPlayer = nullptr;
};

#endif
