/*
 * Tarpeeksi Hyvae Soft 2018 /
 * AV Scissors.
 *
 * Handles the displaying of messages (and/or notifications) sent by the program's
 * units to the user.
 *
 * At the moment, will take in a message through new_message(), then show it as a
 * popup of sorts on the main window. After a certain number of seconds, the popup
 * will be removed automatically.
 *
 */

#include <QTimer>
#include "../../src/messager/messager.h"

// Re-compute the messages' positions in the parent window. This would be called
// e.g. when one of the messages is deleted.
//
messager_c::messager_c(QWidget * const parentWidget) :
    parent(parentWidget)
{
    return;
}

messager_c::~messager_c()
{
    return;
}

void messager_c::reorder_message_labels(void)
{
    const uint marginX = 70;
    const uint marginY = 14;
    const uint spacingY = 10;

    for (int i = 0; i < this->messagePopups.size(); i++)
    {
        QLabel *const label = this->messagePopups.at(i);

        label->move((this->parent->width() / 2 - label->width()/2),
                    ((this->messagePopups.size() - i - 1) * (label->height() + spacingY)) + marginY);

        label->show();
    }

    return;
}

// Removes all messages currently being displayed in the parent window.
//
void messager_c::remove_all_messages(void)
{
    for (auto *const label: this->messagePopups)
    {
        label->close();
    }
    this->messagePopups.clear();

    return;
}

// Removes the given popup label.
//
void messager_c::remove_message(QLabel *const label)
{
    k_assert((this->messagePopups.indexOf(label) >= 0), "Was asked to remove an unknown message.");

    label->close();
    this->messagePopups.removeAt(this->messagePopups.indexOf(label));

    reorder_message_labels();

    return;
}

// Call this to put up a new message on screen. If there are more messages already
// than there is room for, the oldest message will be removed.
//
void messager_c::new_message(const QString &message)
{
    k_assert((parent != nullptr), "Was asked to add an message, but no parent had been set.");

    QLabel *newMessage = new QLabel(this->parent);

    newMessage->setAttribute(Qt::WA_DeleteOnClose);

    // Create and add the message to the system. This will also display it on screen.
    {
        this->messagePopups.push_back(newMessage);
        if (this->messagePopups.size() > (int)this->maxNumMessages)
        {
            // If we have more message than there is room for, delete the oldest one.
            this->remove_message(this->messagePopups.front());
        }

        this->craft_message_label(newMessage, message);
        this->reorder_message_labels();
    }

    // Set up a timer that will automatically remove the message message (and
    // itself) once it times out.
    {
        QTimer *timer = new QTimer(newMessage);

        connect(timer, &QTimer::timeout,
                 this, [this, newMessage]{ this->remove_message(newMessage); });

        timer->start(this->messageTimeoutMs);
    }

    return;
}

// Style the given popup label so that it contains the given message string and
// looks how we want message popups to look.
//
void messager_c::craft_message_label(QLabel *const label, const QString &message)
{
    label->setStyleSheet("padding: 25px;"
                         "background-color: #A34848;"
                         "color: #E6DCDC;"
                         "border: 1px solid #101010;"
                         "border-bottom: 3px solid #101010;"
                         "border-radius: 0px;");
    label->setText(message);
    label->adjustSize();

    return;
}
