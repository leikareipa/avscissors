/*
 * Tarpeeksi Hyvae Soft 2018 /
 * AV Scissors.
 *
 */

#ifndef VIDEO_PLAYER_H_
#define VIDEO_PLAYER_H_

#include <QMediaPlayer>
#include "../../src/common.h"

class QMediaPlaylist;
class QVideoWidget;
class QLabel;
class video_activity_c;
class video_object_c;
class video_info_c;

class video_player_c : public QObject
{
    Q_OBJECT

public:
    video_player_c(QWidget *const parent);
    ~video_player_c();

    void set_play_icon_parent(QWidget *const parent);

    void set_play_icon_y_offset(const uint yOffs);

    void fit_player_to_parent(void);

    void add_video_file(const video_object_c *const _video);

    void remove_video_file(const video_object_c *const video);

    void play(void) const;

    void pause(void) const;

    void seek_to_ms(qint64 newPosMs);

    void seek_to_x(const int x);

    void seek_to_frame(const int frameIdx);

    void mute(void);

    void unmute(void);

    void raise(void);

    bool is_muted(void);

    bool is_playing(void);

    bool has_video(void);

    QSize play_icon_size(void) const;

    const video_info_c& video_info(void);

    const video_activity_c& video_activity(void);

    uint playback_pos_ms(void) const;

signals:
    void video_pos_changed(const qint64 newPosMs);

    void message_to_user(const QString message);

private slots:
    void new_video_pos(const qint64 newPosMs);

    void new_video_state(const QMediaPlayer::State state);

private:
    const QWidget *const parentWidget;
    QMediaPlaylist *const mediaPlaylist;
    QMediaPlayer *const mediaPlayer;
    QVideoWidget *const videoWidget;

    qint64 curPlaybackPosMs = 0;

    // The icon shown in the GUI to indicate the current playback position.
    QLabel *playbackIcon = nullptr;

    // Text shown to the user when no video file is loaded in.
    QLabel *noVideoText = nullptr;

    // The imagese used on the playback icon, depending on the status of the playback.
    QPixmap *iconPlaying = nullptr;
    QPixmap *iconPaused = nullptr;

    // The video this player will play.
    const video_object_c *video = nullptr;
};

#endif
