/*
 * Tarpeeksi Hyvae Soft 2018 /
 * AV Scissors.
 *
 * Loads up the given video, and polls OpenCV on it for some metadata, like frame
 * rate, duration, etc.
 *
 */

#include <QDebug>
#include <opencv2/highgui/highgui.hpp>
#include "../../src/messager/messager.h"
#include "../../src/audio/audio_file.h"
#include "../../src/video/video_info.h"
#include "../../src/common.h"

video_info_c::video_info_c(const QString videoFilename, const messager_c *const messager) :
    filename(videoFilename)
{
    connect(    this, &video_info_c::message_to_user,
            messager, &messager_c::new_message);

    INFO(("Loading video file '%s'...", this->file_name().toStdString().c_str()));

    if (videoFilename.contains("\""))
    {
        emit message_to_user("Can't load files whose names have double quotes.");
        this->videoIsValid = false;
        return;
    }

    if (this->file_name().isEmpty())
    {
        emit message_to_user("This file has an invalid name.");
        this->videoIsValid = false;
        return;
    }

    // Initialize the video information.
    {
        cv::VideoCapture video(this->file_name().toStdString());
        if (!video.isOpened())
        {
            emit message_to_user("That is not a supported video file.");
            this->videoIsValid = false;
            return;
        }

        if ((int)video.get(CV_CAP_PROP_FRAME_COUNT) <= 0)
        {
            emit message_to_user("That is not a supported video file.");
            this->videoIsValid = false;
            return;
        }
        else if ((int)video.get(CV_CAP_PROP_FRAME_COUNT) < 100)
        {
            emit message_to_user("That is not a supported video file.");
            this->videoIsValid = false;
            return;
        }

        this->numVideoFrames = (int)video.get(CV_CAP_PROP_FRAME_COUNT);
        this->framerate = video.get(CV_CAP_PROP_FPS);
        this->durationMs = int(video.get(CV_CAP_PROP_FRAME_COUNT) / (real)video.get(CV_CAP_PROP_FPS) * 1000.0);
        this->resolution.setWidth((int)video.get(CV_CAP_PROP_FRAME_WIDTH));
        this->resolution.setHeight((int)video.get(CV_CAP_PROP_FRAME_HEIGHT));
    }

    this->videoIsValid = true;

    return;
}

video_info_c::~video_info_c()
{
    return;
}

const QString &video_info_c::file_name() const
{
    return this->filename;
}

const QString video_info_c::file_name_sans_path() const
{
    return QFileInfo(this->filename).fileName();
}

real video_info_c::frame_rate() const
{
    return this->framerate;
}

uint video_info_c::duration_ms() const
{
    return this->durationMs;
}

uint video_info_c::width() const
{
    return this->resolution.width();
}

uint video_info_c::height() const
{
    return this->resolution.height();
}

uint video_info_c::num_frames() const
{
    return this->numVideoFrames;
}

bool video_info_c::is_valid_video() const
{
    return videoIsValid;
}
