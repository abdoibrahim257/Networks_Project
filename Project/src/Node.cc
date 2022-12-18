//Self Message Code

// Start ----> Send First Message at Start time
// Transmission ----> Insert Transmission delay Time delay
// StopTimer  ----> start timer for each message
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
//vector<MyMessage_Base *> buffer;
queue<MyMessage_Base *> buffer;
int nBuffered;
int i;
int AcceptedDelay;

void Node::initialize()
{
    // TODO - Generated method body
    MaxSeqNum = par("WS").intValue()-1;
    Next_frame_to_send = 0;
    Ack_expected = 1;
    Frame_expected = 0;
    nBuffered = 0;
    AcceptedDelay = 3;
    i;
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
            if(nBuffered <= MaxSeqNum)
            {
                t = this->ReadMsgFromFile(E, Msg);
                if(t)
                {
                    MyMessage_Base* msg3 = new MyMessage_Base("Transmission");

                    //construct
                    string FramedMsg = Framing(Msg);
                    msg3->setPayload(FramedMsg.c_str());
                    msg3->setHeaderSeq_num(Next_frame_to_send);
                    msg3->setAck_Nack_num(Ack_expected);
                    msg3->setFrame_type(0);
                    this->calculateParity(msg3);

                    nBuffered++;
                    EV<<"nBuffered: " << nBuffered<<endl;
                    buffer.push(msg3);
                    inc(Next_frame_to_send, MaxSeqNum);

                    scheduleAt(simTime() + par("PT").doubleValue(), msg3);

                    MyMessage_Base* msg4 = new MyMessage_Base("StopTimer");
                    msg4->setHeaderSeq_num(msg3->getHeaderSeq_num());

                    //EV << "simTime: " << simTime();
                    scheduleAt(simTime() + par("PT").doubleValue() + par("TO").doubleValue(), msg4); //send message to myself and check if message is in the queue and timesout

                    if(nBuffered <= MaxSeqNum)
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
        else if(MuxCode == "StopTimer")
        {
            //end Time aka timeout
            //check if msg is in the q if yes then timeout and retransmit
            //else already sent and received the ack --->ignore

            MyMessage_Base *mmsg = check_and_cast<MyMessage_Base *>(msg);
            MyMessage_Base *Temp_mmsg = buffer.front();
            //EV << "First element in Buffer: " << Temp_mmsg->getHeaderSeq_num() << endl;
            if(Temp_mmsg->getHeaderSeq_num() == mmsg->getHeaderSeq_num())
            {
                //timeout
                EV<<"Timed out message " << mmsg->getHeaderSeq_num() << endl;
                Next_frame_to_send = Ack_expected;
                for(i = 1 ; i <= nBuffered ; i++)
                {

                    MyMessage_Base* msg2be_Resent = new MyMessage_Base("Transmission");
                    msg2be_Resent = copyMessage(buffer.front());


                    EV << "Retransmit: " << msg2be_Resent->getHeaderSeq_num() << endl;
                    buffer.push(buffer.front());
                    buffer.pop();
                    scheduleAt(simTime() + i*par("PT").doubleValue(), msg2be_Resent); //====> ask? when retransmit do i need to add PT???
                    inc(Next_frame_to_send, MaxSeqNum);
                }
                EV <<"End of frame: " << Next_frame_to_send << endl;
            }
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
//                myfile.open ("D:\\CUFE\\Fall 2022\\Networks\\project\\Networks_Project\\Project\\node1.txt");
            }
            else
            {
//                myfile.open ("D:\\Uni\\Senior 1\\Semester 1\\Networks\\Project_test\\node0.txt");
                myfile.open ("D:\\GAM3A\\4- Senior 01\\Computer networks\\github\\Networks_Project\\Project\\node0.txt");
//                myfile.open ("D:\\CUFE\\Fall 2022\\Networks\\project\\Networks_Project\\Project\\node0.txt");
            }

            int startT=stol(startTime);
            scheduleAt(simTime()+startT,new cMessage("Start"));
        }
        else
        {
            MyMessage_Base *mmsg = check_and_cast<MyMessage_Base *>(msg);
            if(mmsg->getFrame_type() == 1)
            {
                EV<< "Advance Window NOW " << endl;
                EV<<"ACK expected before: " <<Ack_expected<<endl;
                EV<<"Ack Received: "<<mmsg->getAck_Nack_num()<<endl;
                EV<<"Next frame to send: "<< Next_frame_to_send<<endl;
                if(Ack_expected ==  mmsg->getAck_Nack_num())
                    {
                        nBuffered--;
                        if(!buffer.empty())
                            buffer.pop();
                        inc(Ack_expected, MaxSeqNum);
                        EV<<"ACK expected after: " <<Ack_expected<<endl;
                    }
                //advances the lower end and frees the buffer next is to send any message if there exist in file
                EV << "Begining of the buffer: " << buffer.front()->getHeaderSeq_num() << endl;

                //now i can send more messages
                scheduleAt(simTime() ,new cMessage("Start"));


            }
            else if(mmsg->getFrame_type() == 0)
            {
                MyMessage_Base* msg3 = new MyMessage_Base("Transmission");
                //receiver
                //check expected frame
                //send ack/ nack
                EV <<"Iam Rec" <<endl;
                if(mmsg->getHeaderSeq_num() == Frame_expected)
                {
                    bool correct = calculateParity(mmsg);
                    if(correct)
                    {
                        //send ack
                        msg3->setPayload("Ack");
                        msg3->setFrame_type(1);
                        inc(Frame_expected, MaxSeqNum);
                    }
                    else
                    {
                        //Nack
                        msg3->setPayload("Nack");
                        msg3->setFrame_type(2);
                    }
                }
                msg3->setAck_Nack_num(Frame_expected);
                scheduleAt(simTime() + par("PT").doubleValue(), msg3);
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
    (seq_num < Max) ? seq_num++ : seq_num = 0;
}

bool Node::Between(int seq_a, int seq_b, int seq_c)
{
    if(((seq_a <= seq_b) && (seq_b < seq_c)) || ((seq_c < seq_a) && (seq_a <= seq_b)) || ((seq_b < seq_c) && (seq_c < seq_a)))
        return true;
    else
        return false;
}

bool Node::isContains(queue<MyMessage_Base *> q, int x)
{
    while(!q.empty())
    {
        if(q.front()->getHeaderSeq_num() == x)
            return true;
        q.pop();
    }
    return false;
}


bool Node::calculateParity(MyMessage_Base*&msg)
{
        bitset<8> answer_parity (0);
        string payload=msg->getPayload();
        for(int i=0;i<payload.size();i++)
        {
            bitset<8> xbits (payload[i]);
            answer_parity^=xbits;
        }
        bitset<8> seqnobits( msg->getHeaderSeq_num());
        answer_parity^=seqnobits;

        bitset<8> ackNackbits(msg->getAck_Nack_num());
        answer_parity^=ackNackbits;

        bitset<8> frameTypebits(msg->getFrame_type());
        answer_parity^=frameTypebits;
        if(!sender)
        {
            bitset<8> parityBits(msg->getTrailer_parity());
            answer_parity^=parityBits;
            //EV<<"i entered heree therfore i am receiver"<<endl;
            if(answer_parity == 00000000)
            {
                return true;//correct even parity -> correct message
            }
            else
            {
                return false;
            }

        }
    //EV<<"HII  "<<(char)answer_parity.to_ulong()<<endl;
    msg->setTrailer_parity((char)answer_parity.to_ulong());
    return true;
}

MyMessage_Base * Node::copyMessage(MyMessage_Base*msg)
{
    //set name manually
    MyMessage_Base* msg2be_Resent = new MyMessage_Base();

    msg2be_Resent->setHeaderSeq_num(msg->getHeaderSeq_num());
    msg2be_Resent->setPayload(msg->getPayload());
    msg2be_Resent->setAck_Nack_num(msg->getAck_Nack_num());
    msg2be_Resent->setFrame_type(msg->getFrame_type());
    msg2be_Resent->setTrailer_parity(msg->getTrailer_parity());

    return msg2be_Resent;
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


//retransmission copying message block
//                    msg2be_Resent->setHeaderSeq_num(buffer.front()->getHeaderSeq_num());
//                    msg2be_Resent->setPayload(buffer.front()->getPayload());
//                    msg2be_Resent->setAck_Nack_num(buffer.front()->getAck_Nack_num());
//                    msg2be_Resent->setFrame_type(buffer.front()->getFrame_type());
//                    msg2be_Resent->setTrailer_parity(buffer.front()->getTrailer_parity());
