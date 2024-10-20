#ifndef WEBSOCKET_SERVER_H
#define WEBSOCKET_SERVER_H

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include "MyAccount.h"
#include <json/json.h>

namespace beast = boost::beast;
namespace net = boost::asio;
using tcp = net::ip::tcp;
using namespace std;

class WebSocketServer {
public:
    WebSocketServer(net::io_context& ioc, unsigned short port, MyAccount& account);

private:
    tcp::acceptor acceptor_;
    MyAccount& myAcc_;

    void do_accept();

    class Session : public std::enable_shared_from_this<Session> {
    public:
        Session(tcp::socket socket, MyAccount& account);
        void run();

    private:
        beast::websocket::stream<tcp::socket> ws_;
        beast::flat_buffer buffer_;
        MyAccount& myAccount_;

        void on_accept(beast::error_code ec);
        void do_read();
        void on_read(beast::error_code ec, std::size_t bytes_transferred);
        void send_sdp_ok();
        void on_write(beast::error_code ec, std::size_t bytes_transferred);
    };
};

#endif // WEBSOCKET_SERVER_H
