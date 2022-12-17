//Self Message Code

// Start ----> Send First Message at Start time
// Transmission ----> Insert Transmission delay Time delay

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

int MaxSeqNum;
int Next_frame_to_send;
int Ack_expected;
int Frame_expected;
vector<MyMessage_Base *> buffer;
int nBuffered;
int i;
int AcceptedDelay;

void Node::initialize()
{
    // TODO - Generated method body
    MaxSeqNum = par("WS").intValue()-1;
    Next_frame_to_send = 0;
    Ack_expected = 0;
    Frame_expected = 0;
    nBuffered = 0;
    AcceptedDelay = 3;
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
            //insert this clock on for loop until buffer is full or data is completed
            if(nBuffered < par("WS").intValue())
            {
                t = this->ReadMsgFromFile(E, Msg);
                if(t)
                {
                    MyMessage_Base* msg3 = new MyMessage_Base("Transmission");

                    //construct
                    string FramedMsg = Framing(Msg);
                    msg3->setPayload(FramedMsg.c_str());
                    msg3->setHeaderSeq_num(Next_frame_to_send);
                    msg3->setFrame_type(0);
                    //parity
                    //insert trailer

                    nBuffered++;
                    EV<<"nBuffered: " << nBuffered<<endl;
                    buffer.push_back(msg3);


                    scheduleAt(simTime() + par("PT").doubleValue(), msg3);
                    inc(Next_frame_to_send, MaxSeqNum);
                    if(nBuffered < par("WS").intValue())
                    {
                        scheduleAt(simTime()+ par("PT").doubleValue(), new cMessage("Start"));
                    }
                    //time delay part
                    Msg = "";
                    E = "";
                }
                else
                {
                    EV << "No More Message/s "<<endl;
                }
            }
        }
        else if(MuxCode == "Transmission")
        {
            sendDelayed(msg, par("TD").doubleValue(), "out");
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
//                myfile.open ("D:\\Uni\\Senior 1\\Semester 1\\Networks\\Project_test\\node1.txt");
                myfile.open ("D:\\GAM3A\\4- Senior 01\\Computer networks\\github\\Networks_Project\\Project\\node1.txt");
            }
            else
            {
//                myfile.open ("D:\\Uni\\Senior 1\\Semester 1\\Networks\\Project_test\\node1.txt");
                myfile.open ("D:\\GAM3A\\4- Senior 01\\Computer networks\\github\\Networks_Project\\Project\\node1.txt");
            }

            int startT=stol(startTime);
            scheduleAt(simTime()+startT,new cMessage("Start"));
        }
        else
        {
            MyMessage_Base *mmsg = check_and_cast<MyMessage_Base *>(msg);
            if(mmsg->getFrame_type() == 1 || mmsg->getFrame_type() == 2)
            {

            }
            else if(mmsg->getFrame_type() == 0)
            {
                //receiver
                //check expected frame
                //send ack/ nack
                EV <<"Iam Rec" <<endl;
                if(mmsg->getHeaderSeq_num() == Frame_expected)
                {
                    //check parity
                    //if true send ack
                    //inc frame expected
                    //else Nack
                    //dont inc frame expected
                }

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

void Node::SendMsg()
{

}

string Node::Framing(string Msg)
{
    vector<char> Framed_Msg;
    Framed_Msg.push_back('$');
    for (int  i=0;i<Msg.size();i++)
    {
        if (Msg[i]== '/' || Msg[i]=='$')
        {
            Framed_Msg.push_back('/');
        }
        Framed_Msg.push_back(Msg[i]);
    }
    Framed_Msg.push_back('$');

    string framedStr="";
    for (int i=0;i<Framed_Msg.size();i++)
    {
        framedStr += Framed_Msg[i];
    }

    return framedStr;
}

void Node::inc(int &seq_num, int Max)
{
    (seq_num <=Max) ? seq_num++ : seq_num = 0;
}

bool Node::Between(int seq_a, int seq_b, int seq_c)
{
    if(((seq_a <= seq_b) && (seq_b < seq_c)) || ((seq_c < seq_a) && (seq_a <= seq_b)) || ((seq_b < seq_c) && (seq_c < seq_a)))
        return true;
    else
        return false;
}


//check ack
                //protocol send fn
//                EV<<"Check timeout" << endl;
//                MyMessage_Base* msg3 = new MyMessage_Base();
//                E = "";
//                Msg = "";
//                t = this->ReadMsgFromFile(E, Msg);
//                msg3->setPayload(Msg.c_str());
//                msg3->setFrame_type(0);
//                send (msg3,"out");

