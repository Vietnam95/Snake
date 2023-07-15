#pragma once
#include "stdafx.h"

#include "SystemCommon.h"
#include "Common.h"

class Snake
{
public:

	Snake(const ControlLogicType& enmType);

	// Create snake
	void init(const ControlLogicType& enmType);

	void drawSnakePart(Point);
	void show();
	// move the snake
	void move();

	// Growing snake when it ate an apple
	void growing();

	void reBorn();

	// Check if snake bite itself
	bool isBiteItself();

	// Check if the snake ate apple
	bool isAteApple(Point objApple);

	// Check if the snake hit the wall
	bool isHitWall();

	Direction getDirection();

	bool updateDirection(const Direction enmDirection);

	// Redraw head & tail to make the snake move
	void drawHeadnTail();

	bool synchro(const jsoncons::json& objSynchro);

	const Point getPreTail();
	const std::vector<Point> getSnake();
protected:

	std::vector<Point> m_lstSnakePieces;
	Point m_objPrevTail;
	Direction m_enmDerection;
	std::mutex m_Mutex;
};
