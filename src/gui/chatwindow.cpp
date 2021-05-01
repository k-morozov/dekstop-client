#include "chatwindow.h"
#include "ui_chatwindow.h"

ChatWindow::ChatWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatWindow)
{
    ui->setupUi(this);

    QPalette pail;
    pail.setColor(this->backgroundRole(), Qt::white);
    m_chat_model = new ChatModel;
//    pail.setBrush(this->backgroundRole(), QBrush(QPixmap(":/theme/gui/images/chatwindow.jpg")));
    this->setPalette(pail);
//    this->setAutoFillBackground(true);

    ui->text_zone->show();
    ui->text_input->setPlaceholderText("Enter message to send");
    ui->text_zone->setModel(m_chat_model);

    chatDelegate = new ChatDelegate(this);
    ui->text_zone->setItemDelegate(chatDelegate);

//    ui->text_zone->setWindowFlag(Qt::FramelessWindowHint);
//    ui->text_zone->setContextMenuPolicy(Qt::CustomContextMenu);
//    ui->text_zone->setSelectionBehavior(QAbstractItemView::SelectRows);
//    ui->text_zone->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
//     ui->text_zone->setSelectionMode(QAbstractItemView::NoSelection);

//    ui->text_zone->setStyleSheet(
//                "QListView{ outline: 0;}"
//                "QListView::item:hover{   background-color:  rgb(181, 181, 181);selection-color: transperent;}"
//                "QListView::item:selected{ background-color:  rgb(139, 0, 0);selection-color: transperent;}"
//                );
    this->setAutoFillBackground(true);
    this->setWindowTitle("Dialog");
}

ChatWindow::~ChatWindow()
{
    delete ui;
}


void ChatWindow::on_push_send_clicked()
{
    using namespace boost::posix_time;

    auto message = ui->text_input->text();

    msg_text_t a_msg {
        client_ptr->get_login(),
        message.toStdString(),
        client_ptr->get_current_channel(),
        get_current_DateTime()
    };

    qDebug() << "time msg: " << a_msg.dt.time.hours << ":" << a_msg.dt.time.minutes;
    send_text_data(a_msg);
    ui->text_input->clear();
//    ui->text_output->setTextColor(QColor(50,205,50));
//    ui->text_output->append(/*"[" + QString::fromUtf8(to_simple_string(second_clock::local_time().time_of_day()).c_str()) + "] " + */
//                            /*msg.author + ": " + */message);
//    ui->text_output->setTextColor(QColor(0,0,0));
}

void ChatWindow::print_text(const msg_text_t& msg) {
    if (msg.channel_name == client_ptr->get_current_channel()) {
//        std::string s(/*"[" + msg.dt.to_simple_time() + "] " +*/ msg.author + ": " + msg.text);
//        QString message(s.data());
        if (msg.author=="server") {
//            ui->text_output->setTextColor(QColor(255,0,0));
//            ui->text_output->append(message);
//            ui->text_output->setTextColor(QColor(0,0,0));
        }
        else {
//            ui->text_output->append(message);
            m_chat_model->add_msg(msg);
            m_chat_model->update();
        }
    }

}

void ChatWindow::upload_history() {
//    ui->text_output->clear();
    m_chat_model->clear();
    for(const auto& msg : client_ptr->get_history(client_ptr->get_current_channel()) ) {
        print_text(msg);
    }
}

void ChatWindow::on_button_back_clicked()
{
    emit go_channels_window();
}
