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

#include "Node.h"
#include "MyMessage_m.h"
#include <fstream>
#include <string>
#include <bitset>
#include <vector>
#include <iostream>

using namespace std;
Define_Module(Node);

//vars
bool sender=false;

void Node::initialize()
{
    // TODO - Generated method body
}

void Node::handleMessage(cMessage *msg)
{
    string error, Msg;
    if(msg->isSelfMessage())
    {
        string a(msg->getName());
        if(a=="1")
        {
            string line;
            ifstream myfile ("D:\\GAM3A\\4- Senior 01\\Computer networks\\github\\Networks_Project\\Project\\node1.txt");
            if (myfile.is_open())
            {
                getline (myfile,line);
                EV<<line<<endl;
                for(int i = 0; i < 5; i++)
                {
                    error+=line[i];
                }

                for(int i = 5; i < line.size(); i++)
                {
                    Msg+=line[i];
                }
                EV << "Error Code: " << error<<endl;
                EV << "Message: " << Msg <<endl;

                //check which error code-
                //perform framming here
                //Add processing Delay

                cMessage* msg2  = new cMessage(Msg.c_str());
                send (msg2,"out");

                error = "";
                Msg = "";
            }
            myfile.close();
        }
        else
        {
            string line;
            ifstream myfile ("D:\\GAM3A\\4- Senior 01\\Computer networks\\github\\Networks_Project\\Project\\node1.txt");
            if (myfile.is_open())
            {
                getline (myfile,line);
                while(getline (myfile,line))
                {
                    EV<<line<<endl;
                    for(int i = 0; i < 5; i++)
                    {
                        error+=line[i];
                    }

                    for(int i = 5; i < line.size(); i++)
                    {
                        Msg+=line[i];
                    }
                    EV << "Error Code: " << error<<endl;
                    EV << "Message: " << Msg <<endl;

                    //check which error code-
                    //perform framming here
                    cMessage* msg2  = new cMessage(Msg.c_str());
                    send (msg2,"out");

                    error = "";
                    Msg = "";

                }
                myfile.close();
            }
        //sender side
        //wait for ack
        }
    }
    else
    {
        cGate *arrivalGate=msg->getArrivalGate();
        if(arrivalGate==gate("in",0))
        {
            sender=true;
            EV<<"i am sender "<<sender<<endl;
            string line(msg->getName());
            string startTime;
            for(int i=3;i<line.size();i++)
            {
                if(line[i]==']')
                {
                    break;
                }
                startTime+=line[i];
            }
            EV<<"my start time= "<<stol(startTime)<<endl;
            int startT=stol(startTime);
            scheduleAt(simTime()+startT,new cMessage("1"));
        }
        else
        {
            if(sender)
            {
                //check ack
                //protocol send fn
            }
            else
            {
                //receiver
                //check expected frame
                //send ack/ nack
            }
        }
    }
}




