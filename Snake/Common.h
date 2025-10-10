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
class Point
{
public:
	Point() :x(0), y(0)
	{

	}

	Point(int inx, int iny)
		: x(inx)
		, y(iny)
	{}

	friend class boost::serialization::access;
	// Convert msg to binary
	const std::vector<char> toBinary() const
	{
		std::ostringstream stream;
		boost::archive::binary_oarchive archive(stream, boost::archive::no_header);
		archive << *this;

		std::string adapter = stream.str();
		return std::vector<char>(adapter.begin(), adapter.end());
	}

	// Boost serialize
	template <typename Archive>
	void serialize(Archive& ar, const unsigned int)
	{
		ar& x;
		ar& y;
	}

	// show details as string
	const std::string toString() const
	{
		return boost::io::str(boost::format("(x: %1%, y: %2%)") % x % y);
	}

	int x;
	int y;
};
BOOST_CLASS_VERSION(Point, 1)

enum ControlLogicType
{
	None = 0
	, Single
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


