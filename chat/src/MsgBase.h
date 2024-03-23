#pragma once
#include "UsrBase.h"

extern net* Online;
extern UsrBase* UserBase;
extern string curDateTime();
extern string userUUID;

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
public:
	MsgBase();
	~MsgBase();
	void getAllMsgBase(const string& uuid);
    void getPerMsgBase(const string& uuid);
    Message splitMsgPkg(string& msgPkg);
    string packMsgPkg(Message& msgPkg);
    void printPers(const string& uuidFrom, const string& uuidTo);
    void printMain();
    void printEmpty();
    void getMsgComp(vector<int>& msgComp, const string& msgTo = "ALL");
    void getMsgComp(vector<int>& msgComp, const string& uuidFrom, const string& uuidTo);
    void printMsgComp(vector<int>& msgComp);
	void printMsgStruct(Message& mesg);
    void sendMsg(const string& msgTo, const string& msgFrom, const string& msg);
    int countUnreadPM();
    int countUnreadPM(const string& from);
    int countUnreadAM();
private:
    static const int msgAttr{ 7 };
    enum class m_Attr {indId, indFrom, indTo, indDate, indMsg, indStatus, indFor};
    string sepBeg[msgAttr] = { "<id>", "<from>", "<to>", "<date>", "<text>", "<status>", "<for_user>" };
    string sepEnd[msgAttr] = { "</id>", "</from>", "</to>", "</date>", "</text>", "</status>", "</for_user>" };
public:
    string tagStructParam(m_Attr id, const string& value);
    string unTag(m_Attr id, string& str);
};