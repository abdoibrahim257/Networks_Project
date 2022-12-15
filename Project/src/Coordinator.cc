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
      ifstream myfile ("D:\\Uni\\Senior 1\\Semester 1\\Networks\\Project_test\\coordinator.txt");
      //ifstream myfile.open ("D:\\GAM3A\\4- Senior 01\\Computer networks\\github\\Networks_Project\\Project\\coordinator.txt");
      if (myfile.is_open())
      {
        while(getline (myfile,line))
        {
            EV<<line<<endl;
        }
        myfile.close();
        if(line[1]=='0')
        {
            string start = to_string(line[3]);
            cMessage* msg  = new cMessage(line.c_str());
            send (msg,"out",0);
        }
        else if((line[1]=='1'))
        {
            string start = to_string(line[3]);
            cMessage* msg  = new cMessage(line.c_str());
            send (msg,"out",1);
        }
      }

      else EV << "Unable to open file";

    //read from file
    //check which node and start timer
    //assign variable node1 or 2
    //send to whether each to be the sender with msg containing start time
}

void Coordinator::handleMessage(cMessage *msg)
{
    // TODO - Generated method body
}
