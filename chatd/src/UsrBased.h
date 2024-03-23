#pragma once
#include "TmplBased.h"

class SQL_DB;
extern SQL_DB* db_connect;

struct User
{
    string uuid;
    string name;
    string login;
    string pwd;
    int deleted;
    User(const string& newName, const string& newLogin, const string& newPwd) : name(newName), login(newLogin), pwd(newPwd) { uuid = "-"; deleted = 0; }
    User(const string& newUUID, const string& newName, const string& newLogin, const string& newPwd, int status) : uuid(newUUID), name(newName), login(newLogin), pwd(newPwd), deleted(status) {}
};

class UsrBase: public Base<User>
{
private:
    static const int usrAttr{ 5 };
    enum class m_Attr { indUUID, indName, indLogin, indPwd, indDel };
    string sepBeg[usrAttr]{ "<uuid>", "<name>", "<login>", "<pwd>", "<deleted>" };
    string sepEnd[usrAttr] = { "</uuid>", "</name>", "</login>", "</pwd>", "</deleted>" };
    SHA256 sha256;
public:
	UsrBase();
	~UsrBase();
    User splitUsrPkg(string& usrPkg);
    User getUser(int id);
    User getUser(const string& uuid);
    string packUsrPkg(User& account);
    void splitChgPwd(string& str, string arr[]);
    string tagStructParam(m_Attr id, string value);
    string unTag(m_Attr id, string& str);
};