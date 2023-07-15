#include "stdafx.h"

#include "Timer.h"

KeyTimer::KeyTimer()
    : id_(0)
    , param_(0)
{}

KeyTimer::KeyTimer(const int id, const int& param)
    : id_(id)
    , param_(param)
{}

InfoTimer::InfoTimer()
    : m_KeyTimer()
    , m_timer()
    , m_blIsRunning(false)
    , m_tmTimeoutValue(0)
{
}

TimerControler::TimerControler(std::shared_ptr<boost::asio::io_service> pService)
    : m_pService(pService)
{
}

TimerControler::~TimerControler()
{
    for (const auto& objTimer : m_lstInfoTimer)
    {
        stopTimer(objTimer.first);
    }
}

bool TimerControler::registerTimer(const KeyTimer& timerKey, std::function<HandlerResult(const KeyTimer&, const boost::posix_time::milliseconds&)> objFunc)
{
    bool blRet = true;

    m_lstHandler.emplace(timerKey, objFunc);

    std::shared_ptr<boost::asio::deadline_timer> pTimer = std::make_shared<boost::asio::deadline_timer>(*m_pService);
    InfoTimer objTimerInfo;
    objTimerInfo.m_KeyTimer = timerKey;
    objTimerInfo.m_timer = pTimer;

    m_lstInfoTimer[timerKey] = objTimerInfo;

    return blRet;
}

bool TimerControler::startTimer(const KeyTimer& timerKey, const boost::posix_time::milliseconds& tmTimeOutValue)
{
    bool blRet = false;

    const auto& it = m_lstInfoTimer.find(timerKey);

    if (it == m_lstInfoTimer.end())
    {
       // "Timer not exist! Timer ID: %1%") % timerKey.id_));
        return blRet;
    }

    InfoTimer& objTimerInfo = it->second;

    if (objTimerInfo.m_blIsRunning)
    {
        // "Timer is running! Timer ID: %1%") % timerKey.id_));
        return blRet;
    }

    objTimerInfo.m_blIsRunning = true;
    objTimerInfo.m_tmTimeoutValue = tmTimeOutValue;

    objTimerInfo.m_timer->expires_from_now(objTimerInfo.m_tmTimeoutValue);

    objTimerInfo.m_timer->async_wait(boost::bind(&TimerControler::onExecute, this, timerKey, objTimerInfo.m_tmTimeoutValue, boost::asio::placeholders::error));

    boost::thread t(boost::bind(&boost::asio::io_service::run, m_pService));

    t.detach();

    blRet = true;
    return blRet;
}

bool TimerControler::stopTimer(const KeyTimer& timerKey)
{
    bool blRet = false;

    const auto& it = m_lstInfoTimer.find(timerKey);

    if (it == m_lstInfoTimer.end())
    {
        // "Timer not exist! Timer ID: %1%") % timerKey.id_));
        return blRet;
    }

    InfoTimer& objTimerInfo = it->second;

    if (!objTimerInfo.m_blIsRunning)
    {
        blRet = true;
        return blRet;
    }

    boost::system::error_code error;
    objTimerInfo.m_timer->cancel(error);
    if (error)
    {
        // "Stop Timer Fail! Timer ID: %1%, Error: %2%") % timerKey.id_ % error.value()));
        return blRet;
    }
    objTimerInfo.m_blIsRunning = false;

    blRet = true;
    return blRet;
}

bool TimerControler::onExecute(const KeyTimer& timerKey, const boost::posix_time::milliseconds& /*tmTimeOutValue*/, const boost::system::error_code& error)
{
    bool blRet = false;
    std::lock_guard<std::mutex> lock(m_Mutex);

    if (error == boost::asio::error::operation_aborted)
    {
        return blRet;
    }
    const auto& it = m_lstInfoTimer.find(timerKey);

    if (it == m_lstInfoTimer.end())
    {
        return blRet;
    }
    InfoTimer& objTimerInfo = it->second;

    if (!objTimerInfo.m_blIsRunning)
    {
        return blRet;
    }

    HandlerResult enmHandlerResult = m_lstHandler[timerKey](timerKey, objTimerInfo.m_tmTimeoutValue);

    switch (enmHandlerResult)
    {
    case Success:
    case Failure:
    {
        stopTimer(timerKey);
    }
    break;
    case RestartTimer:
    {
        objTimerInfo.m_timer->expires_from_now(objTimerInfo.m_tmTimeoutValue);
        objTimerInfo.m_timer->async_wait(boost::bind(&TimerControler::onExecute, this, timerKey, it->second.m_tmTimeoutValue, boost::asio::placeholders::error));
    }
    break;
    default:
        break;
    }

    blRet = true;
    return blRet;
}