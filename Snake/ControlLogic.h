#pragma once
#include "stdafx.h"

#include "Snake.h"
#include "TCPConnect.h"
#include "MessageID.h"
#include "Timer.h"
#include "MessageQueued.h"

// Timer resiger
#define ONTIMER_REG_HANDLE(timerid, func) m_pTimer->registerTimer(timerid, std::bind(&ControlLogic::func, this, std::placeholders::_1, std::placeholders::_2))

class ControlLogic
{
public:
	ControlLogic(const ControlLogicType& enmType, std::shared_ptr<asio::io_service> pService, std::shared_ptr<TCPConnect> pTCPConnect);
	ControlLogic(const ControlLogicType& enmType);

	void startGame();
	bool handleSynchroReq(const jsoncons::json& objSynchroReq);
	bool handleWinningNotice(const jsoncons::json& objSynchroReq);
	bool handleUpdateDirectionReq(const jsoncons::json& objUpdateDirectionReq);
	bool requestSynchro();
	bool onRecvMessage(const std::string& strMessage);
private:
	bool initGame();
	void drawBox();
	void genApple();
	void displayScore();
	void showEndMenu();
	void createDispathTable();
	HandlerResult onTimer_SendMessageLoop(const KeyTimer& timerKey, const boost::posix_time::milliseconds& timeoutValue);

protected:
	std::map<ControlLogicType, std::shared_ptr<Snake>> m_lstSnake;

	std::map<std::string, std::function<bool(const jsoncons::json&)>> m_lstDispathTable;

	std::vector<Point> m_lstApple;

	std::map<ControlLogicType, int> m_lstScore;

	int m_unSpeed;

	ControlLogicType m_enmType;

	std::shared_ptr<TCPConnect> m_pTCPConnect;

	bool m_blGameRunning;

	std::mutex m_Mutex;

	std::shared_ptr<TimerControler> m_pTimer;

	std::shared_ptr<MessageQueued> m_pMessageQueued;

	std::shared_ptr<asio::io_service> m_pService;
};
