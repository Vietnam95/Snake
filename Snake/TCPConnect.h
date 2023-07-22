#pragma once
#include "stdafx.h"

using namespace boost;

// Class base TCP connect
class TCPConnect
{
public:
    // Contructor
    TCPConnect();

    // Destructor
    virtual ~TCPConnect() = default;

    // Init TCP
    virtual void init() = 0;

    // Start thread ios
    void startThread();

    // Stop connection
    virtual void stop() = 0;

    // Send message
    virtual bool requestWrite(const std::vector<char>& message) = 0;

    // Set Callback function
    bool setMessageReceivedHandler(std::function<bool(const std::string&)> func);

    // Check if Client with Server is pairing
    virtual bool isPairing() = 0;

protected:
    // Run ios service
    void runService();

    asio::io_service    m_ios;          // ios service
    asio::streambuf     m_buffer;       // message buffer
    boost::thread       m_thrd;         // thread
    bool                m_blPairing;    // is pairing or not
    std::vector<char>   m_messageHeader;

    std::function<bool(const std::string&)> m_func; // Callback funtion
};

// Class TCP Server
class TCPServer : public TCPConnect
{
public:
    // Contructor
    TCPServer();

    // Destructor
    virtual ~TCPServer();

    // Init Server
    virtual void init() override;

    // Stop Server
    virtual void stop() override;

    // Start Server
    bool startServer();

    // Send message
    virtual bool requestWrite(const std::vector<char>& message) override;

    // Accept socket
    void onAccept(const system::error_code& ec,
        std::shared_ptr<asio::ip::tcp::socket> sock);

    virtual bool isPairing() override;
private:
    // Receive message
    void onMessageReceived(const boost::system::error_code& ec
        , const boost::asio::ip::tcp::socket::native_handle_type& sessionID
        , std::size_t bytes_transferred);

    // Start another socket
    void startAccept();

    // Add socket to list
    bool handleAccept(const system::error_code& ec,
        std::shared_ptr<asio::ip::tcp::socket> sock);

    bool handleReadExactly(const std::vector<char>& messageHeader, const boost::asio::ip::tcp::socket::native_handle_type unSessionID);

    bool handleWriteExactly(const std::vector<char>& message, const boost::asio::ip::tcp::socket::native_handle_type unSessionID);

    asio::ip::tcp::acceptor m_acceptor; // Boost acceptor
    std::map<boost::asio::ip::tcp::socket::native_handle_type, std::shared_ptr<asio::ip::tcp::socket>> m_lstSocket; // Socket list
    std::map<boost::asio::ip::tcp::socket::native_handle_type, bool> m_lstIsPairing;    // List pairing status with each socket
    std::mutex m_Mutex; // Mutex
};

// Class TCP Client
class TCPClient : public TCPConnect
{
public:
    // Contructor
    TCPClient();

    // Destructor
    virtual ~TCPClient();

    // Init Client
    virtual void init() override;

    // Stop Client
    virtual void stop() override;

    // Request connect to Server
    bool requestConnect(const std::string& strIP, const int& nPort);

    // Send message
    virtual bool requestWrite(const std::vector<char>& message) override;

    // Check if Client is connect
    bool isConnect();

    virtual bool isPairing() override;
private:
    // When connect complete, start wait read message
    bool onConnectCompleted(const system::error_code& ec);

    // Recevie message
    void onMessageReceived(const boost::system::error_code& ec, std::size_t bytes_transferred);

    bool handleReadExactly(const std::vector<char>& messageHeader);
    bool handleWriteExactly(const std::vector<char> message);

    // Close Client socket
    void closeSocket();

    std::shared_ptr<asio::ip::tcp::socket> m_pSocket;   // Socket
};