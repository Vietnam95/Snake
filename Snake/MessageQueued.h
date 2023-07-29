#pragma once
#include "stdafx.h"
#include "MessageDef.h"

class MessageQueued
{
public:
	MessageQueued();
	virtual ~MessageQueued() = default;

	bool registerMessage(const jsoncons::json& objMsg);

	bool registerMessage(const MessageId enmMsgId, const std::vector<char>& charMsg);

	std::deque<jsoncons::json> queueJsonMsgHandling();

	std::deque<std::pair<MessageId, std::vector<char>>> queueBinaryMsgHandling();

	bool clearQueue();

private:
	std::deque<jsoncons::json> m_lstJsonMessage;

	std::deque<std::pair<MessageId, std::vector<char>>> m_lstBinaryMessage;
};
