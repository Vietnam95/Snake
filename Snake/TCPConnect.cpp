#pragma once
#include "stdafx.h"

#include "TCPConnect.h"

using namespace boost;

/**************************************** TCPConnect ******************************************/
// Contructor
TCPConnect::TCPConnect()
{
}

// start thread for ios run
void TCPConnect::startThread()
{
    m_thrd = boost::thread(&TCPConnect::runService, this);
    m_thrd.detach();
}

// set call back func to handle message
bool TCPConnect::setMessageReceivedHandler(std::function<bool(const std::string&)> func)
{
    // Only set call back if func valid
    if (!func)
    {
        return false;
    }

    m_func = func;

    return true;
}

// run service
void TCPConnect::runService()
{
    m_ios.run();
}

/**************************************** TCPServer ******************************************/
// Contructor
TCPServer::TCPServer() : TCPConnect()
, m_acceptor(m_ios, asio::ip::tcp::endpoint(asio::ip::address_v4::any(), 9003))
, m_lstSocket()
, m_lstIsPairing()
{
}

// Destructor
TCPServer::~TCPServer()
{
    stop();
}

// Init boost acceptor
void TCPServer::init()
{
    // Acceptor listen
    m_acceptor.listen();
    // Start async accept
    startAccept();
}

// Start async accept
void TCPServer::startAccept()
{
    std::shared_ptr<asio::ip::tcp::socket> pSocket = std::make_shared<asio::ip::tcp::socket>(m_ios);
    m_acceptor.async_accept(*pSocket.get(), std::bind(&TCPServer::onAccept, this, std::placeholders::_1, pSocket));
}

// Create new thread each socket
void TCPServer::onAccept(const system::error_code& ec,
    std::shared_ptr<asio::ip::tcp::socket> pSocket)
{
    m_thrd = boost::thread(&TCPServer::handleAccept, this, ec, pSocket);
    m_thrd.detach();

    // Start another async accept
    startAccept();
}

// Add socket to list 
bool TCPServer::handleAccept(const system::error_code& ec,
    std::shared_ptr<asio::ip::tcp::socket> pSocket)
{
    if (ec.value() == 0)
    {
        // Get session id of socket
        boost::asio::ip::tcp::socket::native_handle_type unSessionID = pSocket->native_handle();

        // If socket is not connect
        if (m_lstSocket.count(unSessionID) == 0)
        {
            m_lstSocket[unSessionID] = pSocket;
        }

        m_lstIsPairing[unSessionID] = true;

        //std::cout << "accept socket: " << unSessionID << std::endl;

        // Read message
        asio::async_read_until(*pSocket.get(),
            m_buffer, '\n', std::bind(&TCPServer::onMessageReceived, this, std::placeholders::_1, unSessionID, std::placeholders::_2));
    }
    else
    {
        //the corresponding message is output to the standard output stream.
        std::cout << "Error occured! Error code = "
            << ec.value()
            << ". Message: " << ec.message();
        return false;
    }
    return true;
}

// Send message
bool TCPServer::requestWrite(const std::string& strReq)
{
    // On Server, send to all client
    for (const auto& pSocket : m_lstSocket)
    {
        if (!pSocket.second->is_open())
        {
            return false;
        }
        boost::system::error_code error;
        std::string cmd = strReq + "\n";

        boost::asio::write(*pSocket.second.get(), boost::asio::buffer(cmd), error);
    }
    return true;
}

// Stop Server
void TCPServer::stop()
{
    // Send shutdown message to client
    requestWrite("Srv.shutdown");
    m_thrd.interrupt();

    // Close all Client
    for (const auto& pSocket : m_lstSocket)
    {
        pSocket.second->close();
    }
    m_lstIsPairing.clear();
    m_ios.stop();
    m_acceptor.close();
}

// Start TCP Server
bool TCPServer::startServer()
{
    // init Server
    init();
    // Start ios thread
    startThread();
    return true;
}

// Receive and call back to handle
void TCPServer::onMessageReceived(const boost::system::error_code& ec
    , const boost::asio::ip::tcp::socket::native_handle_type& sessionID
    , std::size_t bytes_transferred)
{
    auto bufs = m_buffer.data();
    std::string result(buffers_begin(bufs), buffers_begin(bufs) + bytes_transferred);

    // If call back func is set
    if (m_func)
    {
        // Call back
        m_func(result);
    }

    // If Client shutdown
    if (ec == boost::asio::error::connection_reset)
    {
        if (m_lstSocket.count(sessionID) == 1)
        {
            // Close client
            m_lstSocket[sessionID]->close();
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_lstSocket.erase(sessionID);

            m_lstIsPairing.erase(sessionID);
            //std::cout << "socket: " << sessionID << " shutdown" << std::endl;
        }
        return;
    }
    //std::cout << "nhan message: " << result << std::endl;
    m_buffer.consume(bytes_transferred);

    // Read another message
    asio::async_read_until(*m_lstSocket[sessionID].get(),
        m_buffer, '\n', std::bind(&TCPServer::onMessageReceived, this, std::placeholders::_1, sessionID, std::placeholders::_2));
}

// Check if server with client is pairing
bool TCPServer::isPairing()
{
    // Check all client
    for (const auto& blPair : m_lstIsPairing)
    {
        // True if aleast one pair
        if (blPair.second)
        {
            return true;
        }
    }

    return false;
}

/**************************************** TCPClient ******************************************/
// Contructor
TCPClient::TCPClient() : TCPConnect()
{
    m_blPairing = false;
}

// Destructor
TCPClient::~TCPClient()
{
    // Stop connect to Server
    stop();
}

// Init Client
void TCPClient::init()
{
    m_pSocket = std::make_shared<asio::ip::tcp::socket>(m_ios);
}

// Stop Client
void TCPClient::stop()
{
    m_thrd.interrupt();
    m_pSocket->close();
    m_ios.stop();

    m_blPairing = false;
}

// Request connect to Server
bool TCPClient::requestConnect(const std::string& strIP, const int& nPort)
{
    // Start thread ios
    startThread();
    // Wait to connect
    m_pSocket->async_connect(asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(strIP), nPort), std::bind(&TCPClient::onConnectCompleted, this, std::placeholders::_1));

    return true;
}

// Send message to Server
bool TCPClient::requestWrite(const std::string& strReq)
{
    // If socket is not open
    if (!m_pSocket->is_open())
    {
        return false;
    }
    boost::system::error_code error;
    std::string cmd = strReq + "\n";

    // Send message
    boost::asio::write(*m_pSocket.get(), boost::asio::buffer(cmd), error);

    return true;
}

// When connected to Server, start read message
bool TCPClient::onConnectCompleted(const system::error_code& ec)
{
    if (ec)
    {
        std::cout << "Failed to connect. Error: " << ec.message() << std::endl;
        return false;
    }
    else
    {
        m_blPairing = true;

        // Wait read message
        asio::async_read_until(*m_pSocket.get(),
            m_buffer, '\n', std::bind(&TCPClient::onMessageReceived, this, std::placeholders::_1, std::placeholders::_2));
        return true;
    }
}

// Check if Client is connect to Server
bool TCPClient::isConnect()
{
    if (!m_pSocket->is_open())
    {
        return false;
    }
    return true;
}

// Close Client
void TCPClient::closeSocket()
{
    m_pSocket->close();
    m_blPairing = false;
}

// Receive message
void TCPClient::onMessageReceived(const boost::system::error_code& ec, std::size_t bytes_transferred)
{
    auto bufs = m_buffer.data();
    std::string result(buffers_begin(bufs), buffers_begin(bufs) + bytes_transferred);

    // If message shutdown from Server
    if (result == "Srv.shutdown")
    {
        // Close Client
        closeSocket();
        return;
    }

    // If Callback func is set
    if (m_func)
    {
        // Callback
        m_func(result);
    }
    if (ec == boost::asio::error::connection_reset)
    {
        closeSocket();
        return;
    }

    //std::cout << "nhan message: " << result << std::endl;
    m_buffer.consume(bytes_transferred);

    // Wait read message
    asio::async_read_until(*m_pSocket.get(),
        m_buffer, '\n', std::bind(&TCPClient::onMessageReceived, this, std::placeholders::_1, std::placeholders::_2));
}

// Check if Client is pairing with Server
bool TCPClient::isPairing()
{
    return m_blPairing;
}
