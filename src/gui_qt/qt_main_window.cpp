/*
 * Tarpeeksi Hyvae Soft 2018 /
 * AV Scissors.
 *
 */

#include <QMediaPlaylist>
#include <QDesktopWidget>
#include <QVideoWidget>
#include <QMediaPlayer>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QFileInfo>
#include <QShortcut>
#include <QPainter>
#include <QPixmap>
#include <QDebug>
#include <QTimer>
#include <QLabel>
#include "../../src/gui_qt/qt_main_window.h"
#include "../../src/video/video_activity.h"
#include "../../src/video/video_object.h"
#include "../../src/video/video_player.h"
#include "../../src/messager/messager.h"
#include "../../src/video/video_info.h"
#include "../../src/common.h"
#include "ui_qt_main_window.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowTitle(PROGRAM_TITLE);

    messager = new messager_c(ui->centralWidget);

    // Style the main window.
    {
        ui->widget_videoCanvas->setStyleSheet("background-color: #424242;");
    }

    // Style and initialize the playback controls area, including activity strips.
    {
        ui->widget_activityStrips->setStyleSheet("background-color: #4f4f4f;");

        ui->activityStrip_videoActivity->set_colors(QColor("#98CC85"), QColor("#737371"), QColor("#4f4f4f"));
        ui->activityStrip_audioActivity->set_colors(QColor("mediumseagreen"), QColor("#656565"), QColor("#4f4f4f"));

        this->mouseOverIndicator = new QLabel(this);
        this->mouseOverIndicator->setStyleSheet("background-color: transparent;");
        this->mouseOverIndicator->setParent(ui->widget_activityStrips);
        this->mouseOverIndicator->setAttribute(Qt::WA_TransparentForMouseEvents);
        this->mouseOverIndicator->setVisible(false);
    }

    // Create the video player we'll use to display videos to the user.
    {
        this->videoPlayer = new video_player_c(ui->widget_videoCanvas);

        this->videoPlayer->set_play_icon_parent(ui->widget_activityStrips);
        this->videoPlayer->set_play_icon_y_offset((ui->activityStrip_videoActivity->height() - (videoPlayer->play_icon_size().height() / 2)));

        connect(this->videoPlayer, &video_player_c::message_to_user,
                         messager, &messager_c::new_message);
    }

    // Give the UI elements their proper depth order.
    {
        ui->activityStrip_videoActivity->raise();
        this->mouseOverIndicator->raise();
        ui->widget_videoCanvas->raise();
        this->videoPlayer->raise();
    }

    // Assign event filters.
    {
        this->setMouseTracking(true);
        this->installEventFilter(this);

        ui->widget_videoCanvas->setMouseTracking(true);
        ui->widget_videoCanvas->installEventFilter(this);

        ui->widget_activityStrips->setMouseTracking(true);
        ui->widget_activityStrips->installEventFilter(this);
    }

    // Position the window in about the middle of the screen.
    {
        QRect desktopRect = QApplication::desktop()->availableGeometry(this);
        QPoint center = desktopRect.center();

        this->move((center.x() - (width() * 0.5)),
                   (center.y() - (height() * 0.6)));
    }

    this->stripUpdateTimer = new QTimer(this);
    connect(this->stripUpdateTimer, &QTimer::timeout,
                              this, &MainWindow::update_activity_strips);

    // Assign keyboard shortcuts.
    {
        QShortcut *keybShortcutPlay = new QShortcut(QKeySequence(Qt::Key_Space), this);
        keybShortcutPlay->setContext(Qt::ApplicationShortcut);
        connect(keybShortcutPlay, &QShortcut::activated,
                            this, &MainWindow::toggle_playback);

        QShortcut *keybShortcutSeekLeft = new QShortcut(QKeySequence(Qt::Key_Left), this);
        keybShortcutSeekLeft->setContext(Qt::ApplicationShortcut);
        connect(keybShortcutSeekLeft, &QShortcut::activated,
                                this, [this]{ this->playback_seek_ms(-2000); });

        QShortcut *keybShortcutSeekRight = new QShortcut(QKeySequence(Qt::Key_Right), this);
        keybShortcutSeekRight->setContext(Qt::ApplicationShortcut);
        connect(keybShortcutSeekRight, &QShortcut::activated,
                                 this, [this]{ this->playback_seek_ms(2000); });
    }

    // Have these calls last. They attempt to ensure that widgets are in their
    // proper initial places.
    this->show();
    this->videoPlayer->fit_player_to_parent();

    return;
}

MainWindow::~MainWindow()
{
    delete ui;
    delete messager;

    return;
}

// Draw an image that will be shown over the video controls on mouse hover.
//
/// TODO. Gets the job done but is a bit naff.
QPixmap MainWindow::create_mouseover_pixmap(const QSize size, const uint videoOrAudio, const int cursorXOffset, const int cursorYOffset)
{
    QPixmap pixmap(size);
    pixmap.fill(QColor("transparent"));

    // Draw nothing if we don't have a valid video.
    if (!this->videoPlayer->has_video())
    {
        return pixmap;
    }

    // Draw the mouse image. At the moment, it's just a dotted line from the cursor
    // to the middle of the controls.
    {
        QPainter painter(&pixmap);

        const uint lineThickness = 1;
        const QPoint midpoint = QPoint((size.width() / 2), (size.height() / 2));

        painter.setPen(QPen(QColor("#eeeeee"), lineThickness, Qt::DotLine, Qt::SquareCap, Qt::BevelJoin));
        painter.drawLine(QPoint(cursorXOffset, (midpoint.y() + cursorYOffset)),
                         QPoint(cursorXOffset, midpoint.y()));
    }

    return pixmap;
}

// Gets called by a timer to keep re-building the activity strips as more data
// is processed, giving the user real-time updates on the progress.
//
void MainWindow::update_activity_strips(void)
{
    ui->activityStrip_videoActivity->regenerate_activity_strip();
    ui->activityStrip_audioActivity->regenerate_activity_strip();

    if (videoPlayer->video_activity().strip_build_has_finished())
    {
        stripUpdateTimer->stop();
    }

    return;
}

void MainWindow::closeEvent(QCloseEvent *)
{
    // Don't want to be trying to update activity strips, since we're about to
    // delete the video to which their pointer(s) may point.
    stripUpdateTimer->stop();

    delete video;
    delete videoPlayer;

    return;
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    videoPlayer->fit_player_to_parent();
    messager->reorder_message_labels();

    return;
}

/// TODO. Cleanup, cleanup.
bool MainWindow::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::MouseMove)
    {
        // Ignore mouse interaction that doesn't happen over the activity strips.
        if ((object->objectName() != ui->widget_activityStrips->objectName()))
        {
            mouseOverIndicator->setVisible(false);
            return false;
        }

        const QMouseEvent *const e = (QMouseEvent*)event;

        // Assume that there are two activity strips, and that they're both
        // the same height, so that we can decide which strip the mouse is
        // over just by looking at whether its y coordinate is above or
        // below the area's midpoint.
        k_assert((ui->activityStrip_videoActivity->height() == ui->activityStrip_audioActivity->height()),
                 "Expected the activity strips to have equal heights.");
        const uint videoOrAudio = (e->pos().y() < (ui->widget_activityStrips->height() / 2))? 0
                                                                                            : 1;

        // Have the mouse-over indicator track the mouse.
        mouseOverIndicator->resize(ui->widget_activityStrips->size());
        const int yOffs = (e->pos().y() - (ui->widget_activityStrips->height() / 2));
        const int xOffs = e->pos().x();
        mouseOverIndicator->move(0,0);

        mouseOverIndicator->setVisible(true);
        mouseOverIndicator->setPixmap(create_mouseover_pixmap(mouseOverIndicator->size(), videoOrAudio, xOffs, yOffs));

        // If the user is holding a mouse button down while moving the mouse,
        // and not pressing any control keys, scroll the current media position.
        if (QApplication::mouseButtons() & Qt::LeftButton)
        {
            videoPlayer->mute();    /// Temp hack. Prevent sound artefacting while seeking.
            videoPlayer->seek_to_x(e->pos().x());
        }

        return true;
    }
    else if (event->type() == QEvent::MouseButtonPress)
    {
        const QMouseEvent *const e = (QMouseEvent*)event;

        // Ignore mouse interaction that doesn't happen over the activity strips.
        if ((object->objectName() != ui->widget_activityStrips->objectName()))
        {
            return false;
        }

        // Right-clicking lets you snap to active segments.
        if ((QApplication::mouseButtons() & Qt::RightButton) &&
            (this->video != nullptr))
        {
            // Assume that there are two activity strips, and that they're both
            // the same height, so that we can decide which strip the mouse is
            // over just by looking at whether its y coordinate is above or
            // below the area's midpoint.
            k_assert((ui->activityStrip_videoActivity->height() == ui->activityStrip_audioActivity->height()),
                     "Expected the activity strips to have equal heights.");
            const uint videoOrAudio = (e->pos().y() < (ui->widget_activityStrips->height() / 2))? 0
                                                                                                : 1;
            const ActivityStrip *const strip = (videoOrAudio == 0)? ui->activityStrip_videoActivity
                                                                  : ui->activityStrip_videoActivity;

            // Find the first frame of the active segment over which the cursor
            // is currently. If it's not over an active segment, we do nothing.
            uint frameIdx = ((videoPlayer->video_info().num_frames() / (real)strip->width()) * e->pos().x());
            if (videoPlayer->video_activity().is_active_frame_at(frameIdx, videoOrAudio))
            {
                frameIdx = videoPlayer->video_activity().get_start_of_active_segment(frameIdx, videoOrAudio);
                frameIdx -= (videoPlayer->video_info().frame_rate() * 0.5); // Seek back by 500 milliseconds, for user convenience.

                videoPlayer->seek_to_frame(frameIdx);
            }
            else
            {
                return false;
            }
        }
        // The left mouse button seeks the playback position.
        else if (QApplication::mouseButtons() & Qt::LeftButton)
        {
            videoPlayer->seek_to_x(e->pos().x());
        }

        return true;
    }
    else if (event->type() == QEvent::MouseButtonRelease)
    {
        const QMouseEvent *const e = (QMouseEvent*)event;

        /// Temp hack. Prevent sound artefacting while seeking.
        if (videoPlayer->is_muted())
        {
            videoPlayer->unmute();
        }

        return true;
    }
    else if (event->type() == QEvent::Leave)
    {
        // Make sure the mouse-over indicator is hidden when the mouse is outside of the screen.
        mouseOverIndicator->setVisible(false);
    }
    else if (event->type() == QEvent::Enter)
    {
        // Make sure the mouse-over indicator is hidden when the mouse is outside of the screen.
        mouseOverIndicator->setVisible(false);
    }

    return false;
}

// Seek back/forward by the given number of milliseconds.
//
void MainWindow::playback_seek_ms(const int ms)
{
    videoPlayer->seek_to_ms(videoPlayer->playback_pos_ms() + ms);

    return;
}

void MainWindow::toggle_playback(void)
{
    if (videoPlayer->is_playing())
    {
        videoPlayer->pause();
    }
    else
    {
        videoPlayer->play();
    }

    return;
}

// Attempts to change the video we're currently operating on.
//
void MainWindow::insert_video(const QString filename)
{
    // Don't want this timer firing while we've potentially got a null video.
    stripUpdateTimer->stop();

    if (this->video != nullptr)
    {
        delete this->video;
    }

    // Attempt to insert the new video.
    {
        this->video = new video_object_c(filename, messager);
        video->assign_to_player(videoPlayer);

        // If the insertion failed, remove the video from the system.
        if (!this->video->info().is_valid_video())
        {
            delete this->video;
            this->video = nullptr;

            ui->activityStrip_videoActivity->set_strip_data_ptr(nullptr);
            ui->activityStrip_audioActivity->set_strip_data_ptr(nullptr);

            goto done;
        }
    }

    ui->activityStrip_videoActivity->set_strip_data_ptr(&video->video_activity_data());
    ui->activityStrip_audioActivity->set_strip_data_ptr(&video->audio_activity_data());

    // Check in on the progress of the strip-building every now and then, and update
    // the GUI with its current state.
    this->stripUpdateTimer->start(700);

    this->videoPlayer->play();

    done:
    this->update_window_title();
    this->videoPlayer->fit_player_to_parent();
    return;
}

void MainWindow::update_window_title(void)
{
    QString title = PROGRAM_TITLE;

    if (this->videoPlayer->has_video())
    {
        title = QString("%1 - %2").arg(this->videoPlayer->video_info().file_name_sans_path())
                                  .arg(PROGRAM_TITLE);
    }

    this->setWindowTitle(title);

    return;
}

// Gets called when the mouse cursor, dragging an object, is first brought into
// this widget's space.
//
void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
}

// Gets called when something is dropped onto this widget's screen area. Note that
// the drag must first be accepted by dragEnterEvent() before its drop can be
// registered in this function.
//
void MainWindow::dropEvent(QDropEvent *event)
{
    k_assert(!event->mimeData()->urls().isEmpty(), "Expected to receive a file as a drop.");

    if (event->mimeData()->urls().size() > 1)
    {
        INFO(("Received multiple files as a drop. Ignoring all but the first one."));
    }

    /// FIXME. Pretty scummy string conversion. Getting proper file name strings
    ///        from drop URLs seems to not be trivial.
    QString filename = QUrl::fromPercentEncoding(event->mimeData()->urls().at(0).toString().toUtf8());
    this->insert_video(filename.remove("file://"));

    return;
}
