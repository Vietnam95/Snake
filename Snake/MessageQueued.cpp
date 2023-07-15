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
			m_lstMessage.push_back(objMsg);
			return true;
		}
	}
	catch(std::exception& /*exception*/)
	{
		return false;
	}
}

std::deque<jsoncons::json> MessageQueued::queueMsgHandling()
{
	return m_lstMessage;
}


bool MessageQueued::clearQueue()
{
	m_lstMessage.clear();
	return true;
}