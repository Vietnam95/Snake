#pragma once
#include "stdafx.h"

enum HandlerResult
{
    Success = 0
    , Failure
    , RestartTimer
};

enum TimerId : int
{
    TimerId_SendMessageLoop = 0
};

struct KeyTimer
{
public:
    int id_;
    int param_;

    KeyTimer();

    KeyTimer(const int id, const int& param = 0);

    bool operator==(const KeyTimer& otherKey) const
    {
        return id_ == otherKey.id_ && param_ == otherKey.param_;
    }

    bool operator<(const KeyTimer& otherKey) const
    {
        if (id_ < otherKey.id_)
        {
            return true;
        }

        return id_ == otherKey.id_ && param_ < otherKey.param_;
    }
};

class InfoTimer
{
public:
    InfoTimer();

    KeyTimer                                        m_KeyTimer;
    std::shared_ptr<boost::asio::deadline_timer>    m_timer;
    bool                                            m_blIsRunning;
    boost::posix_time::milliseconds                 m_tmTimeoutValue;
};

class TimerControler
{
protected:
    std::map<KeyTimer, InfoTimer>                   m_lstInfoTimer;
    std::shared_ptr<boost::asio::io_service>        m_pService;

    std::mutex                                      m_Mutex;

    std::map<KeyTimer, std::function<HandlerResult(const KeyTimer&, const boost::posix_time::milliseconds)>> m_lstHandler;

public:
    TimerControler(std::shared_ptr<boost::asio::io_service> pService);

    virtual ~TimerControler();

    bool registerTimer(const KeyTimer& timerKey, std::function<HandlerResult(const KeyTimer&, const boost::posix_time::milliseconds&)> func);

    bool startTimer(const KeyTimer& timerKey, const boost::posix_time::milliseconds& tmTimeOutValue);

    bool stopTimer(const KeyTimer& timerKey);

    bool onExecute(const KeyTimer& timerKey, const boost::posix_time::milliseconds&, const boost::system::error_code& error);
};