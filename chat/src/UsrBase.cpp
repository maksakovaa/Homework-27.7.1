#include "UsrBase.h"

UsrBase::UsrBase()
{
    getUsrBase();
}

UsrBase::~UsrBase()
{

}

void UsrBase::getUsrBase()
{
    base->clear();
    vector<string>* servBase = new vector<string>;
    Online->getUsrBase(servBase);
    for (string perStr: *servBase)
    {
        User newUser = splitUsrPkg(perStr);
        addInBase(newUser);
    }
    delete servBase;
}

void UsrBase::printUsrBase()
{
    printf("%-*s %-*s %-*s \n", 5, "ID", 12, "Логин", 32, "Имя пользователя");
    for (int i = 0; i < base->size(); i++)
    {
        if (base->at(i).uuid == userUUID)
        {
            continue;
        }
        if (base->at(i).deleted == 1)
        {
            continue;
        }
        printf("%-*i %-*s %-*s\n", 5, i, 12, base->at(i).login.data(), 32, base->at(i).name.data());
    }
}

User UsrBase::splitUsrPkg(string& usrPkg)
{
    string uuid, name, login, pwdHash;
    int deleted;
    uuid = unTag(m_Attr::indUUID, usrPkg);
    name = unTag(m_Attr::indName, usrPkg);
    login = unTag(m_Attr::indLogin, usrPkg);
    pwdHash = unTag(m_Attr::indPwd, usrPkg);
    deleted = stoi(unTag(m_Attr::indDel, usrPkg));
    User newUser(uuid, name, login, pwdHash, deleted);
    return newUser;
}

string UsrBase::packUsrPkg(const User& account)
{
    string result = tagStructParam(m_Attr::indName, account.name);
    result += tagStructParam(m_Attr::indLogin, account.login);
    result += tagStructParam(m_Attr::indPwd, account.pwd);
    return result;
}

string UsrBase::getUsrName(int id)
{
    return base->at(id).name;
}

string UsrBase::getUsrName(string& uuid)
{
    for (User account: *base)
    {
        if (account.uuid == uuid)
        {
            return account.name;
        }
    }
    return "deleted";
}

string UsrBase::getUsrUUID(int id)
{
    return base->at(id).uuid;
}

string UsrBase::getUsrUUID(const string& login)
{
    for (User account: *base)
    {
        if (account.login == login)
        {
            return account.uuid;
        }
    }
    return {};
}

string UsrBase::getUsrLogin(int id)
{
    return base->at(id).login;
}

string UsrBase::getUsrLogin(const string& uuid)
{
    for (User account: *base)
    {
        if (account.uuid == uuid)
        {
            return account.login;
        }
    }
    return "";
}

int UsrBase::getUsrId()
{
    for (int i = 0; i < (int)base->size(); i++)
    {
        if (base->at(i).uuid == userUUID)
        {
            return i;
        }
    }
    return -1;
}

int UsrBase::getUsrId(string& login)
{
    for (int i = 0; i < (int)base->size(); i++)
    {
        if (base->at(i).login == login)
        {
            return i;
        }
    }
    return -1;
}

void UsrBase::setUsrPwd(int id, string& newPwd)
{
    string usrPkg = tagStructParam(m_Attr::indUUID, base->at(id).uuid);
    usrPkg += tagStructParam(m_Attr::indPwd, sha256(newPwd));
    Online->sendRequest(usrPkg);
    getUsrBase();
}

void UsrBase::delUsr(int id)
{
    Online->delUser(base->at(id).uuid);
    getUsrBase();
}

void UsrBase::regUsr(string& name, string& login, string& pwd)
{
    User newUser(name, login, sha256(pwd));
    string usrPkg = packUsrPkg(newUser);
    Online->regUser(usrPkg);
    getUsrBase();
}

bool UsrBase::logInChk(const string& login)
{
    for (User person: *base)
    {
        if (person.login == login && person.deleted == 0)
        {
            if (person.deleted == 0)
            {
                return true;
            }
            else
            {
                return false;
            }

        }
     }
    return false;
}

bool UsrBase::pwdChk(int id, const string& pwd)
{
    string temp = sha256(pwd);
    if (base->at(id).pwd == temp)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool UsrBase::isDeleted(int id)
{
    if (base->at(id).deleted == 1)
    {
        return true;
    }
    else
    {
        return false;
    }
    
}

string UsrBase::tagStructParam(m_Attr id, string value)
{
    string result = sepBeg[static_cast<int>(id)];
    result += value;
    result += sepEnd[static_cast<int>(id)];
    return result;
}

string UsrBase::unTag(m_Attr id, string& str)
{
    string result;
    result = str.substr(str.find(sepBeg[static_cast<int>(id)]) + sepBeg[static_cast<int>(id)].length(), str.find(sepEnd[static_cast<int>(id)]) - sepBeg[static_cast<int>(id)].length());
    str.erase(0, str.find(sepEnd[static_cast<int>(id)]) + sepEnd[static_cast<int>(id)].length());
    return result;
}
