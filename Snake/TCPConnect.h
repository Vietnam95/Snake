#pragma once
#include "stdafx.h"

using namespace boost;

class TCPConnect
{
public:
    TCPConnect();
    virtual ~TCPConnect() = default;

    virtual void init() = 0;

    void startThread();

    virtual void stop() = 0;

    void onMessageReceived(const boost::system::error_code & ec, std::size_t bytes_transferred);

    void onAccept(const system::error_code & ec,
        std::shared_ptr<asio::ip::tcp::socket> sock);

    bool requestWrite(const std::string& strReq);

    bool setMessageReceivedHandler(std::function<bool(const std::string&)> func);
    void closeSocket();
    bool isPairing();

protected:

    void runService();

    asio::io_service m_ios;
    std::shared_ptr<asio::ip::tcp::socket> m_pSocket;
    asio::streambuf m_buffer;
    boost::thread m_thrd;
    bool m_blPairing;

    std::function<bool(const std::string&)> m_func;
};

class TCPServer : public TCPConnect
{
public:
    TCPServer();
    virtual ~TCPServer();
    virtual void init() override;
    virtual void stop() override;
    bool startServer();
private:
    asio::ip::tcp::acceptor m_acceptor;
};

class TCPClient : public TCPConnect
{
public:
    TCPClient();
    virtual ~TCPClient();
    virtual void init() override;
    virtual void stop() override;

    bool requestConnect(const std::string& strIP, const int& nPort);

    bool isConnect();
private:
    bool onConnectCompleted(const system::error_code& ec);
};