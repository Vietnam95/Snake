#pragma once
#include "stdafx.h"

#include "ControlLogic.h"

// Contructor for muti mode
ControlLogic::ControlLogic(const ControlLogicType& enmType, std::shared_ptr<asio::io_service> pService, std::shared_ptr<TCPConnect> pTCPConnect)
	: m_unSpeed(300)
	, m_enmType(enmType)
	, m_pTCPConnect(pTCPConnect)
	, m_pService(pService)
	, m_blGameRunning(false)
{
	m_lstScore[ControlLogicType::Server] = 0;
	m_lstScore[ControlLogicType::Client] = 0;

	// create dispatch message table
	createDispathTable();

	m_pTimer = std::make_shared<TimerControler>(m_pService);
	m_pMessageQueued = std::make_shared<MessageQueued>();

	ONTIMER_REG_HANDLE(KeyTimer(TimerId_SendMessageLoop), onTimer_SendMessageLoop);
}

// Contructor for single mode
ControlLogic::ControlLogic(const ControlLogicType& enmType)
	: m_unSpeed(300)
	, m_enmType(enmType)
	, m_blGameRunning(false)
{
	m_lstScore[enmType] = 0;
}

// Init game logic
bool ControlLogic::initGame()
{
	switch (m_enmType)
	{
	case ControlLogicType::Single:
	{
		m_lstSnake[ControlLogicType::Single] = std::make_shared<Snake>(ControlLogicType::Single);
	}
	break;
	case ControlLogicType::Server:
	case ControlLogicType::Client:
	{
		m_lstSnake[ControlLogicType::Server] = std::make_shared<Snake>(ControlLogicType::Server);
		m_lstSnake[ControlLogicType::Client] = std::make_shared<Snake>(ControlLogicType::Client);

		// set handle message TCPConnect::onMessageReceived -> ControlLogic::onRecvMessage
		m_pTCPConnect->setMessageReceivedHandler(std::bind(&ControlLogic::onRecvMessage, this, std::placeholders::_1));
	}
	break;
	default:
		break;
	}

	m_blGameRunning = true;

	return true;
}

// Draw snakeboard with WIDTH and HEIGHT was set
void ControlLogic::drawBox()
{
	for (size_t i = 0; i < WIDTH; i++)
		std::cout << '=';
	ConsoleFunc::gotoxy(0, HEIGHT);
	for (size_t i = 0; i < WIDTH; i++)
		std::cout << '=';
	for (size_t i = 1; i < HEIGHT; i++)
	{
		ConsoleFunc::gotoxy(0, i);
		std::cout << '|';
	}
	for (size_t i = 1; i < HEIGHT; i++)
	{
		ConsoleFunc::gotoxy(WIDTH, i);
		std::cout << '|';
	}
}

// Generate apple on the board
void ControlLogic::genApple()
{
	srand(time(0));
	int x = rand() % (WIDTH - 1) + 1;
	int y = rand() % (HEIGHT - 1) + 1;

	m_lstApple.push_back(Point(x, y));

	ConsoleFunc::gotoxy(x, y);
	std::cout << APPLE;
}

// Show score on right side of the board
void ControlLogic::displayScore()
{
	int nCount = 0;
	for(const auto& nScore: m_lstScore)
	{
		ConsoleFunc::gotoxy(WIDTH + 5, 2 + nCount);
		std::cout << "Snake: "<< nScore.first << " - score: " << nScore.second;
		nCount++;
	}
}


// Show menu at the end of the game
void ControlLogic::showEndMenu()
{
	if (m_enmType != ControlLogicType::Single)
	{
		jsoncons::json objNotice;
		objNotice["messageId"] = "WinningNotice";
		m_pMessageQueued->registerMessage(objNotice);
	}

	m_blGameRunning = false;
	std::system("cls");
	ConsoleFunc::gotoxy(0, 0);
	std::cout << "End game!" << std::endl;
	std::cout << "Your score: " << m_lstScore[m_enmType] << std::endl;
	std::cout << "Do you want to play again ([y]/[n]): ";
	char option;
	std::cin >> option;
	option = tolower(option);
	if (option == 'y')
	{
		for (const auto& pSnake : m_lstSnake)
		{
			pSnake.second->reBorn();
		}

		startGame();
	}
	else if (option == 'n')
		exit(1);
}

// Start game
void ControlLogic::startGame()
{
	// In muti mode, wait connect before start game
	switch (m_enmType)
	{
	case ControlLogicType::Server:
	case ControlLogicType::Client:
		std::system("cls");
			std::cout << "Waiting to Connect!" << std::endl;
			while (!m_pTCPConnect->isPairing())
			{

			}
			break;
	default:
		break;
	}

	// init game logic
	initGame();

	std::system("cls");
	ConsoleFunc::ShowConsoleCursor(false);

	drawBox();

	// show snakes
	for (const auto& pSnake : m_lstSnake)
	{
		pSnake.second->show();
	}

	// genarate apple
	genApple();

	// show score
	displayScore();

	if (m_enmType != ControlLogicType::Single)
	{
		// Timer send message
		m_pTimer->startTimer(TimerId_SendMessageLoop, boost::posix_time::milliseconds(200));
	}

	while (true && m_blGameRunning)
	{
		if (_kbhit())
		{
			char ch = _getch();
			ch = tolower(ch);
			if (ch == 'a' && m_lstSnake[m_enmType]->getDirection() != Direction::right)
				m_lstSnake[m_enmType]->updateDirection(Direction::left);
			else if (ch == 'w' && m_lstSnake[m_enmType]->getDirection() != Direction::down)
				m_lstSnake[m_enmType]->updateDirection(Direction::up);
			else if (ch == 's' && m_lstSnake[m_enmType]->getDirection() != Direction::up)
				m_lstSnake[m_enmType]->updateDirection(Direction::down);
			else if (ch == 'd' && m_lstSnake[m_enmType]->getDirection() != Direction::left)
				m_lstSnake[m_enmType]->updateDirection(Direction::right);
			else if (ch == 'q') // Quit game
			{
				showEndMenu();
				break;
			}
			if (m_enmType == ControlLogicType::Client)
			{
				jsoncons::json objMsgUpdateDirecReq;
				objMsgUpdateDirecReq["messageId"] = "updateDirectionReq";
				objMsgUpdateDirecReq["Type"] = static_cast<int>(m_enmType);
				objMsgUpdateDirecReq["Direction"] = static_cast<int>(m_lstSnake[m_enmType]->getDirection());

				m_pMessageQueued->registerMessage(objMsgUpdateDirecReq);
			}
		}
		for (const auto& pSnake : m_lstSnake)
		{
			// Only Server (in multi mode) or Single can move the snake
			if (m_enmType == ControlLogicType::Server
				|| m_enmType == ControlLogicType::Single)
			{
				// move the snake
				pSnake.second->move();
			}

			// draw head and remove old tail
			pSnake.second->drawHeadnTail();

			// if snake ate apple, let him grow up
			if (pSnake.second->isAteApple(m_lstApple[0]))
			{
				m_lstScore[pSnake.first]++;
				displayScore();
				pSnake.second->growing();
				m_lstApple.clear();
				genApple();
			}

			// if snake bite it self, him die
			if (pSnake.second->isBiteItself())
			{
				ConsoleFunc::ShowConsoleCursor(true);
				showEndMenu();
				break;
			}

			// if snake hit the wall, him die
			if (pSnake.second->isHitWall())
			{
				ConsoleFunc::ShowConsoleCursor(true);
				showEndMenu();
				break;
			}
		}
		Sleep(m_unSpeed);
	}
}

// handle synchro request from server/client
bool ControlLogic::handleSynchroReq(const jsoncons::json& objSynchro)
{
	if (m_enmType != ControlLogicType::Client)
	{
		return false;
	}

	std::lock_guard<std::mutex> lock(m_Mutex);
	try
	{
		auto jsonSnake = objSynchro.get("Snake").array_range();

		for (const auto& snake : jsonSnake)
		{
			ControlLogicType enmType = static_cast<ControlLogicType>(snake.get("Type").as<int>());

			m_lstSnake[enmType]->synchro(snake);
		}

		auto jsonApple = objSynchro.get("Apple").array_range();
		std::vector<Point> lstApple;
		for (const auto& apple : jsonApple)
		{
			Point objNewApple(apple.get("x").as<int>(), apple.get("y").as<int>());
			lstApple.push_back(objNewApple);
		}

		auto jsonScore = objSynchro.get("Score");
		std::map<ControlLogicType, int> lstScore;

		lstScore[ControlLogicType::Server] = jsonScore.get(EToString(ControlLogicType::Server)).as<int>();
		lstScore[ControlLogicType::Client] = jsonScore.get(EToString(ControlLogicType::Client)).as<int>();

		m_lstApple = lstApple;
		m_lstScore = lstScore;

		return true;
	}
	catch (std::exception& /*exception*/)
	{
		return false;
	}
}

bool ControlLogic::requestSynchro()
{
	if (!m_blGameRunning)
	{
		return false;
	}

	jsoncons::json objReqSynchro;

	objReqSynchro["messageId"] = "SynchroReq";

	jsoncons::json lstReqSnake = jsoncons::json::array();

	for (const auto& objSnake : m_lstSnake)
	{
		jsoncons::json objReqSnake;
		objReqSnake["Type"] = static_cast<int>(objSnake.first);
		Point PreTail = objSnake.second->getPreTail();

		jsoncons::json jsonPreTail;
		jsonPreTail["x"] = PreTail.x;
		jsonPreTail["y"] = PreTail.y;
		objReqSnake["PreTail"] = jsonPreTail;

		objReqSnake["Direction"] = static_cast<int>(objSnake.second->getDirection());

		std::vector<Point> lstSnakePoint = objSnake.second->getSnake();

		jsoncons::json jsonPoint;
		jsoncons::json lstPoint = jsoncons::json::array();

		for (const auto& point : lstSnakePoint)
		{
			jsonPoint["x"] = point.x;
			jsonPoint["y"] = point.y;
			lstPoint.push_back(jsonPoint);
		}
		objReqSnake["Point"] = lstPoint;

		lstReqSnake.push_back(objReqSnake);
	}

	objReqSynchro["Snake"] = lstReqSnake;

	jsoncons::json jsonApple;
	jsoncons::json lstApple = jsoncons::json::array();
	for (const auto& objApple : m_lstApple)
	{
		jsonApple["x"] = objApple.x;
		jsonApple["y"] = objApple.y;

		lstApple.push_back(jsonApple);
	}

	objReqSynchro["Apple"] = lstApple;

	jsoncons::json jsonScore;
	for (const auto& objScore : m_lstScore)
	{
		jsonScore[EToString(objScore.first)] = objScore.second;
	}

	objReqSynchro["Score"] = jsonScore;

	if (!m_pMessageQueued->registerMessage(objReqSynchro))
	{
		return false;
	}
	return true;
}

bool ControlLogic::onRecvMessage(const std::string& strMessage)
{
	try
	{
		jsoncons::json objRecvReq = jsoncons::json::parse(strMessage);

		std::string strMessageID = objRecvReq.get("messageId").as_string();

		if (m_lstDispathTable.count(strMessageID) == 1)
		{
			m_lstDispathTable[strMessageID](objRecvReq);
		}
		else
		{
			return false;
		}
	}
	catch (std::exception& /*exception*/)
	{
		return false;
	}

	return true;
}

bool ControlLogic::handleWinningNotice(const jsoncons::json& /*objSynchroReq*/)
{
	std::lock_guard<std::mutex> lock(m_Mutex);

	m_blGameRunning = false;

	//std::system("cls");
	//std::cout << "You Won!" << std::endl;

	return true;
}

bool ControlLogic::handleUpdateDirectionReq(const jsoncons::json& objUpdateDirectionReq)
{
	std::lock_guard<std::mutex> lock(m_Mutex);
	try
	{
		ControlLogicType enmType = static_cast<ControlLogicType>(objUpdateDirectionReq.get("Type").as<int>());
		Direction enmDirection = static_cast<Direction>(objUpdateDirectionReq.get("Direction").as<int>());
		m_lstSnake[enmType]->updateDirection(enmDirection);

		return true;
	}
	catch (std::exception& /*exception*/)
	{
		return false;
	}
	return true;
}

void ControlLogic::createDispathTable()
{
	m_lstDispathTable[msg::id::D_SynchroReq] = std::bind(&ControlLogic::handleSynchroReq, this, std::placeholders::_1);
	m_lstDispathTable[msg::id::D_WinningNotice] = std::bind(&ControlLogic::handleWinningNotice, this, std::placeholders::_1);
	m_lstDispathTable[msg::id::D_UpdateDirectionReq] = std::bind(&ControlLogic::handleUpdateDirectionReq, this, std::placeholders::_1);
}

HandlerResult ControlLogic::onTimer_SendMessageLoop(const KeyTimer& /*timerKey*/, const boost::posix_time::milliseconds& /*timeoutValue*/)
{
	// if single game, send nothing
	if (m_enmType == ControlLogicType::Single)
	{
		return HandlerResult::Failure;
	}

	if (m_enmType == ControlLogicType::Server)
	{
		// send req synchro to Server->Client
		requestSynchro();
	}
	// Get queued message
	std::deque<jsoncons::json> lstMessageQueued = m_pMessageQueued->queueMsgHandling();

	for (const auto& msg : lstMessageQueued)
	{
		// sent message
		if (!m_pTCPConnect->requestWrite(msg.to_string()))
		{
			return HandlerResult::Failure;
		}
	}

	m_pMessageQueued->clearQueue();

	return HandlerResult::RestartTimer;
}




