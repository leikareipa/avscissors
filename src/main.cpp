/*
 * Tarpeeksi Hyvae Soft 2018 /
 * AV Scissors.
 *
 * AV Scissors is a program to quickly find the active portions (audio/video)
 * in videos.
 *
 * Useful if you've, say, recorded a video of your pet alone at home, and on
 * reviewing that video want to expedite the skipping over of irrelevant bits
 * where nothing happens.
 *
 */

#include <QApplication>
#include <QFileDialog>
#include "../../src/common.h"
#include "qt_main_window.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    return a.exec();
}
