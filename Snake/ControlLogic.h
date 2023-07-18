#pragma once
#include "stdafx.h"

#include "Snake.h"
#include "TCPConnect.h"
#include "MessageID.h"
#include "Timer.h"
#include "MessageQueued.h"

// Timer resiger
#define ONTIMER_REG_HANDLE(timerid, func) m_pTimer->registerTimer(timerid, std::bind(&ControlLogic::func, this, std::placeholders::_1, std::placeholders::_2))

// Class control logic of game
class ControlLogic
{
public:
	ControlLogic(const ControlLogicType& enmType, std::shared_ptr<asio::io_service> pService, std::shared_ptr<TCPConnect> pTCPConnect);
	ControlLogic(const ControlLogicType& enmType);

	// Start game
	void startGame();

	/*********************** Handle request **************************/
	bool handleSynchroReq(const jsoncons::json& objSynchroReq);
	bool handleWinningNotice(const jsoncons::json& objSynchroReq);
	bool handleUpdateDirectionReq(const jsoncons::json& objUpdateDirectionReq);
	bool requestSynchro();

	// Receive message
	bool onRecvMessage(const std::string& strMessage);
private:
	bool initGame();
	void drawBox();
	void genApple();
	void displayScore();
	void showEndMenu();
	void createDispathTable();

	// Timer for send message
	HandlerResult onTimer_SendMessageLoop(const KeyTimer& timerKey, const boost::posix_time::milliseconds& timeoutValue);

protected:
	std::map<ControlLogicType, std::shared_ptr<Snake>> m_lstSnake;	// Snake list (Multi mode)

	std::map<std::string, std::function<bool(const jsoncons::json&)>> m_lstDispathTable;	// Dispath message table

	std::vector<Point> m_lstApple;	// Apple list

	std::map<ControlLogicType, int> m_lstScore;	// Score list

	int m_unSpeed;	// Speed of game

	ControlLogicType m_enmType;	// Type of game

	std::shared_ptr<TCPConnect> m_pTCPConnect;	// TCP connect

	bool m_blGameRunning;

	std::mutex m_Mutex;

	std::shared_ptr<TimerControler> m_pTimer;	// Timer

	std::shared_ptr<MessageQueued> m_pMessageQueued;	// Message queued for send message

	std::shared_ptr<asio::io_service> m_pService;	// ios service
};
