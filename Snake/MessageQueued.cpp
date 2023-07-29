#pragma once
#include "MessageQueued.h"

MessageQueued::MessageQueued()
{
}

bool MessageQueued::registerMessage(const jsoncons::json& objMsg)
{
	try
	{
		std::string strCheckValid = objMsg.get("messageid").to_string();
		if (!strCheckValid.empty())
		{
			m_lstJsonMessage.push_back(objMsg);
			return true;
		}

		return false;
	}
	catch(std::exception& /*exception*/)
	{
		return false;
	}
}

bool MessageQueued::registerMessage(const MessageId enmMsgId, const std::vector<char>& charMsg)
{
	try
	{
		if (!charMsg.empty())
		{
			m_lstBinaryMessage.push_back(std::make_pair(enmMsgId, charMsg));
			return true;
		}

		return false;
	}
	catch (std::exception& /*exception*/)
	{
		return false;
	}
}

std::deque<jsoncons::json> MessageQueued::queueJsonMsgHandling()
{
	return m_lstJsonMessage;
}

std::deque<std::pair<MessageId, std::vector<char>>> MessageQueued::queueBinaryMsgHandling()
{
	return m_lstBinaryMessage;
}


bool MessageQueued::clearQueue()
{
	m_lstJsonMessage.clear();
	m_lstBinaryMessage.clear();

	return true;
}