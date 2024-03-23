#include "MsgBase.h"

MsgBase::MsgBase() {}

MsgBase::~MsgBase() {}

void MsgBase::getAllMsgBase(const string& uuid)
{
	base->clear();
	vector<string>* servBaseMsg = new vector<string>;
	Online->getAllMsgBase(servBaseMsg, uuid);
	for (string msgStr : *servBaseMsg)
	{
		Message newMsg = splitMsgPkg(msgStr);
		addInBase(newMsg);
	}
	delete servBaseMsg;
}

void MsgBase::getPerMsgBase(const string& uuid)
{
	base->clear();
	vector<string>* servBaseMsg = new vector<string>;
	Online->getPerMsgBase(servBaseMsg, uuid);
	for (string msgStr : *servBaseMsg)
	{
		Message newMsg = splitMsgPkg(msgStr);
		addInBase(newMsg);
	}
	delete servBaseMsg;
}

Message MsgBase::splitMsgPkg(string& msgPkg)
{
	int id, status;
	string from, to, date, msg;
	id = stoi(unTag(m_Attr::indId, msgPkg));
	from = unTag(m_Attr::indFrom, msgPkg);
	to = unTag(m_Attr::indTo, msgPkg);
	date = unTag(m_Attr::indDate, msgPkg);
	msg = unTag(m_Attr::indMsg, msgPkg);
	status = stoi(unTag(m_Attr::indStatus, msgPkg));

	Message newMsg(id, from, to, date, msg, status);
	return newMsg;
}

string MsgBase::packMsgPkg(Message& msgPkg)
{
	string result = tagStructParam(m_Attr::indId, std::to_string(msgPkg.msgId));
	result += tagStructParam(m_Attr::indFrom, msgPkg.msgFrom);
	result += tagStructParam(m_Attr::indTo, msgPkg.msgTo);
	result += tagStructParam(m_Attr::indDate, msgPkg.msgDate);
	result += tagStructParam(m_Attr::indMsg, msgPkg.msgText);
	result += tagStructParam(m_Attr::indStatus, std::to_string(msgPkg.msgStatus));
	return result;
}

void MsgBase::printPers(const string& uuidFrom, const string& uuidTo)
{
	if (getCount() == 0)
	{
		printEmpty();
		return;
	}
	vector<int> msgComp;
	getMsgComp(msgComp, uuidFrom, uuidTo);
	printMsgComp(msgComp);
}

void MsgBase::printMain()
{
	if (getCount() == 0)
	{
		printEmpty();
		return;
	}
	vector<int> msgComp;
	getMsgComp(msgComp, "ALL");
	printMsgComp(msgComp);
}

void MsgBase::printEmpty()
{
	cout << "Пока здесь еще никто ничего не писал" << endl;
}

void MsgBase::getMsgComp(vector<int>& msgComp, const string& uuidTo)
{
	for (int i = 0; i < base->size(); i++)
	{
		if (base->at(i).msgTo == uuidTo)
		{
			msgComp.push_back(i);
		}
	}
}

void MsgBase::getMsgComp(vector<int>& msgComp, const string& uuidFrom, const string& uuidTo)
{
	for (int i = 0; i < base->size(); i++)
	{
		if (base->at(i).msgFrom == uuidFrom && base->at(i).msgTo == uuidTo || base->at(i).msgFrom == uuidTo && base->at(i).msgTo == uuidFrom)
		{
			msgComp.push_back(i);
		}
	}
}

void MsgBase::printMsgComp(vector<int>& msgComp)
{
	if (msgComp.empty())
	{
		printEmpty();
		return;
	}
	else
	{
		for (int i = 0; i < msgComp.size(); i++)
		{
			printMsgStruct(base->at(msgComp[i]));
			if (base->at(msgComp[i]).msgStatus == 0 && base->at(msgComp[i]).msgTo == "ALL")
			{
				string packet = tagStructParam(m_Attr::indId, std::to_string(base->at(msgComp[i]).msgId)) + tagStructParam(m_Attr::indFor, userUUID) + tagStructParam(m_Attr::indStatus, "1");
				Online->setAllMsgStatus(packet);
				base->at(msgComp[i]).msgStatus = 1;
			}
			if (base->at(msgComp[i]).msgStatus == 0 && base->at(msgComp[i]).msgTo == userUUID)
			{
				string packet = tagStructParam(m_Attr::indId, std::to_string(base->at(msgComp[i]).msgId)) + tagStructParam(m_Attr::indStatus, "1");
				Online->setPerMsgStatus(packet);
				base->at(msgComp[i]).msgStatus = 1;
			}
		}
	}
}

void MsgBase::printMsgStruct(Message& mesg)
{
	string border(80, '-');
	size_t count = 80 - mesg.msgFrom.length() - mesg.msgDate.length() -2;

	printf("[%s | %s]\n\n%s\n%s\n", mesg.msgDate.data(), UserBase->getUsrName(mesg.msgFrom).data(), mesg.msgText.data(), border.data());
}

void MsgBase::sendMsg(const string& msgFrom, const string& msgTo, const string& msg)
{
	Message newMsg(msgFrom, msgTo, msg);
	string req = "SND_MSG<|>";
	req.append(packMsgPkg(newMsg));
	Online->sendRequest(req);
	if (msgTo == "ALL")
	{
		getAllMsgBase(msgFrom);
	}
	else
	{
		getPerMsgBase(msgFrom);
	}
}

int MsgBase::countUnreadPM()
{
	int result = 0;
	for (Message msg: *base)
	{
		if (msg.msgTo == userUUID && msg.msgStatus == 0)
		{
			result++;
		}
	}
	return result;
}

int MsgBase::countUnreadPM(const string& from)
{
	int result = 0;
	for (Message msg : *base)
	{
		if (msg.msgTo == userUUID && msg.msgFrom == from && msg.msgStatus == 0)
		{
			result++;
		}
	}
	return result;
}

int MsgBase::countUnreadAM()
{
	int result = 0;
	for (Message msg : *base)
	{
		if (msg.msgTo == "ALL" && msg.msgStatus == 0)
		{
			++result;
		}
	}
	return result;
}

string MsgBase::tagStructParam(m_Attr id, const string& value)
{
	string result = sepBeg[static_cast<int>(id)];
	result += value;
	result += sepEnd[static_cast<int>(id)];
	return result;
}

string MsgBase::unTag(m_Attr id, string& str)
{
	string result;
	result = str.substr(str.find(sepBeg[static_cast<int>(id)]) + sepBeg[static_cast<int>(id)].length(), str.find(sepEnd[static_cast<int>(id)]) - sepBeg[static_cast<int>(id)].length());
	str.erase(0, str.find(sepEnd[static_cast<int>(id)]) + sepEnd[static_cast<int>(id)].length());
	return result;
}