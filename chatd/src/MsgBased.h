#pragma once
#include "UsrBased.h"

struct Message
{
    int msgId;
    string msgFrom;
    string msgTo;
    string msgDate;
    string msgText;
    int msgStatus;
    Message(const string& _msgFrom, const string& _msgTo, const string& _msgText) : msgFrom(_msgFrom), msgTo(_msgTo), msgText(_msgText)
    {
        msgId = 0;
        msgDate = curDateTime();
        msgStatus = 0;
    }
    Message(int id, const string& _msgFrom, const string& _msgTo, const string& _msgDate, const string& _msgText, int _msgStatus) : msgId(id), msgFrom(_msgFrom), msgTo(_msgTo), msgDate(_msgDate), msgText(_msgText), msgStatus(_msgStatus) {}
};

class MsgBase: public Base<Message>
{
private:
    static const int msgAttr{ 7 };
    enum class m_Attr { indId, indFrom, indTo, indDate, indMsg, indStatus, indFor };
    string sepBeg[msgAttr] = { "<id>", "<from>", "<to>", "<date>", "<text>", "<status>", "<for_user>" };
    string sepEnd[msgAttr] = { "</id>", "</from>", "</to>", "</date>", "</text>", "</status>", "</for_user>" };
public:
	MsgBase();
	~MsgBase();
    Message splitMsgPkg(string& msgPkg);
    string packMsgPkg(Message& msgPkg);
    Message getMsg(int id);
    void splitPMStatus(string& str, int arr[]);
    void splitAMStatus(string& str, string arr[]);
    string tagStructParam(m_Attr id, const string& value);
    string unTag(m_Attr id, string& value);
};