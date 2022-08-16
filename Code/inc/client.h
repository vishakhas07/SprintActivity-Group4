#ifndef __CLIENT_H_
#define __CLIENT_H_

#include <iostream>		
#include <vector>		 
#include <sstream>		
#include <unistd.h>		
#include <cstdlib>	
#include <stdio.h>		
#include <cstring>	
#include <errno.h>		
#include <string.h>		
#include <sys/socket.h>	
#include <sys/stat.h>	
#include <sys/types.h>	
#include <netinet/in.h>	
#include <arpa/inet.h>	
#include <fcntl.h>		
#include "client.h"
#include <fstream>
#include <cstdlib>	//for exit
#include <signal.h>
#define DELIM '.'   



#define BLOCKSIZE 512	
#define BUFFERSIZE 1024	

#define SUCCESS 0           
#define FAILURE 1          

using namespace std;

// Check if given string is a numeric string or not
bool IsNumber(const string & str);

// Validate Mobile Number
bool IsValidMobileNum(string mobNum);

// Signal Handling
void signal_handler(int sig);



class Client {
	
	public:
	    
		Client();	// Default Constructor
		Client(string IpAddress, string PortNo, string mobNum);	// Parameterized Constructor
		~Client();	// Destructor
		
		// Creation of a Socket
		int OpenSocket();	
		
		// Checking if the Mobile Number is Registered
		int IsMobNumRegistered(string mobNum);
		
		//Displaying the Main Menu
		int MainMenu();
	
		// Sending Data to Server
		int SendData(char* data);
		
		// Read the Data sent by Server
		int ReadData();
		
		// Check if Service is Activated for Mobile Number
		int IsServiceActivated();
		
		// Activate Service
		int ActivateService();
		
		// Deactivate Service
		int DeactivateService();
		
		// Update Service
		int UpdateService();
		
		// Call another Client
		int CallClient();
		
	private:
		int sd;		// Socket Descriptor
		struct sockaddr_in serverAddress;	// Server Address
		string mobileNumber;	// Mobile Number
		int server_struct_len = sizeof(serverAddress);
};

#endif
