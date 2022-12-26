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
char node_id='0';    //node id '1' or '0'

int MaxSeqNum;
int Next_frame_to_send;
int Ack_expected;
int Frame_expected;
//vector<MyMessage_Base *> buffer;
queue<MyMessage_Base *> buffer;
int nBuffered;
int i;
int AcceptedDelay;
ofstream sender_output;
ofstream receiver_output;

//  myfile.close();




void Node::initialize()
{
    // TODO - Generated method body
    MaxSeqNum = par("WS").intValue()-1;
    Next_frame_to_send = 0;
    Ack_expected = 1;
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
            if(nBuffered <= MaxSeqNum)
            {
                t = this->ReadMsgFromFile(E, Msg);
                if(t)
                {
                    // write to file the sender time info
                    sender_output.open ("sender_output.txt", ios_base::app);
                    if(sender_output.is_open())
                    {
                        sender_output<< "At time ["+to_string(simTime().dbl())+"], Node["+node_id+"] , Introducing channel error with code =["+ E+"]." << endl;
                        sender_output << endl;
                        EV << "At time ["+to_string(simTime().dbl())+"], Node["+node_id+"] , Introducing channel error with code =["+ E +"]." << endl;
                        EV<<"i will write in file now"<<endl;
                    }
                    sender_output.close();
                    //send self message (self is sender) with Name--> the error code read from file
                    //according to the error code (MUXCode) we will enter in the corresponding if statement
                    MyMessage_Base* msg3 = new MyMessage_Base(E.c_str());

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
        else if(MuxCode == "0000") //===> Default / no errors <===
        {
            EV<<"&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&              0000 RETRANSMIT"<<endl;
            par("TD") = 1.0;

            MyMessage_Base *mmsg = check_and_cast<MyMessage_Base *>(msg);
            bitset<8> answer_parity (mmsg->getTrailer_parity());

            string payyload(mmsg->getPayload());
            string trailler(answer_parity.to_string());
            sender_output.open ("sender_output.txt", ios_base::app);//output file for sender
            if(sender_output.is_open())
            {
                sender_output<< "At time ["+to_string(simTime().dbl())+"], Node["+node_id+"] [sent] frame with seq_num=["+ to_string(mmsg->getHeaderSeq_num())+"] and payload=["+payyload+"] and trailer=["+trailler+"] , Modified [-1] ,  Lost [No], Duplicate [0], Delay  [0]. ."<<endl;
                sender_output << endl;
                EV << "At time ["+to_string(simTime().dbl())+"], Node["+node_id+"] [sent] frame with seq_num=["+ to_string(mmsg->getHeaderSeq_num())+"] and payload=["+payyload+"] and trailer=["+trailler+"] , Modified [-1] ,  Lost [No], Duplicate [0], Delay  [0]. ."<<endl;

                EV<<"i will write in file now1"<<endl;
            }
            sender_output.close();

            sendDelayed(msg, par("TD").doubleValue(), "out");
        }
        else if(MuxCode == "0001") //===> delay error <===
        {
            par("TD") = 5.0;

            MyMessage_Base *mmsg = check_and_cast<MyMessage_Base *>(msg);
            bitset<8> answer_parity (mmsg->getTrailer_parity());

            string payyload(mmsg->getPayload());
            string trailler(answer_parity.to_string());
            sender_output.open ("sender_output.txt", ios_base::app);//output file for sender
            if(sender_output.is_open())
            {
                sender_output<< "At time ["+to_string(simTime().dbl())+"], Node["+node_id+"] [sent] frame with seq_num=["+ to_string(mmsg->getHeaderSeq_num())+"] and payload=["+payyload+"] and trailer=["+trailler+"] , Modified [-1] ,  Lost [No], Duplicate [0], Delay  [" + to_string(par("TD").doubleValue()) + "]. ."<<endl;

                sender_output << endl;
                EV << "At time ["+to_string(simTime().dbl())+"], Node["+node_id+"] [sent] frame with seq_num=["+ to_string(mmsg->getHeaderSeq_num())+"] and payload=["+payyload+"] and trailer=["+trailler+"] , Modified [-1] ,  Lost [No], Duplicate [0], Delay  [" + to_string(par("TD").doubleValue()) + "]. ."<<endl;

                EV<<"i will write in file now1"<<endl;
            }
            sender_output.close();

            sendDelayed(msg, par("TD").doubleValue(), "out");
        }
        else if(MuxCode == "0010") //===> duplication error <===
        {
            par("TD") = 1.0;

            MyMessage_Base *mmsg = check_and_cast<MyMessage_Base *>(msg);
            bitset<8> answer_parity (mmsg->getTrailer_parity());

            string payyload(mmsg->getPayload());
            string trailler(answer_parity.to_string());
            //create new duplicate message
            MyMessage_Base* mmsg_Dup = new MyMessage_Base("0000");
            mmsg_Dup = copyMessage(mmsg);

            sender_output.open ("sender_output.txt", ios_base::app);//output file for sender
            if(sender_output.is_open())
            {
                //original messasge
                sender_output<< "At time ["+to_string(simTime().dbl())+"], Node["+node_id+"] [sent] frame with seq_num=["+ to_string(mmsg->getHeaderSeq_num())+"] and payload=["+payyload+"] and trailer=["+trailler+"] , Modified [-1] ,  Lost [No], Duplicate [1], Delay  [0]. ."<<endl;
                //||===> duplicate message
                sender_output<< "At time ["+to_string(simTime().dbl() + par("DD").doubleValue()) +"], Node["+node_id+"] [sent] frame with seq_num=["+ to_string(mmsg->getHeaderSeq_num())+"] and payload=["+payyload+"] and trailer=["+trailler+"] , Modified [-1] ,  Lost [No], Duplicate [2], Delay  [0]. ."<<endl;

                sender_output << endl;

                EV<< "At time ["+to_string(simTime().dbl())+"], Node["+node_id+"] [sent] frame with seq_num=["+ to_string(mmsg->getHeaderSeq_num())+"] and payload=["+payyload+"] and trailer=["+trailler+"] , Modified [-1] ,  Lost [No], Duplicate [1], Delay  [0]. ."<<endl;
                EV<< "At time ["+to_string(simTime().dbl() + par("DD").doubleValue()) +"], Node["+node_id+"] [sent] frame with seq_num=["+ to_string(mmsg->getHeaderSeq_num())+"] and payload=["+payyload+"] and trailer=["+trailler+"] , Modified [-1] ,  Lost [No], Duplicate [2], Delay  [0]. ."<<endl;

                EV<<"i will write in file now1"<<endl;
            }
            sender_output.close();

            sendDelayed(msg, par("TD").doubleValue(), "out");
            sendDelayed(mmsg_Dup, par("TD").doubleValue() + par("DD").doubleValue(), "out");
        }
        else if(MuxCode == "0011") //===> duplication error and transmission delay<===
        {
            par("TD") = 5.0;

            MyMessage_Base *mmsg = check_and_cast<MyMessage_Base *>(msg);
            bitset<8> answer_parity (mmsg->getTrailer_parity());

            string payyload(mmsg->getPayload());
            string trailler(answer_parity.to_string());

            //create new duplicate message
            MyMessage_Base* mmsg_Dup = new MyMessage_Base("0000");
            mmsg_Dup = copyMessage(mmsg);

            sender_output.open ("sender_output.txt", ios_base::app);//output file for sender
            if(sender_output.is_open())
            {
                //original message with delay
                sender_output<< "At time ["+to_string(simTime().dbl())+"], Node["+node_id+"] [sent] frame with seq_num=["+ to_string(mmsg->getHeaderSeq_num())+"] and payload=["+payyload+"] and trailer=["+trailler+"] , Modified [-1] ,  Lost [No], Duplicate [1], Delay  [" + to_string(par("TD").doubleValue()) + "]. ."<<endl;
                //||===> duplicate message with delay
                sender_output<< "At time ["+to_string(simTime().dbl() + par("DD").doubleValue()) +"], Node["+node_id+"] [sent] frame with seq_num=["+ to_string(mmsg->getHeaderSeq_num())+"] and payload=["+payyload+"] and trailer=["+trailler+"] , Modified [-1] ,  Lost [No], Duplicate [2], Delay  [" + to_string(par("TD").doubleValue()) + "]. ."<<endl;

                sender_output << endl;

                EV<< "At time ["+to_string(simTime().dbl())+"], Node["+node_id+"] [sent] frame with seq_num=["+ to_string(mmsg->getHeaderSeq_num())+"] and payload=["+payyload+"] and trailer=["+trailler+"] , Modified [-1] ,  Lost [No], Duplicate [1], Delay  [" + to_string(par("TD").doubleValue()) + "]. ."<<endl;
                EV<< "At time ["+to_string(simTime().dbl() + par("DD").doubleValue()) +"], Node["+node_id+"] [sent] frame with seq_num=["+ to_string(mmsg->getHeaderSeq_num())+"] and payload=["+payyload+"] and trailer=["+trailler+"] , Modified [-1] ,  Lost [No], Duplicate [2], Delay  [" + to_string(par("TD").doubleValue()) + "]. ."<<endl;

                EV<<"i will write in file now1"<<endl;
            }
            sender_output.close();

            sendDelayed(msg, par("TD").doubleValue(), "out");
            sendDelayed(mmsg_Dup, par("TD").doubleValue() + par("DD").doubleValue(), "out");
        }
        else if(MuxCode == "0100") //===> Lost <===
        {
            MyMessage_Base *mmsg = check_and_cast<MyMessage_Base *>(msg);
            bitset<8> answer_parity (mmsg->getTrailer_parity());

            string payyload(mmsg->getPayload());
            string trailler(answer_parity.to_string());
            sender_output.open ("sender_output.txt", ios_base::app);//output file for sender
            if(sender_output.is_open())
            {
                //lost output
                sender_output<< "At time ["+to_string(simTime().dbl())+"], Node["+node_id+"] [sent] frame with seq_num=["+ to_string(mmsg->getHeaderSeq_num())+"] and payload=["+payyload+"] and trailer=["+trailler+"] , Modified [-1] ,  Lost [Yes], Duplicate [0], Delay  [0]. ."<<endl;
                sender_output << endl;
                EV <<  "At time ["+to_string(simTime().dbl())+"], Node["+node_id+"] [sent] frame with seq_num=["+ to_string(mmsg->getHeaderSeq_num())+"] and payload=["+payyload+"] and trailer=["+trailler+"] , Modified [-1] ,  Lost [Yes], Duplicate [0], Delay  [0]. ."<<endl;

                EV<<"i will write in file now1"<<endl;
            }
            sender_output.close();
            //we will not send delayed as the message is lost
        }
        else if(MuxCode == "0101") //===> Lost and delayed<===
        {
            MyMessage_Base *mmsg = check_and_cast<MyMessage_Base *>(msg);
            bitset<8> answer_parity (mmsg->getTrailer_parity());

            string payyload(mmsg->getPayload());
            string trailler(answer_parity.to_string());
            sender_output.open ("sender_output.txt", ios_base::app);//output file for sender
            if(sender_output.is_open())
            {
                //lost output
                sender_output<< "At time ["+to_string(simTime().dbl())+"], Node["+node_id+"] [sent] frame with seq_num=["+ to_string(mmsg->getHeaderSeq_num())+"] and payload=["+payyload+"] and trailer=["+trailler+"] , Modified [-1] ,  Lost [Yes], Duplicate [0], Delay  [" + to_string(par("TD").doubleValue()) + "]. ."<<endl;

                sender_output << endl;

                EV <<  "At time ["+to_string(simTime().dbl())+"], Node["+node_id+"] [sent] frame with seq_num=["+ to_string(mmsg->getHeaderSeq_num())+"] and payload=["+payyload+"] and trailer=["+trailler+"] , Modified [-1] ,  Lost [Yes], Duplicate [0], Delay  [" + to_string(par("TD").doubleValue()) + "]. ."<<endl;

                EV<<"i will write in file now1"<<endl;
            }
            sender_output.close();
            //we will not send delayed as the message is lost
        }
        else if(MuxCode == "0110") //===> Lost and Duplication <===
        {
            MyMessage_Base *mmsg = check_and_cast<MyMessage_Base *>(msg);
            bitset<8> answer_parity (mmsg->getTrailer_parity());

            string payyload(mmsg->getPayload());
            string trailler(answer_parity.to_string());
            sender_output.open ("sender_output.txt", ios_base::app);//output file for sender
            if(sender_output.is_open())
            {
                //lost output
                sender_output<< "At time ["+to_string(simTime().dbl())+"], Node["+node_id+"] [sent] frame with seq_num=["+ to_string(mmsg->getHeaderSeq_num())+"] and payload=["+payyload+"] and trailer=["+trailler+"] , Modified [-1] ,  Lost [Yes], Duplicate [1], Delay  [0]. ."<<endl;
                //duplicated messsage lost
                sender_output<< "At time ["+to_string(simTime().dbl() + par("DD").doubleValue()) +"], Node["+node_id+"] [sent] frame with seq_num=["+ to_string(mmsg->getHeaderSeq_num())+"] and payload=["+payyload+"] and trailer=["+trailler+"] , Modified [-1] ,  Lost [Yes], Duplicate [2], Delay  [0]. ."<<endl;

                sender_output << endl;

                EV <<  "At time ["+to_string(simTime().dbl())+"], Node["+node_id+"] [sent] frame with seq_num=["+ to_string(mmsg->getHeaderSeq_num())+"] and payload=["+payyload+"] and trailer=["+trailler+"] , Modified [-1] ,  Lost [Yes], Duplicate [1], Delay  [0]. ."<<endl;
                EV<< "At time ["+to_string(simTime().dbl() + par("DD").doubleValue()) +"], Node["+node_id+"] [sent] frame with seq_num=["+ to_string(mmsg->getHeaderSeq_num())+"] and payload=["+payyload+"] and trailer=["+trailler+"] , Modified [-1] ,  Lost [Yes], Duplicate [2], Delay  [0]. ."<<endl;
                EV<<"i will write in file now1"<<endl;
            }
            sender_output.close();
            //we will not send delayed as the message is lost
        }
        else if(MuxCode == "0111") //===> Lost, duplication and Delay <===
        {
            par("TD") = 5.0;
            MyMessage_Base *mmsg = check_and_cast<MyMessage_Base *>(msg);
            bitset<8> answer_parity (mmsg->getTrailer_parity());

            string payyload(mmsg->getPayload());
            string trailler(answer_parity.to_string());
            sender_output.open ("sender_output.txt", ios_base::app);//output file for sender
            if(sender_output.is_open())
            {
                //lost output
                sender_output<< "At time ["+to_string(simTime().dbl())+"], Node["+node_id+"] [sent] frame with seq_num=["+ to_string(mmsg->getHeaderSeq_num())+"] and payload=["+payyload+"] and trailer=["+trailler+"] , Modified [-1] ,  Lost [Yes], Duplicate [1], Delay  [" + to_string(par("TD").doubleValue()) + "]. ."<<endl;
                //duplicated messsage lost
                sender_output<< "At time ["+to_string(simTime().dbl() + par("DD").doubleValue()) +"], Node["+node_id+"] [sent] frame with seq_num=["+ to_string(mmsg->getHeaderSeq_num())+"] and payload=["+payyload+"] and trailer=["+trailler+"] , Modified [-1] ,  Lost [Yes], Duplicate [2], Delay  [" + to_string(par("TD").doubleValue()) + "]. ."<<endl;

                sender_output << endl;

                EV <<  "At time ["+to_string(simTime().dbl())+"], Node["+node_id+"] [sent] frame with seq_num=["+ to_string(mmsg->getHeaderSeq_num())+"] and payload=["+payyload+"] and trailer=["+trailler+"] , Modified [-1] ,  Lost [Yes], Duplicate [1], Delay  [" + to_string(par("TD").doubleValue()) + "]. ."<<endl;
                EV<< "At time ["+to_string(simTime().dbl() + par("DD").doubleValue()) +"], Node["+node_id+"] [sent] frame with seq_num=["+ to_string(mmsg->getHeaderSeq_num())+"] and payload=["+payyload+"] and trailer=["+trailler+"] , Modified [-1] ,  Lost [Yes], Duplicate [2], Delay  [" + to_string(par("TD").doubleValue()) + "]. ."<<endl;
                EV<<"i will write in file now1"<<endl;
            }
            sender_output.close();
            //we will not send delayed as the message is lost
        }
        else if(MuxCode == "1000") //===> Modification <===
        {
            EV<<"++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++>>>>>> IM HERE Modification"<<endl;
            MyMessage_Base *mmsg = check_and_cast<MyMessage_Base *>(msg);
            bitset<8> answer_parity (mmsg->getTrailer_parity());
            string payload = mmsg->getPayload();

            vector<bitset<1> > payloadInBit;
            for(int i =  0 ; i < payload.size() ; i++)
            {
                bitset<8> chr(payload[i]);
                string temp = chr.to_string();
                for(int j = 0 ; j < 8 ; j++)
                {
                    if(temp[j] == '0')
                        payloadInBit.push_back(0);
                    else
                        payloadInBit.push_back(1);
                }
            }

            par("TD") = 1.0;

            int rand = int(uniform(0,1) * (payloadInBit.size()-1));

            //insert the error payload to message
            payloadInBit[rand]^=1;

            //convert errored message into string and assign it to payload
            string temp= "";
            for(int i = 0 ; i < payloadInBit.size() ; i+=8) //error not working
            {
                string chr = "";
                for(int j = 0 ; j < 8 ; j++)
                {
                    if(payloadInBit[j+i] == '0')
                        chr+='0';
                    else
                        chr+='1';
                }
                bitset<8> chr2(chr);
                temp += (char)chr2.to_ulong();
            }

            mmsg->setPayload(temp.c_str());

            string payyload(mmsg->getPayload());
            string trailler(answer_parity.to_string());

            sender_output.open ("sender_output.txt", ios_base::app);//output file for sender
            if(sender_output.is_open())
            {
                sender_output<< "At time ["+to_string(simTime().dbl())+"], Node["+node_id+"] [sent] frame with seq_num=["+ to_string(mmsg->getHeaderSeq_num())+"] and payload=["+temp+"] and trailer=["+trailler+"] , Modified ["+to_string(rand)+"] ,  Lost [No], Duplicate [0], Delay  [0]. ."<<endl;
                sender_output << endl;
                EV << "At time ["+to_string(simTime().dbl())+"], Node["+node_id+"] [sent] frame with seq_num=["+ to_string(mmsg->getHeaderSeq_num())+"] and payload=["+temp+"] and trailer=["+trailler+"] , Modified ["+to_string(rand)+"] ,  Lost [No], Duplicate [0], Delay  [0]. ."<<endl;

                EV<<"i will write in file now1"<<endl;
            }
            sender_output.close();

            sendDelayed(msg, par("TD").doubleValue(), "out");
        }
        else if(MuxCode == "1001") //===> Modification and Delay<===
        {
            par("TD") = 5.0;
            EV<<"++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++>>>>>> IM HERE Modification and Delay"<<endl;
            MyMessage_Base *mmsg = check_and_cast<MyMessage_Base *>(msg);
            bitset<8> answer_parity (mmsg->getTrailer_parity());
            string payload = mmsg->getPayload();

            vector<bitset<1> > payloadInBit;
            for(int i =  0 ; i < payload.size() ; i++)
            {
                bitset<8> chr(payload[i]);
                string temp = chr.to_string();
                for(int j = 0 ; j < 8 ; j++)
                {
                    if(temp[j] == '0')
                        payloadInBit.push_back(0);
                    else
                        payloadInBit.push_back(1);
                }
            }
            int rand = int(uniform(0,1) * (payloadInBit.size()-1));

            //insert the error payload to message
            payloadInBit[rand]^=1;

            //convert errored message into string and assign it to payload
            string temp= "";
            for(int i = 0 ; i < payloadInBit.size() ; i+=8) //error not working
            {
                string chr = "";
                for(int j = 0 ; j < 8 ; j++)
                {
                    if(payloadInBit[j+i] == '0')
                        chr+='0';
                    else
                        chr+='1';
                }
                bitset<8> chr2(chr);
                temp += (char)chr2.to_ulong();
            }

            mmsg->setPayload(temp.c_str());

            string payyload(mmsg->getPayload());
            string trailler(answer_parity.to_string());

            sender_output.open ("sender_output.txt", ios_base::app);//output file for sender
            if(sender_output.is_open())
            {
                sender_output<< "At time ["+to_string(simTime().dbl())+"], Node["+node_id+"] [sent] frame with seq_num=["+ to_string(mmsg->getHeaderSeq_num())+"] and payload=["+temp+"] and trailer=["+trailler+"] , Modified ["+to_string(rand)+"] ,  Lost [No], Duplicate [0], Delay  [" + to_string(par("TD").doubleValue()) + "]. ."<<endl;
                sender_output << endl;
                EV << "At time ["+to_string(simTime().dbl())+"], Node["+node_id+"] [sent] frame with seq_num=["+ to_string(mmsg->getHeaderSeq_num())+"] and payload=["+temp+"] and trailer=["+trailler+"] , Modified ["+to_string(rand)+"] ,  Lost [No], Duplicate [0], Delay  [" + to_string(par("TD").doubleValue()) + "]. ."<<endl;

                EV<<"i will write in file now1"<<endl;
            }
            sender_output.close();

            sendDelayed(msg, par("TD").doubleValue(), "out");
        }
        else if(MuxCode == "1010") //===> Modification and Duplicate <===
        {
            EV<<"++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++>>>>>> IM HERE Modification and Duplicate"<<endl;
            MyMessage_Base *mmsg = check_and_cast<MyMessage_Base *>(msg);
            bitset<8> answer_parity (mmsg->getTrailer_parity());
            string payload = mmsg->getPayload();

            vector<bitset<1> > payloadInBit;
            for(int i =  0 ; i < payload.size() ; i++)
            {
                bitset<8> chr(payload[i]);
                string temp = chr.to_string();
                for(int j = 0 ; j < 8 ; j++)
                {
                    if(temp[j] == '0')
                        payloadInBit.push_back(0);
                    else
                        payloadInBit.push_back(1);
                }
            }

            par("TD") = 1.0;

            int rand = int(uniform(0,1) * (payloadInBit.size()-1));

            //insert the error payload to message
            payloadInBit[rand]^=1;

            //convert errored message into string and assign it to payload
            string temp= "";
            for(int i = 0 ; i < payloadInBit.size() ; i+=8) //error not working
            {
                string chr = "";
                for(int j = 0 ; j < 8 ; j++)
                {
                    if(payloadInBit[j+i] == '0')
                        chr+='0';
                    else
                        chr+='1';
                }
                bitset<8> chr2(chr);
                temp += (char)chr2.to_ulong();
            }

            mmsg->setPayload(temp.c_str());
            //create new duplicate message
            MyMessage_Base* mmsg_Dup = new MyMessage_Base("0000");
            mmsg_Dup = copyMessage(mmsg);

            string payyload(mmsg->getPayload());
            string trailler(answer_parity.to_string());

            sender_output.open ("sender_output.txt", ios_base::app);//output file for sender
            if(sender_output.is_open())
            {
                //original messasge
                sender_output<< "At time ["+to_string(simTime().dbl())+"], Node["+node_id+"] [sent] frame with seq_num=["+ to_string(mmsg->getHeaderSeq_num())+"] and payload=["+payyload+"] and trailer=["+trailler+"] , Modified ["+to_string(rand)+"] ,  Lost [No], Duplicate [1], Delay  [0]. ."<<endl;
                //||===> duplicate message
                sender_output<< "At time ["+to_string(simTime().dbl() + par("DD").doubleValue()) +"], Node["+node_id+"] [sent] frame with seq_num=["+ to_string(mmsg->getHeaderSeq_num())+"] and payload=["+payyload+"] and trailer=["+trailler+"] , Modified ["+to_string(rand)+"] ,  Lost [No], Duplicate [2], Delay  [0]. ."<<endl;

                sender_output << endl;

                EV<< "At time ["+to_string(simTime().dbl())+"], Node["+node_id+"] [sent] frame with seq_num=["+ to_string(mmsg->getHeaderSeq_num())+"] and payload=["+payyload+"] and trailer=["+trailler+"] , Modified ["+to_string(rand)+"] ,  Lost [No], Duplicate [1], Delay  [0]. ."<<endl;
                EV<< "At time ["+to_string(simTime().dbl() + par("DD").doubleValue()) +"], Node["+node_id+"] [sent] frame with seq_num=["+ to_string(mmsg->getHeaderSeq_num())+"] and payload=["+payyload+"] and trailer=["+trailler+"] , Modified ["+to_string(rand)+"] ,  Lost [No], Duplicate [2], Delay  [0]. ."<<endl;

                EV<<"i will write in file now1"<<endl;
            }
            sender_output.close();

            sendDelayed(msg, par("TD").doubleValue(), "out");
            sendDelayed(mmsg_Dup, par("TD").doubleValue()+par("DD").doubleValue(), "out");

        }
        else if(MuxCode == "1011") //===> Modification, Duplicate and Delay <===
        {
            EV<<"++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++>>>>>> IM HERE  Modification, Duplicate and Delay"<<endl;
            MyMessage_Base *mmsg = check_and_cast<MyMessage_Base *>(msg);
            bitset<8> answer_parity (mmsg->getTrailer_parity());
            string payload = mmsg->getPayload();

            vector<bitset<1> > payloadInBit;
            for(int i =  0 ; i < payload.size() ; i++)
            {
                bitset<8> chr(payload[i]);
                string temp = chr.to_string();
                for(int j = 0 ; j < 8 ; j++)
                {
                    if(temp[j] == '0')
                        payloadInBit.push_back(0);
                    else
                        payloadInBit.push_back(1);
                }
            }

            par("TD") = 5.0;

            int rand = int(uniform(0,1) * (payloadInBit.size()-1));

            //insert the error payload to message
            payloadInBit[rand]^=1;

            //convert errored message into string and assign it to payload
            string temp= "";
            for(int i = 0 ; i < payloadInBit.size() ; i+=8) //error not working
            {
                string chr = "";
                for(int j = 0 ; j < 8 ; j++)
                {
                    if(payloadInBit[j+i] == '0')
                        chr+='0';
                    else
                        chr+='1';
                }
                bitset<8> chr2(chr);
                temp += (char)chr2.to_ulong();
            }

            mmsg->setPayload(temp.c_str());

            //create new duplicate message
            MyMessage_Base* mmsg_Dup = new MyMessage_Base("0000");
            mmsg_Dup = copyMessage(mmsg);


            string payyload(mmsg->getPayload());
            string trailler(answer_parity.to_string());

            sender_output.open ("sender_output.txt", ios_base::app);//output file for sender
            if(sender_output.is_open())
            {
                //original messasge
                sender_output<< "At time ["+to_string(simTime().dbl())+"], Node["+node_id+"] [sent] frame with seq_num=["+ to_string(mmsg->getHeaderSeq_num())+"] and payload=["+payyload+"] and trailer=["+trailler+"] , Modified ["+to_string(rand)+"] ,  Lost [No], Duplicate [1], Delay  [" + to_string(par("TD").doubleValue()) + "]. ."<<endl;
                //||===> duplicate message
                sender_output<< "At time ["+to_string(simTime().dbl() + par("DD").doubleValue()) +"], Node["+node_id+"] [sent] frame with seq_num=["+ to_string(mmsg->getHeaderSeq_num())+"] and payload=["+payyload+"] and trailer=["+trailler+"] , Modified ["+to_string(rand)+"] ,  Lost [No], Duplicate [2], Delay  [" + to_string(par("TD").doubleValue()) + "]. ."<<endl;

                sender_output << endl;

                EV<< "At time ["+to_string(simTime().dbl())+"], Node["+node_id+"] [sent] frame with seq_num=["+ to_string(mmsg->getHeaderSeq_num())+"] and payload=["+payyload+"] and trailer=["+trailler+"] , Modified ["+to_string(rand)+"] ,  Lost [No], Duplicate [1], Delay  [" + to_string(par("TD").doubleValue()) + "]. ."<<endl;
                EV<< "At time ["+to_string(simTime().dbl() + par("DD").doubleValue()) +"], Node["+node_id+"] [sent] frame with seq_num=["+ to_string(mmsg->getHeaderSeq_num())+"] and payload=["+payyload+"] and trailer=["+trailler+"] , Modified ["+to_string(rand)+"] ,  Lost [No], Duplicate [2], Delay  [" + to_string(par("TD").doubleValue()) + "]. ."<<endl;

                EV<<"i will write in file now1"<<endl;
            }
            sender_output.close();

            sendDelayed(msg, par("TD").doubleValue(), "out");
            sendDelayed(mmsg_Dup, par("TD").doubleValue()+par("DD").doubleValue(), "out");

        }
        else if(MuxCode == "1100") //===> Modification, Loss<===
        {

            EV<<"++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++>>>>>> IM HERE Modification, Loss"<<endl;
            MyMessage_Base *mmsg = check_and_cast<MyMessage_Base *>(msg);
            bitset<8> answer_parity (mmsg->getTrailer_parity());
            string payload = mmsg->getPayload();

            vector<bitset<1> > payloadInBit;
            for(int i =  0 ; i < payload.size() ; i++)
            {
                bitset<8> chr(payload[i]);
                string temp = chr.to_string();
                for(int j = 0 ; j < 8 ; j++)
                {
                    if(temp[j] == '0')
                        payloadInBit.push_back(0);
                    else
                        payloadInBit.push_back(1);
                }
            }
            int rand = int(uniform(0,1) * (payloadInBit.size()-1));

            //insert the error payload to message
            payloadInBit[rand]^=1;

            //convert errored message into string and assign it to payload
            string temp= "";
            for(int i = 0 ; i < payloadInBit.size() ; i+=8) //error not working
            {
                string chr = "";
                for(int j = 0 ; j < 8 ; j++)
                {
                    if(payloadInBit[j+i] == '0')
                        chr+='0';
                    else
                        chr+='1';
                }
                bitset<8> chr2(chr);
                temp += (char)chr2.to_ulong();
            }

            mmsg->setPayload(temp.c_str());

            string payyload(mmsg->getPayload());
            string trailler(answer_parity.to_string());

            sender_output.open ("sender_output.txt", ios_base::app);//output file for sender
            if(sender_output.is_open())
            {
                sender_output<< "At time ["+to_string(simTime().dbl())+"], Node["+node_id+"] [sent] frame with seq_num=["+ to_string(mmsg->getHeaderSeq_num())+"] and payload=["+temp+"] and trailer=["+trailler+"] , Modified ["+to_string(rand)+"] ,  Lost [Yes], Duplicate [0], Delay  [0]. ."<<endl;
                sender_output << endl;
                EV << "At time ["+to_string(simTime().dbl())+"], Node["+node_id+"] [sent] frame with seq_num=["+ to_string(mmsg->getHeaderSeq_num())+"] and payload=["+temp+"] and trailer=["+trailler+"] , Modified ["+to_string(rand)+"] ,  Lost [Yes], Duplicate [0], Delay  [0]. ."<<endl;

                EV<<"i will write in file now1"<<endl;
            }
            sender_output.close();
            //here we will not call sendDelayed() because the message is lost
            // sendDelayed(msg, par("TD").doubleValue(), "out");
        }
        else if(MuxCode == "1101") //===> Modification, Loss and Delay<===
        {

            EV<<"++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++>>>>>> IM HERE  Modification, Loss and Delay"<<endl;
            MyMessage_Base *mmsg = check_and_cast<MyMessage_Base *>(msg);
            bitset<8> answer_parity (mmsg->getTrailer_parity());
            string payload = mmsg->getPayload();

            vector<bitset<1> > payloadInBit;
            for(int i =  0 ; i < payload.size() ; i++)
            {
                bitset<8> chr(payload[i]);
                string temp = chr.to_string();
                for(int j = 0 ; j < 8 ; j++)
                {
                    if(temp[j] == '0')
                        payloadInBit.push_back(0);
                    else
                        payloadInBit.push_back(1);
                }
            }
            int rand = int(uniform(0,1) * (payloadInBit.size()-1));

            //insert the error payload to message
            payloadInBit[rand]^=1;

            //convert errored message into string and assign it to payload
            string temp= "";
            for(int i = 0 ; i < payloadInBit.size() ; i+=8) //error not working
            {
                string chr = "";
                for(int j = 0 ; j < 8 ; j++)
                {
                    if(payloadInBit[j+i] == '0')
                        chr+='0';
                    else
                        chr+='1';
                }
                bitset<8> chr2(chr);
                temp += (char)chr2.to_ulong();
            }

            mmsg->setPayload(temp.c_str());

            string payyload(mmsg->getPayload());
            string trailler(answer_parity.to_string());

            sender_output.open ("sender_output.txt", ios_base::app);//output file for sender
            if(sender_output.is_open())
            {
                sender_output<< "At time ["+to_string(simTime().dbl())+"], Node["+node_id+"] [sent] frame with seq_num=["+ to_string(mmsg->getHeaderSeq_num())+"] and payload=["+temp+"] and trailer=["+trailler+"] , Modified ["+to_string(rand)+"] ,  Lost [Yes], Duplicate [0], Delay  [" + to_string(par("TD").doubleValue()) + "]. ."<<endl;
                sender_output << endl;
                EV << "At time ["+to_string(simTime().dbl())+"], Node["+node_id+"] [sent] frame with seq_num=["+ to_string(mmsg->getHeaderSeq_num())+"] and payload=["+temp+"] and trailer=["+trailler+"] , Modified ["+to_string(rand)+"] ,  Lost [Yes], Duplicate [0], Delay  [" + to_string(par("TD").doubleValue()) + "]. ."<<endl;

                EV<<"i will write in file now1"<<endl;
            }
            sender_output.close();
            //here we will not call sendDelayed() because the message is lost
            // sendDelayed(msg, par("TD").doubleValue(), "out");
        }
        else if(MuxCode == "1110") //===> Modification, Loss and Duplicate <===
        {
            EV<<"++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++>>>>>> IM HERE  Modification, Loss and Duplicate"<<endl;
            MyMessage_Base *mmsg = check_and_cast<MyMessage_Base *>(msg);
            bitset<8> answer_parity (mmsg->getTrailer_parity());
            string payload = mmsg->getPayload();

            vector<bitset<1> > payloadInBit;
            for(int i =  0 ; i < payload.size() ; i++)
            {
                bitset<8> chr(payload[i]);
                string temp = chr.to_string();
                for(int j = 0 ; j < 8 ; j++)
                {
                    if(temp[j] == '0')
                        payloadInBit.push_back(0);
                    else
                        payloadInBit.push_back(1);
                }
            }

            par("TD") = 1.0;

            int rand = int(uniform(0,1) * (payloadInBit.size()-1));

            //insert the error payload to message
            payloadInBit[rand]^=1;

            //convert errored message into string and assign it to payload
            string temp= "";
            for(int i = 0 ; i < payloadInBit.size() ; i+=8) //error not working
            {
                string chr = "";
                for(int j = 0 ; j < 8 ; j++)
                {
                    if(payloadInBit[j+i] == '0')
                        chr+='0';
                    else
                        chr+='1';
                }
                bitset<8> chr2(chr);
                temp += (char)chr2.to_ulong();
            }

            mmsg->setPayload(temp.c_str());

            string payyload(mmsg->getPayload());
            string trailler(answer_parity.to_string());

            sender_output.open ("sender_output.txt", ios_base::app);//output file for sender
            if(sender_output.is_open())
            {
                //original messasge
                sender_output<< "At time ["+to_string(simTime().dbl())+"], Node["+node_id+"] [sent] frame with seq_num=["+ to_string(mmsg->getHeaderSeq_num())+"] and payload=["+payyload+"] and trailer=["+trailler+"] , Modified ["+to_string(rand)+"] ,  Lost [Yes], Duplicate [1], Delay  [0]. ."<<endl;
                //||===> duplicate message
                sender_output<< "At time ["+to_string(simTime().dbl() + par("DD").doubleValue()) +"], Node["+node_id+"] [sent] frame with seq_num=["+ to_string(mmsg->getHeaderSeq_num())+"] and payload=["+payyload+"] and trailer=["+trailler+"] , Modified ["+to_string(rand)+"] ,  Lost [Yes], Duplicate [2], Delay  [0]. ."<<endl;

                sender_output << endl;

                EV<< "At time ["+to_string(simTime().dbl())+"], Node["+node_id+"] [sent] frame with seq_num=["+ to_string(mmsg->getHeaderSeq_num())+"] and payload=["+payyload+"] and trailer=["+trailler+"] , Modified ["+to_string(rand)+"] ,  Lost [Yes], Duplicate [1], Delay  [0]. ."<<endl;
                EV<< "At time ["+to_string(simTime().dbl() + par("DD").doubleValue()) +"], Node["+node_id+"] [sent] frame with seq_num=["+ to_string(mmsg->getHeaderSeq_num())+"] and payload=["+payyload+"] and trailer=["+trailler+"] , Modified ["+to_string(rand)+"] ,  Lost [Yes], Duplicate [2], Delay  [0]. ."<<endl;

                EV<<"i will write in file now1"<<endl;
            }
            sender_output.close();
            //here we will not call sendDelayed() because the 2 messages are lost
            //sendDelayed(msg, par("TD").doubleValue(), "out");
            //sendDelayed(mmsg_Dup, par("TD").doubleValue()+par("DD").doubleValue(), "out");
        }
        else if(MuxCode == "1111") //===> Modification, Loss, Duplicate and Delay <===
                {
                    EV<<"++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++>>>>>> IM HERE Modification, Loss, Duplicate and Delay"<<endl;
                    MyMessage_Base *mmsg = check_and_cast<MyMessage_Base *>(msg);
                    bitset<8> answer_parity (mmsg->getTrailer_parity());
                    string payload = mmsg->getPayload();

                    vector<bitset<1> > payloadInBit;
                    for(int i =  0 ; i < payload.size() ; i++)
                    {
                        bitset<8> chr(payload[i]);
                        string temp = chr.to_string();
                        for(int j = 0 ; j < 8 ; j++)
                        {
                            if(temp[j] == '0')
                                payloadInBit.push_back(0);
                            else
                                payloadInBit.push_back(1);
                        }
                    }

                    par("TD") = 5.0;

                    int rand = int(uniform(0,1) * (payloadInBit.size()-1));

                    //insert the error payload to message
                    payloadInBit[rand]^=1;

                    //convert errored message into string and assign it to payload
                    string temp= "";
                    for(int i = 0 ; i < payloadInBit.size() ; i+=8) //error not working
                    {
                        string chr = "";
                        for(int j = 0 ; j < 8 ; j++)
                        {
                            if(payloadInBit[j+i] == '0')
                                chr+='0';
                            else
                                chr+='1';
                        }
                        bitset<8> chr2(chr);
                        temp += (char)chr2.to_ulong();
                    }

                    mmsg->setPayload(temp.c_str());

                    string payyload(mmsg->getPayload());
                    string trailler(answer_parity.to_string());

                    sender_output.open ("sender_output.txt", ios_base::app);//output file for sender
                    if(sender_output.is_open())
                    {
                        //original messasge
                        sender_output<< "At time ["+to_string(simTime().dbl())+"], Node["+node_id+"] [sent] frame with seq_num=["+ to_string(mmsg->getHeaderSeq_num())+"] and payload=["+payyload+"] and trailer=["+trailler+"] , Modified ["+to_string(rand)+"] ,  Lost [Yes], Duplicate [1], Delay  [" + to_string(par("TD").doubleValue()) + "]. ."<<endl;
                        //||===> duplicate message
                        sender_output<< "At time ["+to_string(simTime().dbl() + par("DD").doubleValue()) +"], Node["+node_id+"] [sent] frame with seq_num=["+ to_string(mmsg->getHeaderSeq_num())+"] and payload=["+payyload+"] and trailer=["+trailler+"] , Modified ["+to_string(rand)+"] ,  Lost [Yes], Duplicate [2], Delay  [" + to_string(par("TD").doubleValue()) + "]. ."<<endl;

                        sender_output << endl;

                        EV<< "At time ["+to_string(simTime().dbl())+"], Node["+node_id+"] [sent] frame with seq_num=["+ to_string(mmsg->getHeaderSeq_num())+"] and payload=["+payyload+"] and trailer=["+trailler+"] , Modified ["+to_string(rand)+"] ,  Lost [Yes], Duplicate [1], Delay  [" + to_string(par("TD").doubleValue()) + "]. ."<<endl;
                        EV<< "At time ["+to_string(simTime().dbl() + par("DD").doubleValue()) +"], Node["+node_id+"] [sent] frame with seq_num=["+ to_string(mmsg->getHeaderSeq_num())+"] and payload=["+payyload+"] and trailer=["+trailler+"] , Modified ["+to_string(rand)+"] ,  Lost [Yes], Duplicate [2], Delay  [" + to_string(par("TD").doubleValue()) + "]. ."<<endl;

                        EV<<"i will write in file now1"<<endl;
                    }
                    sender_output.close();
                    //here we will not call sendDelayed() because the 2 messages are lost
                    //sendDelayed(msg, par("TD").doubleValue(), "out");
                    //sendDelayed(mmsg_Dup, par("TD").doubleValue()+par("DD").doubleValue(), "out");
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
                sender_output.open ("sender_output.txt", ios_base::app);//output file for sender
                if(sender_output.is_open())
                {
                    sender_output<<"Time out event at time ["+to_string(simTime().dbl())+"], at Node["+node_id+"] for frame with seq_num=["+to_string(mmsg->getHeaderSeq_num())+"]"<<endl;
                    sender_output << endl;
                    EV << "Time out event at time ["+to_string(simTime().dbl())+"], at Node["+node_id+"] for frame with seq_num=["+to_string(mmsg->getHeaderSeq_num())+"]"<<endl;
                    EV<<"i will write in file now2"<<endl;
                }
                sender_output.close();
                EV<<"Timed out message " << mmsg->getHeaderSeq_num() << endl;
                Next_frame_to_send = Ack_expected - 1;
                for(i = 1 ; i <= nBuffered ; i++)
                {

                    MyMessage_Base* msg2be_Resent = new MyMessage_Base();
                    msg2be_Resent = copyMessage(buffer.front());
                    msg2be_Resent->setName("0000");

                    EV << "Retransmit: " << msg2be_Resent->getHeaderSeq_num() << endl;
                    EV<<"buffuer.fornt "<<buffer.front()->getHeaderSeq_num()<<endl;
                    buffer.push(buffer.front());
                    buffer.pop();
                    EV<<"SIMTIME():"<<simTime()<<"   "<<i*par("PT").doubleValue()<<endl;
//                    sendDelayed(msg2be_Resent, par("TD").doubleValue()+ i*par("PT").doubleValue(), "out");
                    scheduleAt(simTime() + i*par("PT").doubleValue(), msg2be_Resent);
                    inc(Next_frame_to_send, MaxSeqNum);
                }
                EV<<"buffuer.fornt "<<buffer.front()->getHeaderSeq_num()<<endl;
                EV <<"End of frame: " << Next_frame_to_send << endl;
            }
        }
        else if(MuxCode == "AckTransmission")
        {
            MyMessage_Base *mmsg = check_and_cast<MyMessage_Base *>(msg);
            receiver_output.open ("receiver_output.txt", ios_base::app);//output file for sender
            double rand = uniform(1,1);//apply the random function to know weather i will loose ack or no

            EV<<"rand= "<<rand<< " par(LP)= "<<endl;
            if(receiver_output.is_open())
            {

                if(mmsg->getFrame_type()==1)
                {
                    if(rand>par("LP").doubleValue())//if rand generated is greater than the probability of loss of ACK so we will send it
                    {

                        receiver_output<< " At time[" + to_string(simTime().dbl())+ "], Node[" + node_id + "] Sending [ACK] with number ["+to_string(mmsg->getAck_Nack_num())+"] , loss [No]"<<endl;
                        EV << " At time[" + to_string(simTime().dbl())+ "], Node[" + node_id + "] Sending [ACK] with number ["+to_string(mmsg->getAck_Nack_num())+"] , loss [No]"<<endl;
                    }
                    else//otherwise we the ACK will be lost
                    {
                    receiver_output<< " At time[" + to_string(simTime().dbl())+ "], Node[" + node_id + "] Sending [ACK] with number ["+to_string(mmsg->getAck_Nack_num())+"] , loss [Yes]"<<endl;
                    EV << " At time[" + to_string(simTime().dbl())+ "], Node[" + node_id + "] Sending [ACK] with number ["+to_string(mmsg->getAck_Nack_num())+"] , loss [Yes]"<<endl;
                    }
                    receiver_output << endl;
                }else if(mmsg->getFrame_type()==2)
                {
                    if(rand>par("LP").doubleValue())//if rand generated is greater than the probability of loss of NACK so we will send it
                    {
                        receiver_output<< " At time[" + to_string(simTime().dbl())+ "], Node[" + node_id + "] Sending [NACK] with number [" + to_string(mmsg->getAck_Nack_num()) + "] , loss [No]"<<endl;
                        EV << " At time[" + to_string(simTime().dbl())+ "], Node[" + node_id + "] Sending [NACK] with number [" + to_string(mmsg->getAck_Nack_num()) + "] , loss [No]"<<endl;
                    }
                    else//otherwise we the NACK will be lost
                    {
                        receiver_output<< " At time[" + to_string(simTime().dbl())+ "], Node[" + node_id + "] Sending [NACK] with number [" + to_string(mmsg->getAck_Nack_num()) + "] , loss [Yes]"<<endl;
                        EV << " At time[" + to_string(simTime().dbl())+ "], Node[" + node_id + "] Sending [NACK] with number [" + to_string(mmsg->getAck_Nack_num()) + "] , loss [Yes]"<<endl;
                    }
                }
                EV<<"i will write in file receiver now"<<endl;
            }
            receiver_output.close();
            if(rand>par("LP").doubleValue())
            {
                EV<<"send ack no ack loss"<<endl;
                sendDelayed(msg, par("TD").doubleValue(), "out");
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
            node_id=line[1];
            if(line[1]=='1')
            {
//                myfile.open ("D:\\Uni\\Senior 1\\Semester 1\\Networks\\Project_test\\node1.txt");
//                myfile.open ("D:\\GAM3A\\4- Senior 01\\Computer networks\\github\\Networks_Project\\Project\\node1.txt");
                myfile.open ("D:\\CUFE\\Fall 2022\\Networks\\project\\Networks_Project\\Project\\node1.txt");

//                sender_output.open ("D:\\CUFE\\Fall 2022\\Networks\\project\\Networks_Project\\Project\\sender_output.txt",ios_base::app);//output file for sender
//                receiver_output.open ("D:\\CUFE\\Fall 2022\\Networks\\project\\Networks_Project\\Project\\receiver_output.txt",ios_base::app);//output file for receiver

                sender_output.open ("sender_output.txt",  ios_base::app);//output file for sender
                receiver_output.open ("receiver_output.txt",  ios_base::app);//output file for receiver
            }
            else
            {
//                myfile.open ("D:\\Uni\\Senior 1\\Semester 1\\Networks\\Project_test\\node0.txt");
//                myfile.open ("D:\\GAM3A\\4- Senior 01\\Computer networks\\github\\Networks_Project\\Project\\node0.txt");
                myfile.open ("D:\\CUFE\\Fall 2022\\Networks\\project\\Networks_Project\\Project\\node0.txt");
            }

            int startT=stol(startTime);
            scheduleAt(simTime()+startT,new cMessage("Start"));
        }
        else
        {
            MyMessage_Base *mmsg = check_and_cast<MyMessage_Base *>(msg);
            if(mmsg->getFrame_type() == 1)//sender if ack received if NACK ignore
            {
                EV<< "Advance Window NOW " << endl;
                EV<<"ACK expected before: " <<Ack_expected<<endl;
                EV<<"Ack Received: "<<mmsg->getAck_Nack_num()<<endl;
                EV<<"Next frame to send: "<< Next_frame_to_send<<endl;
                EV << "Begining of the buffer before: " << buffer.front()->getHeaderSeq_num() << endl;
                if(Ack_expected ==  mmsg->getAck_Nack_num())
                {
                    nBuffered--;
                    if(!buffer.empty())
                        buffer.pop();
                    inc(Ack_expected, MaxSeqNum);
                    EV<<"ACK expected after: " <<Ack_expected<<endl;
                    EV << "Begining of the buffer after: " << buffer.front()->getHeaderSeq_num() << endl;
                }
                //advances the lower end and frees the buffer next is to send any message if there exist in file


                //now i can send more messages
                scheduleAt(simTime() ,new cMessage("Start"));
            }
            else if(mmsg->getFrame_type() == 0)//receiver
            {
                MyMessage_Base* msg3 = new MyMessage_Base("AckTransmission");
                //receiver
                //check expected frame
                //send ack/ nack

                    EV <<"Iam Rec" <<endl;
                    EV << "Seq num: " << mmsg->getHeaderSeq_num() << endl;
                    EV << "Frame expected: " << Frame_expected << endl;
                    if(mmsg->getHeaderSeq_num() == Frame_expected)
                    {
                        EV << "Seq num: " << mmsg->getHeaderSeq_num() << endl;
                        EV << "Frame expected: " << Frame_expected << endl;
                        bool correct = checkParity(mmsg);
                        EV << correct << endl;
                        if(correct)
                        {
                            //send ack as frame expected is correct
                            msg3->setPayload("Ack");
                            msg3->setFrame_type(1);
                            inc(Frame_expected, MaxSeqNum);
                            msg3->setAck_Nack_num(Frame_expected);
                            scheduleAt(simTime() + par("PT").doubleValue(), msg3);
                        }
                    }
                    //                else
                    //                {
                    //                    //send Nack because this is not the expected frame number
                    //                    msg3->setPayload("Nack");
                    //                    msg3->setFrame_type(2);
                    //                    msg3->setAck_Nack_num(Frame_expected);
                    //                    scheduleAt(simTime() + par("PT").doubleValue(), msg3);
                    //                }


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
          for(int i = 0; i < 4; i++)
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
  //  EV<<"HII i am sender in calc parity  "<<(char)answer_parity.to_ulong()<<endl;
    msg->setTrailer_parity((char)answer_parity.to_ulong());
    return true;
}

bool Node::checkParity(MyMessage_Base*&msg)
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


