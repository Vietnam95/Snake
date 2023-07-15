#pragma once
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


