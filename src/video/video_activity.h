/*
 * Tarpeeksi Hyvae Soft 2018 /
 * AV Scissors
 *
 */

#ifndef VIDEO_FRAME_ACTIVITY_H
#define VIDEO_FRAME_ACTIVITY_H

#include <QFuture>
#include <QObject>
#include <atomic>
#include "../../src/video/video_info.h"
#include "../../src/audio/audio_file.h"
#include "../../src/common.h"

namespace cv
{
    class Mat;
}

class video_activity_c : public QObject
{
    Q_OBJECT

    friend class video_object_c;

public:
    video_activity_c(const video_info_c &sourceVideo, const messager_c *const messager);
    ~video_activity_c(void);

    bool is_active_frame_at(const uint offs, const uint videoOrAudioOrBoth = 2/*2 means both*/) const;

    bool has_valid_audio(void) const;

    bool strip_build_has_finished(void) const;

    uint get_start_of_active_segment(const uint startFrameIdx, const uint videoOrAudio) const;

    // One of these identifiers will be assigned to each frame.
    enum class activity_type_e
    {
        NoData = -2,        // When we don't have data for this frame (e.g. for sound frames when no sound is present on the video).
        Uninitialized = -1, // The frame's activity type hasn't yet been computed.
        Inactive = 0,       // This frame doesn't differ notably from the previous one.
        Active = 1,         // This frame differs notably from the previous one.
    };

signals:
    void message_to_user(const QString message);

private:
    void mark_video_frame_activity(void);
    void mark_audio_frame_activity(void);

    bool frames_differ(const cv::Mat &frame1, const cv::Mat &frame2, const u8 threshold);

    // For each frame in the video, whether there's visual or acoustic activity.
    QVector<activity_type_e> videoFrameIsActive;
    QVector<activity_type_e> audioFrameIsActive;

    // For threading frame analysis.
    QFuture<void> videoStripThread;
    QFuture<void> audioStripThread;

    // Set to true to signal to any worker threads to quit their stuff.
    std::atomic<bool> workerThreadsShouldStop;

    const messager_c *const messager;

    const video_info_c &videoInfo;

    // The video's audio as a separate WAV data object.
    audio_file_c *audio = nullptr;

    void extract_audio(void);
};

#endif
