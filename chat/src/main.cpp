#include "navigation.h"

net* Online = new net;
UsrBase* UserBase = new UsrBase;
MsgBase* AllMsgBase = new MsgBase;
MsgBase* PerMsgBase = new MsgBase;
string userUUID;
int choice;
bool auth;

int main()
{
    menu();
    delete AllMsgBase, UserBase, Online;
    return 0;
}