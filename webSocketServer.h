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
    WebSocketServer(net::io_context& ioc, unsigned short port, MyAccount& account)
        : acceptor_(ioc, tcp::endpoint(tcp::v4(), port)), myAcc_(account) {
        cout << "*** WebSocket Server started on port " << port << " ***" << endl;
        do_accept();
    }

private:
    tcp::acceptor acceptor_;
    MyAccount& myAcc_;

    void do_accept() {
        acceptor_.async_accept(
            [this](beast::error_code ec, tcp::socket socket) {
                if (!ec) {
                    cout << "New WebSocket connection accepted." << endl;
                    std::make_shared<Session>(std::move(socket), myAcc_)->run();
                }
                else {
                    cout << "Error accepting connection: " << ec.message() << endl;
                }
                do_accept();
            });
    }


    class Session : public std::enable_shared_from_this<Session> {
    public:
        Session(tcp::socket socket, MyAccount& account)
            : ws_(std::move(socket)), myAccount_(account), checkSdp_(false) {}

        void run() {
            checkSdp_ = false;
            ws_.async_accept(beast::bind_front_handler(&Session::on_accept, shared_from_this()));
        }

    private:
        beast::websocket::stream<tcp::socket> ws_;
        beast::flat_buffer buffer_;
        MyAccount& myAccount_;
        bool checkSdp_;

        void on_accept(beast::error_code ec) {
            if (ec) {
                cout << "WebSocket handshake error: " << ec.message() << endl;
                return;
            }
            cout << "WebSocket handshake successful." << endl;
            do_read();
        }

        void do_read() {
            ws_.async_read(buffer_,
                beast::bind_front_handler(&Session::on_read, shared_from_this()));
        }

        void on_read(beast::error_code ec, std::size_t bytes_transferred) {
            if (checkSdp_) {
                return;
            }
            checkSdp_ = true;
            if (ec) {
                cout << "Error on read: " << ec.message() << endl;
                return;
            }

            // Convert the buffer to a string
            std::string received_message = beast::buffers_to_string(buffer_.data());
            cout << "Received message from WebSocket: " << received_message << endl;

            // Parse the JSON message
            Json::Value json_message;
            Json::CharReaderBuilder reader;
            std::string errs;

            std::istringstream iss(received_message);
            if (Json::parseFromStream(reader, iss, &json_message, &errs)) {
                // Check if the message is an offer and contains SDP
                if (json_message.isMember("type") && json_message["type"].asString() == "offer" &&
                    json_message.isMember("sdp")) {
                    std::string sdp = json_message["sdp"].asString();
                    cout << "SDP offer received." << endl;
                    cout << "SDP message: " << sdp << "\n end sdp !!" << endl;

                    CallOpParam call_op;
                    call_op.statusCode = PJSIP_SC_OK;
                    std::string sip_uri = "sip:omer2002simhi@sip.linphone.org";
                    MyCall* new_call = new MyCall(myAccount_);

                    try {
                        new_call->makeCall(sip_uri, call_op);
                    }
                    catch (Error& err) {
                        cout << "Error making call: " << err.info() << endl;
                    }

                    cout << "Call initiated with SDP offer sent to SIP endpoint." << endl;

                    // Send SDP OK response to WebRTC client
                    send_sdp_ok();
                }
            }
            else {
                cout << "JSON parse error: " << errs << endl;
            }

            // Write back the received message as an ack
            ws_.async_write(buffer_.data(),
                beast::bind_front_handler(&Session::on_write, shared_from_this()));
        }

        void send_sdp_ok() {
            // Create a valid SDP answer with DTLS fingerprint and ICE candidates
            std::string sdp_answer =
                "v=0\r\n"
                "o=- 0 0 IN IP4 127.0.0.1\r\n"
                "s=SDP Seminar\r\n"
                "c=IN IP4 127.0.0.1\r\n"
                "t=0 0\r\n"
                "a=group:BUNDLE 0\r\n"
                "a=extmap-allow-mixed\r\n"
                "a=msid-semantic: WMS dcf1902f-dd28-4ff5-8b9d-06baf442094e\r\n"
                "m=audio 9 UDP/TLS/RTP/SAVPF 111 63 9 0 8 13 110 126\r\n"
                "c=IN IP4 0.0.0.0\r\n"
                "a=rtcp:9 IN IP4 0.0.0.0\r\n"
                "a=ice-ufrag:jtA7\r\n"
                "a=ice-pwd:uY8HEwHA/R7ty/RiKANWj5VS\r\n"
                "a=ice-options:trickle\r\n"
                "a=fingerprint:sha-256 61:36:8F:6C:70:75:47:BE:B2:79:FE:C0:98:62:83:D8:C4:DD:83:7F:48:DC:87:9A:62:FB:24:1C:40:2F:48:7E\r\n"
                "a=setup:active\r\n"
                "a=mid:0\r\n"
                "a=extmap:1 urn:ietf:params:rtp-hdrext:ssrc-audio-level\r\n"
                "a=extmap:2 http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time\r\n"
                "a=extmap:3 http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01\r\n"
                "a=extmap:4 urn:ietf:params:rtp-hdrext:sdes:mid\r\n"
                "a=sendrecv\r\n"
                "a=msid:dcf1902f-dd28-4ff5-8b9d-06baf442094e 4e587a87-cbde-4276-a664-7b84289c7d3f\r\n"
                "a=rtcp-mux\r\n"
                "a=rtcp-rsize\r\n"
                "a=rtpmap:111 opus/48000/2\r\n"
                "a=rtcp-fb:111 transport-cc\r\n"
                "a=fmtp:111 minptime=10;useinbandfec=1\r\n"
                "a=rtpmap:63 red/48000/2\r\n"
                "a=fmtp:63 111/111\r\n"
                "a=rtpmap:9 G722/8000\r\n"
                "a=rtpmap:0 PCMU/8000\r\n"
                "a=rtpmap:8 PCMA/8000\r\n"
                "a=rtpmap:13 CN/8000\r\n"
                "a=rtpmap:110 telephone-event/48000\r\n"
                "a=rtpmap:126 telephone-event/8000\r\n"
                "a=ssrc:750539669 cname:q9CJy6RcChmdsO7q\r\n"
                "a=ssrc:750539669 msid:dcf1902f-dd28-4ff5-8b9d-06baf442094e 4e587a87-cbde-4276-a664-7b84289c7d3f\r\n";

            Json::Value response;
            response["type"] = "answer";
            response["sdp"] = sdp_answer;

            Json::StreamWriterBuilder writer;
            std::string response_str = Json::writeString(writer, response);

            ws_.async_write(net::buffer(response_str),
                beast::bind_front_handler(&Session::on_write, shared_from_this()));
        }



        void on_write(beast::error_code ec, std::size_t bytes_transferred) {
            buffer_.consume(buffer_.size());
            if (ec) {
                cout << "Error on write: " << ec.message() << endl;
                return;
            }
            cout << "Message sent back to client." << endl;
            do_read();
        }
    };
};

#endif // WEBSOCKET_SERVER_H
