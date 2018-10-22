/*
 * Tarpeeksi Hyvae Soft 2018 /
 * AV Scissors.
 *
 * Collects together various pieces of information about a given video file.
 *
 */

#include <QMessageBox>
#include "../../src/video/video_object.h"
#include "../../src/video/video_player.h"
#include "../../src/messager/messager.h"

video_object_c::video_object_c(const QString filename, const messager_c *const messager) :
    videoFilename(filename),
    videoInfo(videoFilename, messager),
    videoActivity(videoInfo, messager)
{
    connect(    this, &video_object_c::message_to_user,
            messager, &messager_c::new_message);

    if (!this->info().is_valid_video())
    {
        DEBUG(("the given video file '%s' could not be loaded.", filename.toLatin1().constData()));
    }

    return;
}

video_object_c::~video_object_c()
{
    if (associatedPlayer != nullptr)
    {
        associatedPlayer->remove_video_file(this);
        associatedPlayer = nullptr;
    }

    return;
}

// Video objects can be assigned to one video player. Once assigned, that's what
// the player will play.
//
void video_object_c::assign_to_player(video_player_c *const player)
{
    k_assert((associatedPlayer == nullptr), "Trying to re-assign the video object to a player.");

    player->add_video_file(this);
    associatedPlayer = player;

    return;
}

const video_info_c& video_object_c::info() const
{
    return videoInfo;
}

const video_activity_c& video_object_c::activity() const
{
    return videoActivity;
}

const QVector<video_activity_c::activity_type_e>& video_object_c::video_activity_data() const
{
    return this->videoActivity.videoFrameIsActive;
}

const QVector<video_activity_c::activity_type_e>& video_object_c::audio_activity_data() const
{
    return this->videoActivity.audioFrameIsActive;
}
