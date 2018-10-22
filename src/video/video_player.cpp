/*
 * Tarpeeksi Hyvae Soft 2018 /
 * AV Scissors.
 *
 * Uses QVideoWidget to display videos to the user.
 *
 */

#include <QMediaPlaylist>
#include <QVideoWidget>
#include <QPainter>
#include <QDebug>
#include <QLabel>
#include "../../src/video/video_player.h"
#include "../../src/video/video_object.h"
#include "../../src/video/video_info.h"

video_player_c::video_player_c(QWidget *const parent) :
    parentWidget(parent),
    mediaPlaylist(new QMediaPlaylist(parent)),
    mediaPlayer(new QMediaPlayer(parent)),
    videoWidget(new QVideoWidget(parent))
{
    this->mediaPlaylist->setCurrentIndex(0);
    this->mediaPlaylist->setPlaybackMode(QMediaPlaylist::CurrentItemOnce);

    this->mediaPlayer->setPlaylist(this->mediaPlaylist);
    this->mediaPlayer->setVideoOutput(this->videoWidget);
    this->mediaPlayer->setNotifyInterval(1); // So we get frequent info on the current playback position.
    this->mediaPlayer->pause();

    this->videoWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);   // We'll use a separate kludge function to resize the widget to fit its parent.
    this->videoWidget->show();

    this->noVideoText = new QLabel;
    this->noVideoText->setStyleSheet("padding: 70px;"
                                     "color: #7f7f7f;"
                                     "background-color: transparent;"
                                     "border: 2px dashed #505050;"
                                     "border-radius: 11px;");
    this->noVideoText->setText("No video file loaded.<br>Drag one here to start.");
    this->noVideoText->setAlignment(Qt::AlignHCenter);
    this->noVideoText->adjustSize();
    this->noVideoText->setParent(parent);
    this->noVideoText->setVisible(true);

    // Create the playback icon.
    {
        this->playbackIcon = new QLabel;

        this->playbackIcon->setStyleSheet("background-color: transparent;");
        this->playbackIcon->adjustSize();
        this->playbackIcon->setAttribute(Qt::WA_TransparentForMouseEvents);
        this->playbackIcon->setVisible(false);
        this->playbackIcon->resize(40, 41);

        // Draw the icon graphics.
        {
            iconPlaying = new QPixmap(this->play_icon_size());
            iconPaused = new QPixmap(this->play_icon_size());

            iconPlaying->fill(QColor("transparent"));
            iconPaused->fill(QColor("transparent"));

            QPainter painterPlaying(this->iconPlaying);
            QPainter painterPaused(this->iconPaused);

            painterPlaying.setRenderHint(QPainter::Antialiasing);
            painterPaused.setRenderHint(QPainter::Antialiasing);

            // A solid triangle to indicate 'playing' status.
            {
                QPolygon triangle;
                triangle << QPoint(0, 0);
                triangle << QPoint(0, (this->playbackIcon->height() - 1));
                triangle << QPoint((this->playbackIcon->width() - 2), (this->playbackIcon->height() / 2));

                painterPlaying.setPen(QColor("#fafaff"));
                painterPlaying.setBrush(painterPlaying.pen().color());
                painterPlaying.drawPolygon(triangle);
            }

            // A rectangle to indicate 'paused' status.
            {
                painterPaused.setPen(painterPlaying.pen().color());
                painterPaused.setBrush(painterPaused.pen().color());
                painterPaused.drawRect(0, 0, this->playbackIcon->width(), this->playbackIcon->height());
            }
        }
    }

    this->videoWidget->hide();
    this->videoWidget->show();

    connect(this->mediaPlayer, &QMediaPlayer::positionChanged,
            this, &video_player_c::new_video_pos);

    connect(this->mediaPlayer, &QMediaPlayer::stateChanged,
            this, &video_player_c::new_video_state);

    return;
}

video_player_c::~video_player_c()
{
    this->video = nullptr;

    delete iconPlaying;
    iconPlaying = nullptr;

    delete iconPaused;
    iconPaused = nullptr;

    return;
}

// Set the widget in which the playback icon will be displayed.
//
void video_player_c::set_play_icon_parent(QWidget *const parent)
{
    k_assert((parent != nullptr), "Received a null play icon parent.");

    this->playbackIcon->setParent(parent);

    return;
}

// Set the playback icon's height relative to its parent widget.
//
void video_player_c::set_play_icon_y_offset(const uint yOffs)
{
    this->playbackIcon->move(this->playbackIcon->pos().x(), yOffs);
}

// Resizes/moves the player so that it's positioned in the middle of its parent
// widget and taking up as much of the parent's space as reasonable.
//
void video_player_c::fit_player_to_parent()
{
    const QWidget *const nvParent = this->noVideoText->parentWidget();
    this->noVideoText->move(QPoint((nvParent->width() / 2) - (this->noVideoText->width() / 2),
                                   (nvParent->height() / 2) - (this->noVideoText->height() / 2)));

    if (video == nullptr)
    {
        DEBUG(("No video file set; ignoring call to fit to parent."));
        return;
    }

    // Resize and move the player widget.
    {
        // Match the size to either the parent's width or height, depending on
        // which fits better.
        const real aspectRatio = (this->video_info().width() / (real)this->video_info().height());
        QSize newPlayerSize = QSize((parentWidget->height() * aspectRatio),
                                    parentWidget->height());
        if (newPlayerSize.width() > parentWidget->width())
        {
            const real aspectRatio = (this->video_info().height() / (real)this->video_info().width());
            newPlayerSize = QSize(parentWidget->width(),
                                  (parentWidget->width() * aspectRatio));
        }

        // Have the player be slightly smaller than its parent.
        newPlayerSize.setWidth(newPlayerSize.width() * 0.99);
        newPlayerSize.setHeight(newPlayerSize.height() * 0.99);

        this->videoWidget->move(((parentWidget->width() / 2) - (newPlayerSize.width() / 2)),
                                ((parentWidget->height() / 2) - (newPlayerSize.height() / 2)));

        this->videoWidget->resize(newPlayerSize);
    }

    // Make sure the playback icon's position reflects the new size.
    {
        const uint pixelPosX = (((this->playbackIcon->parentWidget())->width() / (real)this->video_info().duration_ms()) * curPlaybackPosMs);
        this->playbackIcon->move((pixelPosX - this->playbackIcon->width()), this->playbackIcon->y());
    }

    this->videoWidget->show();
    this->videoWidget->raise();

    return;
}

// Make the given video the one this player plays. Note that although this makes
// reference to QMediaPlaylist, no more than one video can be in that list at any
// given time, at the moment.
//
void video_player_c::add_video_file(const video_object_c *const _video)
{
    k_assert((_video != nullptr), "Was passed a null video object.");

    this->video = _video;

    // Add the video to the playlist.
    k_assert((this->mediaPlaylist != nullptr), "Tried to assign to a null playlist.");
    k_assert((this->mediaPlaylist->mediaCount() <= 1), "Can only have one video on the playlist.");

    // Make sure there are no duplicates of this video.
    for (int i = 0; i < this->mediaPlaylist->mediaCount(); i++)
    {
        if (this->mediaPlaylist->media(i).canonicalUrl() == QUrl::fromLocalFile(video->info().file_name()))
        {
            INFO(("Skipping duplicate video file."));
            goto done;
        }
    }

    this->mediaPlayer->stop();
    this->mediaPlaylist->removeMedia(0);
    this->mediaPlaylist->addMedia(QUrl::fromLocalFile(this->video_info().file_name()));
    this->mediaPlayer->pause();

    done:

    // If we now have a video to play, unhide player functionality.
    if (this->mediaPlaylist->mediaCount() > 0)
    {
        this->noVideoText->setVisible(false);
        this->videoWidget->setVisible(true);
    }

    return;
}

// Eject the video that was previously assigned to this player.
//
void video_player_c::remove_video_file(const video_object_c *const video)
{
    k_assert((video != nullptr), "Was passed a null video object.");
    k_assert((this->video != nullptr), "Was asked to remove video file, but there was none to begin with.");

    // Find the first instance of this video on the playlist, and remove it.
    for (int i = 0; i < this->mediaPlaylist->mediaCount(); i++)
    {
        if (this->mediaPlaylist->media(i).canonicalUrl() == QUrl::fromLocalFile(video->info().file_name()))
        {
            this->mediaPlayer->stop();
            this->mediaPlaylist->removeMedia(i);
            this->mediaPlayer->pause();

            this->video = nullptr;

            goto done;
        }
    }
    k_assert(0, "Failed to find the video to remove.");

    done:

    // If we have no more videos to play, hide player functionality.
    if (this->mediaPlaylist->mediaCount() == 0)
    {
        this->playbackIcon->setVisible(false);
        this->noVideoText->setVisible(true);
        this->videoWidget->setVisible(false);
    }

    return;
}

void video_player_c::play() const
{
    this->mediaPlayer->play();

    return;
}

void video_player_c::pause() const
{
    this->mediaPlayer->pause();

    return;
}

void video_player_c::seek_to_ms(qint64 newPosMs)
{
    if (this->video == nullptr)
    {
        INFO(("Video player has no video; ignoring call to seek."));
        return;
    }

    if (newPosMs < 0)
    {
        newPosMs = 0;
    }
    // Leave a buffer at the end when seeking manually, so that the video doesn't
    // immediately jump back to the beginning when seeking to the end.
    else if (newPosMs >= (this->video_info().duration_ms() - 1500))
    {
        newPosMs = (this->video_info().duration_ms() - 1500);
    }

    this->mediaPlayer->setPosition(newPosMs);

    return;
}

// Seek to the given x coordinate relative to the player's parent widget.
//
void video_player_c::seek_to_x(const int x)
{
    if (this->video == nullptr)
    {
        INFO(("Video player has no video; ignoring call to seek."));
        return;
    }

    const qint64 posMs = ((this->video_info().duration_ms() / (real)this->parentWidget->width()) * x);
    this->seek_to_ms(posMs);

    return;
}

// Seek to (about) the nth video frame. Qt seeks in milliseconds, so that's why
// this isn't guaranteed to be exact.
//
void video_player_c::seek_to_frame(const int frameIdx)
{
    if (this->video == nullptr)
    {
        INFO(("Video player has no video; ignoring call to seek."));
        return;
    }

    const qint64 posMs = ((frameIdx / (real)this->video_info().frame_rate()) * 1000.0);
    this->seek_to_ms(posMs);

    return;
}

void video_player_c::mute()
{
    this->mediaPlayer->setMuted(true);
}

void video_player_c::unmute()
{
    this->mediaPlayer->setMuted(false);
}

void video_player_c::raise()
{
    this->videoWidget->raise();
    this->playbackIcon->raise();
}

bool video_player_c::is_muted()
{
    return this->mediaPlayer->isMuted();
}

bool video_player_c::is_playing()
{
    return bool(mediaPlayer->state() == QMediaPlayer::PlayingState);
}

bool video_player_c::has_video()
{
    return bool(this->video != nullptr);
}

QSize video_player_c::play_icon_size() const
{
    return this->playbackIcon->size();
}

const video_info_c& video_player_c::video_info()
{
    k_assert((video != nullptr), "Was asked for the player's video file, but the file is null.");
    return video->info();
}

const video_activity_c& video_player_c::video_activity()
{
    k_assert((video != nullptr), "Was asked for the player's video file, but the file is null.");
    return video->activity();
}

uint video_player_c::playback_pos_ms() const
{
    return curPlaybackPosMs;
}

// Gets signaled by Qt's media player when the video playback position changes.
//
void video_player_c::new_video_pos(const qint64 newPosMs)
{
    // Position the play icon to reflect the new position, with the assumption
    // that the total width of the icon's parent represents the entire length
    // of the video.
    if (this->playbackIcon->parent() != nullptr)
    {
        uint pixelPosX = (((this->playbackIcon->parentWidget())->width() / (real)this->video_info().duration_ms()) * newPosMs);
        this->playbackIcon->move((pixelPosX - this->playbackIcon->width()), this->playbackIcon->y());

        if (!this->playbackIcon->isVisible())
        {
            this->playbackIcon->setVisible(true);
        }
    }

    curPlaybackPosMs = newPosMs;

    emit video_pos_changed(newPosMs);
}

// Gets signaled by Qt's media player when the video's state changes.
//
void video_player_c::new_video_state(const QMediaPlayer::State state)
{
    switch (state)
    {
        case QMediaPlayer::PausedState:
        {
            k_assert(this->playbackIcon != nullptr, "Null playback icon container.");
            k_assert(this->iconPaused != nullptr, "Null playback graphic.");

            this->playbackIcon->setPixmap(*this->iconPaused);

            break;
        }
        case QMediaPlayer::PlayingState:
        {
            k_assert(this->playbackIcon != nullptr, "Null playback icon container.");
            k_assert(this->iconPlaying != nullptr, "Null playback graphic.");

            this->playbackIcon->setPixmap(*this->iconPlaying);

            break;
        }
        case QMediaPlayer::StoppedState:
        {
            this->play();   // Assume stopped because the video ended; so loop back to the beginning and start over.

            break;
        }
        default:
        {
            k_assert(0, "Unknown video state.");

            break;
        }
    }

    return;
}
