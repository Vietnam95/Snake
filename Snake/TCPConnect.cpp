#pragma once
#include "stdafx.h"

#include "TCPConnect.h"

using namespace boost;

/**************************************** TCPConnect ******************************************/
TCPConnect::TCPConnect()
{
}

void TCPConnect::startThread()
{
    m_thrd = boost::thread(&TCPConnect::runService, this);
    m_thrd.detach();
}

bool TCPConnect::requestWrite(const std::string& strReq)
{
    if (!m_pSocket->is_open())
    {
        return false;
    }
    boost::system::error_code error;
    std::string cmd = strReq + "\n";

    boost::asio::write(*m_pSocket.get(), boost::asio::buffer(cmd), error);

    return true;
}

void TCPConnect::onMessageReceived(const boost::system::error_code& ec, std::size_t bytes_transferred)
{
    auto bufs = m_buffer.data();
    std::string result(buffers_begin(bufs), buffers_begin(bufs) + bytes_transferred);

    if (result == "Srv.shutdown")
    {
        closeSocket();
        return;
    }
    if (m_func)
    {
        m_func(result);
    }
    if (ec == boost::asio::error::connection_reset)
    {
        closeSocket();
        return;
    }

    m_buffer.consume(bytes_transferred);
    asio::async_read_until(*m_pSocket.get(),
        m_buffer, '\n', std::bind(&TCPConnect::onMessageReceived, this, std::placeholders::_1, std::placeholders::_2));
}

bool TCPConnect::setMessageReceivedHandler(std::function<bool(const std::string&)> func)
{
    if (!func)
    {
        return false;
    }

    m_func = func;

    return true;
}

void TCPConnect::onAccept(const system::error_code& ec,
    std::shared_ptr<asio::ip::tcp::socket> pSocket)
{
    if (ec.value() == 0)
    {
        m_blPairing = true;

        asio::async_read_until(*pSocket.get(),
            m_buffer, '\n', std::bind(&TCPConnect::onMessageReceived, this, std::placeholders::_1, std::placeholders::_2));
    }
    else
    {
        //the corresponding message is output to the standard output stream.
        std::cout << "Error occured! Error code = "
            << ec.value()
            << ". Message: " << ec.message();
    }
}

void TCPConnect::runService()
{
    m_ios.run();
}

void TCPConnect::closeSocket()
{
    m_pSocket->close();
    m_blPairing = false;
}

bool TCPConnect::isPairing()
{
    return m_blPairing;
}

/**************************************** TCPServer ******************************************/
TCPServer::TCPServer() : TCPConnect()
, m_acceptor(m_ios, asio::ip::tcp::endpoint(asio::ip::address_v4::any(), 9003))
{
    m_blPairing = false;
}

TCPServer::~TCPServer()
{
    stop();
}

void TCPServer::init()
{
    m_acceptor.listen();
    m_pSocket = std::make_shared<asio::ip::tcp::socket>(m_ios);
    m_acceptor.async_accept(*m_pSocket.get(), std::bind(&TCPConnect::onAccept, this, std::placeholders::_1, m_pSocket));
}

void TCPServer::stop()
{
    requestWrite("Srv.shutdown");
    m_thrd.interrupt();
    m_pSocket->close();
    m_ios.stop();
    m_acceptor.close();
}

bool TCPServer::startServer()
{
    init();
    startThread();

    return true;
}

/**************************************** TCPClient ******************************************/
TCPClient::TCPClient() : TCPConnect()
{
    m_blPairing = false;
}

TCPClient::~TCPClient()
{
    stop();
}

void TCPClient::init()
{
    m_pSocket = std::make_shared<asio::ip::tcp::socket>(m_ios);
}

void TCPClient::stop()
{
    m_thrd.interrupt();
    m_pSocket->close();
    m_ios.stop();

    m_blPairing = false;
}

bool TCPClient::requestConnect(const std::string& strIP, const int& nPort)
{
    startThread();
    m_pSocket->async_connect(asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(strIP), nPort), std::bind(&TCPClient::onConnectCompleted, this, std::placeholders::_1));

    return true;
}

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
        asio::async_read_until(*m_pSocket.get(),
            m_buffer, '\n', std::bind(&TCPConnect::onMessageReceived, this, std::placeholders::_1, std::placeholders::_2));
        return true;
    }
}

bool TCPClient::isConnect()
{
    if (!m_pSocket->is_open())
    {
        return false;
    }
    return true;
}
