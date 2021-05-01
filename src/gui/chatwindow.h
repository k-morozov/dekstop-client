#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QWidget>
#include "client/client.h"
#include "gui/models/chatmodel.h"
#include "gui/delegate/chatdelegate.h"

namespace Ui {
class ChatWindow;
}

class ChatWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ChatWindow(QWidget *parent = nullptr);
    void set_client(std::shared_ptr<Client> a_client) {
        client_ptr = a_client;
    }

    ~ChatWindow();

signals:
    /**
     * @brief Notify user is sending message
     *
     * @param login user's login
     * @param text message's content
     * @param room_id room where user is subscriber
     */
    void send_text_data(msg_text_t);

    /**
     * @brief Change the room
     *
     * @param new_room_id id of the room user wants to join
     */
    void send_change_room(int new_room_id);

    void go_channels_window();

private slots:
    void on_push_send_clicked();

private slots:
//    void on_push_change_room_id_clicked();

    void on_button_back_clicked();

public slots:
    /**
     * @brief show incoming message
     *
     * @param login sender's login
     * @param message message's content
     * @param dt date and time of sending the text
     */
    void print_text(const msg_text_t& msg);

    void upload_history();

private:
    Ui::ChatWindow *ui;
    std::shared_ptr<Client> client_ptr;
    ChatModel *m_chat_model;

    ChatDelegate *chatDelegate;
};

#endif // CHATWINDOW_H
