#include "stdafx.h"

#include "SystemCommon.h"
#include "Common.h"
#include "ControlLogic.h"
#include "TCPConnect.h"

/*
 * Let's the game start
 */

 // Show at the begining of the game
ControlLogicType showStartMenu(std::shared_ptr<TCPServer> pServer, std::shared_ptr<TCPClient> pClient)
{
	std::system("cls");
	std::cout << "Welcome to snake game!" << std::endl;
	std::cout << "Options:" << std::endl;
	std::cout << "1. Single" << std::endl;
	std::cout << "2. Muti" << std::endl;
	std::cout << "3. Quit" << std::endl;
	std::cout << "Your choice: ";
	int option;
	std::cin >> option;
	if (option == 1)
	{
		std::system("cls");
		std::cout << "Tip: While playing game, you can press 'q' to quit";
		ConsoleFunc::gotoxy(0, 3);
		std::cout << "Ready!";
		Sleep(1000);
		for (size_t i = 3; i > 0; i--)
		{
			ConsoleFunc::gotoxy(0, 3);
			std::cout << i << "         ";
			Sleep(1000);
		}
		ConsoleFunc::gotoxy(0, 3);
		std::cout << "GO!";
		Sleep(1000);

		return ControlLogicType::Single;
	}
	else if (option == 2)
	{
		std::system("cls");
		std::cout << "Choose Mode" << std::endl;
		std::cout << "1. Server" << std::endl;
		std::cout << "2. Client" << std::endl;
		int nChoose = 0;
		std::cin >> nChoose;
		if (nChoose == 1)
		{
			pServer->startServer();
			return ControlLogicType::Server;
		}
		else if (nChoose == 2)
		{
			pClient->init();
			pClient->requestConnect("127.0.0.1", 9003);

			return ControlLogicType::Client;
		}
	}
	else if (option == 3)
		exit(1);
}
int main()
{
	std::shared_ptr<asio::io_service> pService = std::make_shared<asio::io_service>();
	std::shared_ptr<TCPServer> pServer = std::make_shared<TCPServer>();
	std::shared_ptr<TCPClient> pClient = std::make_shared<TCPClient>();

	ControlLogicType enmType = showStartMenu(pServer, pClient);
	std::shared_ptr<ControlLogic> pControlLogic;

	std::string cmd = "";
	switch (enmType)
	{
	case ControlLogicType::Single:
		pControlLogic = std::make_shared<ControlLogic>(enmType);
		break;
	case ControlLogicType::Server:
		// Todo: check server open
		pControlLogic = std::make_shared<ControlLogic>(enmType, pService, pServer);
		break;
		/*while (true)
		{
			std::cin >> cmd;
			if (cmd == "q")
			{
				break;

			}
			else
			{
				pServer->requestWrite(cmd);
			}
		}*/
	case ControlLogicType::Client:
		pControlLogic = std::make_shared<ControlLogic>(enmType, pService, pClient);
		break;
		/*while (true)
		{
			std::cin >> cmd;
			if (cmd == "q")
			{
				break;

			}
			else
			{
				pClient->requestWrite(cmd);
			}
		}*/
	default:
		break;
	}
	if(pControlLogic)
	pControlLogic->startGame();

	return 0;
}




