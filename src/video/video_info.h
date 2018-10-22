/*
 * Tarpeeksi Hyvae Soft 2018 /
 * AV Scissors
 *
 */

#ifndef VIDEO_FILE_INFO_H
#define VIDEO_FILE_INFO_H

#include <QFileInfo>
#include <QString>
#include <QVector>
#include <QImage>
#include "../../src/types.h"

class audio_file_c;
class messager_c;

class video_info_c : public QObject
{
    Q_OBJECT

public:
    video_info_c(const QString videoFilename, const messager_c *const messager);
    ~video_info_c(void);

    const QString& file_name(void) const;

    const QString file_name_sans_path(void) const;

    real frame_rate(void) const;

    uint duration_ms(void) const;

    uint width(void) const;

    uint height(void) const;

    uint num_frames(void) const;

    bool is_valid_video(void) const;

signals:
    void message_to_user(const QString message);

private:
    // The video's filename.
    const QString filename;

    // Metadata about the video.
    QSize resolution = QSize();
    uint numVideoFrames = 0;
    real framerate = 0;
    uint durationMs = 0;

    // Will be set to true if we were able to successfully load the video.
    bool videoIsValid = false;
};

#endif
