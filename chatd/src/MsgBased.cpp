#include "MsgBased.h"

MsgBase::MsgBase(){}
MsgBase::~MsgBase(){}

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

Message MsgBase::getMsg(int id)
{
	Message temp = base->at(id);
	return temp;
}

void MsgBase::splitPMStatus(string& str, int arr[])
{
	arr[0] = stoi(unTag(m_Attr::indId, str));
	arr[1] = stoi(unTag(m_Attr::indStatus, str));
}

void MsgBase::splitAMStatus(string& str, string arr[])
{
	arr[0] = unTag(m_Attr::indId, str);
	arr[1] = unTag(m_Attr::indFor, str);
	arr[2] = unTag(m_Attr::indStatus, str);
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