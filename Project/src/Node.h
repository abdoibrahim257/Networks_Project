//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef __PROJECT_NODE_H_
#define __PROJECT_NODE_H_
#include "MyMessage_m.h"
#include <omnetpp.h>
#include <string>
#include <queue>
using namespace std;
using namespace omnetpp;

/**
 * TODO - Generated class
 */
class Node : public cSimpleModule
{
  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    bool Between(int seq_a, int seq_b, int seq_c);
    void inc(int &seq_num, int Max);
    string Framing(string msg);
    void SendMsg();
    string ReadMsgFromFile(std::string &error, std::string &Msg);
    bool isContains(queue<MyMessage_Base *> q, int x);
    bool calculateParity(MyMessage_Base*&msg);
    bool checkParity(MyMessage_Base*&msg);
    MyMessage_Base * copyMessage(MyMessage_Base*msg);
};

#endif
