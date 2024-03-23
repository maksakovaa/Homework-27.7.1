#pragma once
#include "TmplBase.h"

extern bool auth;
extern string userUUID;
extern net* Online;

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
    static const int usrAttr = { 5 };
    enum class m_Attr { indUUID, indName, indLogin, indPwd, indDel };
    string sepBeg[usrAttr]{ "<uuid>", "<name>", "<login>", "<pwd>", "<deleted>"};
    string sepEnd[usrAttr] = { "</uuid>", "</name>", "</login>", "</pwd>", "</deleted>"};
    SHA256 sha256;
public:
    UsrBase();
    ~UsrBase();
    void getUsrBase();
    void printUsrBase();
    User splitUsrPkg(string& usrPkg);
    string packUsrPkg(const User& account);
    string getUsrName(int id);
    string getUsrName(string& uuid);
    int getUsrId();
    int getUsrId(string& login);
    string getUsrUUID(int id);
    string getUsrUUID(const string& login);
    string getUsrLogin(int id);
    string getUsrLogin(const string& uuid);
    void setUsrPwd(int id, string& newPwd);
    void delUsr(int id);
    void regUsr(string& name, string& login, string& pwd);
    bool logInChk(const string& login);
    bool pwdChk(int id, const string& pwd);
    bool isDeleted(int id);
    string tagStructParam(m_Attr id, string value);
    string unTag(m_Attr id, string& str);
};