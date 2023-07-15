#pragma once
#include "stdafx.h"

class MessageQueued
{
public:
	MessageQueued();
	virtual ~MessageQueued() = default;

	bool registerMessage(const jsoncons::json& objMsg);

	std::deque<jsoncons::json> queueMsgHandling();

	bool clearQueue();

private:
	std::deque<jsoncons::json> m_lstMessage;
};
