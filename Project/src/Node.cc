//Self Message Code

// Start ----> Send First Message at Start time
// 2 ----> Insert Processing Time delay (both sender and receiver)

#include "Node.h"
#include "MyMessage_m.h"
#include <fstream>
#include <string>
#include <bitset>
#include <vector>
#include <iostream>
#include <queue>
using namespace std;


Define_Module(Node);

//vars
bool sender=false;
ifstream myfile;
string line2;
string E, Msg;

void Node::initialize()
{
    // TODO - Generated method body
}

void Node::handleMessage(cMessage *msg)
{
    bool t = false;
    if(msg->isSelfMessage())
    {
        //sender side
        //wait for ack
        string MuxCode(msg->getName());
        if(MuxCode == "Start")
        {
            t = this->ReadMsgFromFile(E, Msg);
            if(t)
            {
                //Perform Framing
                //and check error
                MyMessage_Base* msg3 = new MyMessage_Base();




                msg3->setPayload(Msg.c_str());
                msg3->setFrame_type(0);
                send (msg3,"out");

            }
            else
                EV << "No More Message/s "<<endl;
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
                if(line2[i]==']')
                {
                    break;
                }
                startTime+=line[i];
            }
            EV<<"my start time= "<<stol(startTime)<<endl;

            if(line[1]=='1')
            {
                myfile.open ("D:\\GAM3A\\4- Senior 01\\Computer networks\\github\\Networks_Project\\Project\\node1.txt");
            }
            else
            {
                myfile.open ("D:\\GAM3A\\4- Senior 01\\Computer networks\\github\\Networks_Project\\Project\\node0.txt");
            }

            int startT=stol(startTime);
            scheduleAt(simTime()+startT,new cMessage("Start"));


        }
        else
        {
            MyMessage_Base *mmsg = check_and_cast<MyMessage_Base *>(msg);
            if(mmsg->getFrame_type() == 1 || mmsg->getFrame_type() == 2)
            {
                //check ack
                //protocol send fn
                EV<<"Check timeout" << endl;
                MyMessage_Base* msg3 = new MyMessage_Base();
                E = "";
                Msg = "";
                t = this->ReadMsgFromFile(E, Msg);
                msg3->setPayload(Msg.c_str());
                msg3->setFrame_type(0);
                send (msg3,"out");
            }
            else
            {
                //receiver
                //check expected frame
                //send ack/ nack
                EV <<"Iam Rec" <<endl;
                MyMessage_Base* msg3 = new MyMessage_Base();




                msg3->setPayload("ACK");
                msg3->setFrame_type(1);
                send (msg3,"out");
            }
        }
    }
}


bool Node::ReadMsgFromFile(string &error2, string &Msg2)
{
    string line3;
    if (myfile.is_open())
    {
       if(getline (myfile,line2))
       {
          EV<<line2<<endl;
          for(int i = 0; i < 5; i++)
          {
              error2+=line2[i];
          }

          for(int i = 5; i < line2.size(); i++)
          {
              Msg2+=line2[i];
          }
          EV << "Error Code: " << error2<<endl;
          EV << "Message: " << Msg2 <<endl;

          //check which error code-
          //perform framming here
          return true;
       }
       else
       {
           return false;
       }

   }
    else
    {
        EV <<"file didnt Open" <<endl;
        return false;
    }
}



//sender
//string a(msg->getName());
//if(a=="1")
//{
//    string line;
//    if (myfile.is_open())
//    {
//        while(getline (myfile,line))
//        {
//            EV<<line<<endl;
//            for(int i = 0; i < 5; i++)
//            {
//                error+=line[i];
//            }
//
//            for(int i = 5; i < line.size(); i++)
//            {
//                Msg+=line[i];
//            }
//            EV << "Error Code: " << error<<endl;
//            EV << "Message: " << Msg <<endl;
//
//            //check which error code-
//            //perform framming here
//            cMessage* msg2  = new cMessage(Msg.c_str());
//            send (msg2,"out");
//
//            error = "";
//            Msg = "";
//
//        }
//    myfile.close();
//}
//else
//{
//
//}
//}



