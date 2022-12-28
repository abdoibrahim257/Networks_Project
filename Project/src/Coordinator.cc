#include "Coordinator.h"
#include <fstream>
#include <string>
#include <bitset>
#include <vector>
#include <iostream>
#include <cstdlib>
using namespace std;


Define_Module(Coordinator);

void Coordinator::initialize()
{
      string line;
//      ifstream myfile ("D:\\Uni\\Senior 1\\Semester 1\\Networks\\Project_test\\coordinator.txt");
//      ifstream myfile ("D:\\GAM3A\\4- Senior 01\\Computer networks\\github\\Networks_Project\\Project\\coordinator.txt");
      ifstream myfile ("D:\\CUFE\\Fall 2022\\Networks\\project\\Networks_Project\\Project\\coordinator.txt");
      if (myfile.is_open())
      {
        while(getline (myfile,line))
        {
            EV<<line<<endl;
        }
        myfile.close();
        if(line[1] == '0')
        {
            string start = to_string(line[3]);
            cMessage* msg  = new cMessage(line.c_str());
            send (msg,"out",0);
        }
        else if((line[1] =='1'))
        {
            string start = to_string(line[3]);
            cMessage* msg  = new cMessage(line.c_str());
            send (msg,"out",1);
        }
      }

      else EV << "Unable to open file";
}

void Coordinator::handleMessage(cMessage *msg)
{
    // TODO - Generated method body
}
