#include "netd.h"
Logger* logNewMsg = new Logger;
Settings* Config = new Settings;
UsrBase* UserBase = new UsrBase;
MsgBase* AllMsgBase = new MsgBase;
MsgBase* PerMsgBase = new MsgBase;
SQL_DB* db_connect = new SQL_DB;

int main()
{
#if defined (_WIN32) || defined (_WIN64)
    SetConsoleOutputCP(65001);
    SetConsoleCP(1251);
#elif defined (__linux__)
    setlocale(LC_ALL, "ru_RU.utf8");
#endif
   
    net* online = new net;
    online->transmission();
    delete online, db_connect, AllMsgBase, UserBase, Config;
    return 0;
}