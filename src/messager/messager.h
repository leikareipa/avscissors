/*
 * Tarpeeksi Hyvae Soft 2018 /
 * AV Scissors
 *
 */

#ifndef MESSAGER_H
#define MESSAGER_H

#include <QObject>
#include <QLabel>
#include <QDebug>
#include <QList>
#include "../../src/common.h"

class messager_c : public QObject
{
    Q_OBJECT

public:
    messager_c(QWidget *const parentWidget);
    ~messager_c(void);

    void reorder_message_labels(void);

    void remove_all_messages();

public slots:
    void new_message(const QString &message);

private slots:
    void remove_message(QLabel *const label);

private:
    void craft_message_label(QLabel *const label, const QString &message);

    // The widget where we'll display the messages.
    QWidget *const parent;

    // The visual realization of each message, i.e. a popup on screen.
    QVector<QLabel*> messagePopups;

    // For how long message messages stay visible.
    const uint messageTimeoutMs = 5000;

    // The maximum number of message we'll show at a time. Older ones get destroyed.
    const uint maxNumMessages = 5;
};

#endif
