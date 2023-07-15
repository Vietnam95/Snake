#pragma once
#include "stdafx.h"

#include "Snake.h"

Snake::Snake(const ControlLogicType& enmType):m_objPrevTail(0, 0), m_enmDerection(Direction::right)
{
	init(enmType);
}

void Snake::init(const ControlLogicType& enmType)
{
	if (enmType == ControlLogicType::Server)
	{
		m_lstSnakePieces = {
			Point{ WIDTH / 2 + 2, HEIGHT / 2 + 4},
			Point{ WIDTH / 2 + 1, HEIGHT / 2 + 4},
			Point{ WIDTH / 2, HEIGHT / 2 + 4},
			Point{ WIDTH / 2 - 1, HEIGHT / 2 + 4},
			Point{ WIDTH / 2 - 2, HEIGHT / 2 + 4}
		};
	}
	else
	{
		m_lstSnakePieces = {
			Point{ WIDTH / 2 + 2, HEIGHT / 2 },
			Point{ WIDTH / 2 + 1, HEIGHT / 2 },
			Point{ WIDTH / 2, HEIGHT / 2 },
			Point{ WIDTH / 2 - 1, HEIGHT / 2 },
			Point{ WIDTH / 2 - 2, HEIGHT / 2 }
		};
	}
}

// Draw a part of snake
void Snake::drawSnakePart(Point p)
{
	ConsoleFunc::gotoxy(p.x, p.y);
	std::cout << BODY;
}


// Draw full snake
void Snake::show()
{
	for (size_t i = 0; i < m_lstSnakePieces.size(); i++)
	{
		drawSnakePart(m_lstSnakePieces[i]);
	}
}

// move the snake
void Snake::move()
{
	m_objPrevTail = m_lstSnakePieces.back();

	for (size_t i = m_lstSnakePieces.size() - 1; i > 0; i--)
	{
		m_lstSnakePieces[i] = m_lstSnakePieces[i - 1];
	}

	if (m_enmDerection == Direction::up)
		m_lstSnakePieces[0].y -= 1;
	else if (m_enmDerection == Direction::down)
		m_lstSnakePieces[0].y += 1;
	else if (m_enmDerection == Direction::left)
		m_lstSnakePieces[0].x -= 1;
	else if (m_enmDerection == Direction::right)
		m_lstSnakePieces[0].x += 1;
}

// Growing snake when it ate an apple
void Snake::growing()
{
	m_lstSnakePieces.push_back(m_objPrevTail);
}

void Snake::reBorn()
{
	m_enmDerection = Direction::right;

	m_lstSnakePieces = {
		Point{ WIDTH / 2 + 2, HEIGHT / 2 },
		Point{ WIDTH / 2 + 1, HEIGHT / 2 },
		Point{ WIDTH / 2, HEIGHT / 2 },
		Point{ WIDTH / 2 - 1, HEIGHT / 2 },
		Point{ WIDTH / 2 - 2, HEIGHT / 2 }
	};
}

// Check if snake bite itself
bool Snake::isBiteItself()
{
	Point head = m_lstSnakePieces[0];

	for (size_t i = 1; i < m_lstSnakePieces.size(); i++)
	{
		if (head.x == m_lstSnakePieces[i].x && head.y == m_lstSnakePieces[i].y)
		{
			return true;
		}
	}

	return false;
}

// Check if the snake ate apple
bool Snake::isAteApple(Point objApple)
{
	return m_lstSnakePieces[0].x == objApple.x && m_lstSnakePieces[0].y == objApple.y;
}

// Check if the snake hit the wall
bool Snake::isHitWall()
{
	return m_lstSnakePieces[0].x == 0 
		|| m_lstSnakePieces[0].y == 0 
		|| m_lstSnakePieces[0].x == WIDTH 
		|| m_lstSnakePieces[0].y == HEIGHT;
}

Direction Snake::getDirection()
{
	return m_enmDerection;
}

bool Snake::updateDirection(const Direction enmDirection)
{
	m_enmDerection = enmDirection;

	return true;
}

// Redraw head & tail to make the snake move
void Snake::drawHeadnTail()
{
	ConsoleFunc::gotoxy(m_lstSnakePieces[0].x, m_lstSnakePieces[0].y);

	std::cout << BODY;

	ConsoleFunc::gotoxy(m_objPrevTail.x, m_objPrevTail.y);

	std::cout << ' '; // Clear the old tail
}

bool Snake::synchro(const jsoncons::json& objSynchro)
{
	std::lock_guard<std::mutex> lock(m_Mutex);
	try
	{
		auto jsonPreTail = objSynchro.get("PreTail");

		Point objSynchroTail(jsonPreTail.get("x").as_int(), jsonPreTail.get("y").as_int());

		int nDirection = objSynchro.get("Direction").as_int();

		auto lstPoint = objSynchro.get("Point").array_range();
		std::vector<Point> lstNewPoint;
		for (const auto& point: lstPoint)
		{
			Point objNewPoint(point.get("x").as_int(), point.get("y").as_int());
			lstNewPoint.push_back(objNewPoint);
		}

		m_objPrevTail = objSynchroTail;
		m_enmDerection = static_cast<Direction>(nDirection);
		m_lstSnakePieces = lstNewPoint;

		return true;
	}
	catch (std::exception& /*exception*/)
	{
		return false;
	}
}

const Point Snake::getPreTail()
{
	return m_objPrevTail;
}

const std::vector<Point> Snake::getSnake()
{
	return m_lstSnakePieces;
}