#pragma once
#include "stdafx.h"
/*
* MACRO
*/
#define WIDTH 40
#define HEIGHT 20
#define BODY '*'
#define APPLE 'O'

/*
* Enum
*/
enum class Direction
{
	up,
	right,
	down,
	left
};

// Each point is a part of the snake
struct Point
{
	Point(int inx, int iny)
		: x(inx)
		, y(iny)
	{}

	int x;
	int y;
};

enum ControlLogicType
{
	Single = 0
	, Server
	, Client

};

// Convert Enum to String
inline std::string EToString(ControlLogicType v)
{
	switch (v)
	{
	case ControlLogicType::Single:		return "Single";
	case ControlLogicType::Server:		return "Server";
	case ControlLogicType::Client:		return "Client";
	default:							return "Unknown";
	}
}

namespace common
{
	// Expand message
	template <typename MSG>
	inline bool expandMessage(const std::vector<char> message, MSG& objMsg)
	{
		// If message empty do nothing and return
		if (message.empty())
		{
			return false;
		}

		try
		{
			std::istringstream stream(std::string(message.begin(), message.end()));
			boost::archive::binary_iarchive archive(stream, boost::archive::no_header);
			archive >> objMsg;
		}
		catch (std::exception& exception)
		{
			std::cout << "exception: " << exception.what() << std::endl;
			return false;
		}
		return true;
	}
}


