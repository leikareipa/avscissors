/*
 * Tarpeeksi Hyvae Soft 2018 /
 * AV Scissors.
 *
 */

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>

class QLabel;
class QTimer;
class video_player_c;
class video_info_c;
class video_object_c;
class messager_c;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void fit_video_player_to_canvas();

signals:
    void message_to_user(const QString message);

private slots:
    void toggle_playback(void);

    void update_activity_strips();

    void playback_seek_ms(const int ms);

private:
    bool eventFilter(QObject *object, QEvent *event);
    void resizeEvent(QResizeEvent *);
    void closeEvent(QCloseEvent *);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

    Ui::MainWindow *ui = nullptr;

    QPixmap create_mouseover_pixmap(const QSize size, const uint videoOrAudio, const int cursorXOffset, const int cursorYOffset);

    void update_window_title();

    void insert_video(const QString filename);

    video_object_c *video = nullptr;

    video_player_c *videoPlayer = nullptr;

    // For showing messages/notifications to the user from the program.
    messager_c *messager = nullptr;

    // Shown in the GUI for when the mouse hovers over video playback controls.
    QLabel *mouseOverIndicator = nullptr;

    // Used to keep periodically checking on the threaded progress of video
    // analysis, and to update the GUI on its progress.
    QTimer *stripUpdateTimer = nullptr;
};

#endif // MAIN_WINDOW_H
