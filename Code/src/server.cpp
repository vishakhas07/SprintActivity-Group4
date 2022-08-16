#include "server.h"
#define PORT 4056

void signal_handler(int sig) 
{ 
    	cout << "Ctrl+C Sent: " << endl << "Server EXIT..."<<endl;  
        exit(EXIT_FAILURE);
}

Server::Server() {

	sd = 0;
	fd = 0;
	mobNumber = "";
	databuf[0] = '\0';
	clientIpAddress[0] = '\0';
	
	// Populating Own address structure to bind it to the Socket
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(4056);	// PORT NO
	serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");	// IP ADDRESS

	
}

int Server::OpenSocket() {

	sd = socket(AF_INET,SOCK_DGRAM, IPPROTO_UDP);
	if (sd < 0) {

		perror("SOCKET OPEN FAILURE");
        	exit(0);
	}
	
	return SUCCESS;
}

int Server::BindData() {

	int status = 0;
	
	// Now Binding
	status = bind(sd, (const struct sockaddr*) &serverAddress, sizeof(serverAddress));
	if (status < 0) {
		perror("BIND FAILURE");
		close(sd);
        	exit(0);
	}
	cout<<"Server done with binding"<<endl;

	cout<<"Server is now open to listening for clients"<<endl;

	cout << endl << "Request from - Client IP Address: " <<"127.0.0.1"<<endl;

	return SUCCESS;
}


int Server::ReadData() {

    	int count = 0;
	
	while(1) {

		// Receive Data from Client
		count = recvfrom(sd, databuf, BUFFERSIZE, 0,(struct sockaddr*)&clientAddress,(socklen_t*)&client_struct_len);
		if (count <= 0) {
			perror("DATA NOT RECEIVED FROM CLIENT");
			close(sd);
		        exit(0);
		}
	
		databuf[count] = '\0';
	
		// cout << "Data Received from Client: " << databuf << endl;
		cout << databuf << endl;
	
		if (strcmp(databuf, "Exit") == 0) {
			break;
		}
		
		// Tokenize Data sent by Client
		TokenizeData(databuf);		
	}
	
	return SUCCESS;
}

int Server::SendData(char* data) {

	int count = 0, len = 0;
	len = strlen(data);

	count = sendto(sd, data, len, 0,(struct sockaddr*)&clientAddress,client_struct_len);
	if (count < 0) {
		perror("DATA NOT SENT TO CLIENT");
		close(sd);
	    	exit(0);
	}
	return SUCCESS;
}	

int Server::TokenizeData(char* data) {	

	string str = data;
	
	// Split the string into tokens
	vector<string> tokens;
	
	stringstream ss(str);
	string intermediate;
	
	while(getline(ss, intermediate, DELIM)) {
		tokens.push_back(intermediate);
	}
	
	// Store Case (tokens[1])
	int choice = atoi(tokens[1].c_str());
	
	// Store Mobile Number
	mobNumber = tokens[0];
	
	switch(choice) {

		case 1	: 	// Check if Mobile Number is Registered
					IsNumberRegistered();
					break;
					
		case 2	:	// Check if Service is Activated for Mobile Number
					IsServiceActivated();
					break;
					
		case 3	:	// Process Activation Request for Global Service
					ProcessGlobalActivationRequest();
					break;

		case 4	:	// Process Activation Request for Selective Service 
					ProcessSelectiveActivationRequest(tokens[2]);
					break;
					
		case 5	:	// Process Deactivation Request for Mobile Number
					ProcessDeactivationRequest();
					break;
					
		case 6	:	// Process Update Service Request for Mobile Number
					ProcessUpdateRequest();
					break;
					
		case 7	:	// Process Call to another Client
					ProcessCallRequest(tokens[2]);
					break;					
	
		default	:	cout << "Invalid Choice!" << endl;
					break;				
	}

    return SUCCESS;
}

int Server::IsNumberRegistered() {

	int status = 0, isRegistered = 1;
	string msg;
	char buffer[BUFFERSIZE];
    	string file ="/home/sharatbhat/P/p8/dat/RegisteredClients.txt";
    	const char* clientFile = file.c_str();
    
	// Opening the File
	fd = open(clientFile, O_RDONLY);
	if (fd < 0) {
		perror("FILE OPENING FAil!") ;
        	exit(0);
	}
	
	// Read the file in Blocks of BLOCKSIZE 
	status = read(fd, buffer, BLOCKSIZE);
	if (status < 0) {
		perror("FILE READ FAILURE!");
        	exit(0);
	}	

	stringstream ss(buffer);
	string mnumber;
	
	// Compare Mobile Number with each Number in File
	while (getline(ss, mnumber)) {
		
		if(mobNumber == mnumber) {
			isRegistered = 0;
		}
	}
	
	if (isRegistered == 0) {
		
		// If Number is Registered
		msg = "Mobile Number is already Registered!";
		SendData(const_cast<char*>(msg.c_str()));
	
	} else if (isRegistered == 1) {

		mobNumber.append("\n");
	
		// If Number is Not Registered
		string file ="/home/sharatbhat/P/p8/dat/RegisteredClients.txt";
		WriteFile(const_cast<char*>(mobNumber.c_str()), const_cast<char*>(file.c_str()));
		msg = "Mobile Number is successfully Registered into Database!";
		SendData(const_cast<char*>(msg.c_str()));
	}
	
	// Close File Descriptor
	close(fd);

	return SUCCESS;
}

int Server::IsServiceActivated() {

	int status = 0, isActivated = 1;
	string msg;
	char buffer[BUFFERSIZE] = {0};
	string file = "/home/sharatbhat/P/p8/dat/ActivatedClients.txt";
	const char* clientFile = file.c_str();
	
	// Opening the File
	fd = open(clientFile, O_RDONLY);
	if (fd < 0) {
		perror("FILE OPENING FAILED!");
        exit(0);
	}
	
	// Read the file in Blocks of BLOCKSIZE 
	status = read(fd, buffer, BLOCKSIZE);
	if (status < 0) {
		perror("FILE READ FAILURE!");
		exit(0);
	}	
	
	stringstream ss(buffer);
	string intermediate;
	
	// Get Each Client Record
	while (getline(ss, intermediate)) {
		isActivated = SearchMobNum(intermediate, mobNumber);
	}
	
	// Check if Service is Activated
	if (isActivated == 0) {
		msg = "yes";
		SendData(const_cast<char*>(msg.c_str()));
	} else if (isActivated == 1){
		msg = "no";
		SendData(const_cast<char*>(msg.c_str()));
	}

	return SUCCESS;
}

int Server::SearchMobNum(string data, string mNum) {

	string msg;
	
	// Split the Data into Client Number
	vector<string> clientNum;
	
	stringstream ss(data);
	string intermediate;
	
	while(getline(ss, intermediate, ',')) {
		clientNum.push_back(intermediate);
	}
	
	// Return SUCCESS if Mobile Number exists
	if (clientNum[0] == mNum) {
		return SUCCESS;
	}
	
	return FAILURE;
}

int Server::ProcessGlobalActivationRequest() {

	string msg;
	string file = "/home/sharatbhat/P/p8/dat/ActivatedClients.txt";
	string data = mobNumber + ",G" + "\n";

	WriteFile(const_cast<char*>(data.c_str()), const_cast<char*>(file.c_str()));
	
	msg = "Global Service successfully Activated!";
	SendData(const_cast<char*>(msg.c_str()));

	return SUCCESS;
}

int Server::ProcessSelectiveActivationRequest(string blockNum) {

    	string msg;
	string file = "/home/sharatbhat/P/p8/dat/ActivatedClients.txt";
	string data = mobNumber + ",S," + blockNum + "\n";

	WriteFile(const_cast<char*>(data.c_str()), const_cast<char*>(file.c_str()));
	
	msg = "Selective Service successfully Activated!";
	SendData(const_cast<char*>(msg.c_str()));

	return SUCCESS;
}

int Server::ProcessDeactivationRequest() {

	int status = 0;
	string msg = "";
	string data = "";
	char buffer[BUFFERSIZE] = {0};
	string file = "/home/sharatbhat/P/p8/dat/ActivatedClients.txt";
	const char* clientFile = file.c_str();

	// Clear Previous Records
	clientRecord.clear();
	
	// Opening the File in Read Mode
	fd = open(clientFile, O_RDONLY);
	if (fd < 0) {
		perror("FILE OPENING FAILED!") ;
		exit(0);
	}
	
	// Read the file in Blocks of BLOCKSIZE 
	status = read(fd, buffer, BLOCKSIZE);
	if (status < 0) {
		perror("FILE READ FAILURE!");
        	exit(0);
	}	
	
	stringstream ss(buffer);
	string intermediate;
	
	// Get Each Client Record
	while (getline(ss, intermediate)) {
		clientRecord.push_back(intermediate);
	}
	
	// Delete Record of Client if Mobile Number Exists
	for(unsigned int i = 0; i < clientRecord.size(); i++) {
		if (SearchMobNum(clientRecord[i], mobNumber) == 0) {

			// clientRecord[i].erase();
			clientRecord.erase(clientRecord.begin() + i);
		}
	}
	
	// Close File Descriptor
	close(fd);
	
	// Append Updated Data
	for(unsigned int i = 0; i < clientRecord.size(); i++) {
		data.append(clientRecord[i]);
		data.append("\n");
	}
	
	// Open File in Truncate Mode
	fd = open(clientFile, O_TRUNC);
	if (fd < 0) {
		perror("FILE OPENING FAILED!");
		exit(0);
	}
	
	// Close File Descriptor
	close(fd);
		
	// Write Updated Data to File
	WriteFile(const_cast<char*>(data.c_str()), const_cast<char*>(file.c_str()));
	
	// Send Message to Client
	msg = "DND Service is successfully Deactivated!";
	SendData(const_cast<char*>(msg.c_str()));

	return SUCCESS;
}

int Server::ProcessUpdateRequest() {

	string msg = "";
	
	// Send Message to Client
	msg = "DND Service is successfully Updated!";
	SendData(const_cast<char*>(msg.c_str()));
	
	return SUCCESS;
}

int Server::ProcessCallRequest(string callNum) {
	
	int status = 0, isNumPresent = 1, isActivated = 1;
	string msg = "", serviceType = "";
	char buffer[BUFFERSIZE] = {0};
	string file = "/home/sharatbhat/P/p8/dat/ActivatedClients.txt";
	const char* clientFile = file.c_str();
	
	// Opening the File
	fd = open(clientFile, O_RDONLY);
	if (fd < 0) {
		perror("FILE OPENING FAILED!") ;
		exit(0);
	}
	
	// Read the file in Blocks of BLOCKSIZE 
	status = read(fd, buffer, BLOCKSIZE);
	if (status < 0) {
		perror("FILE READ FAILURE!");
		exit(0);
	}	
	
	stringstream ss(buffer);
	string intermediate;
	
	// Get Each Client Record
	while (getline(ss, intermediate)) {
		isActivated = SearchMobNum(intermediate, callNum);
		
		// Check if DND Service is Activated for Receiver
		if (isActivated == 0) {
		
			// Split the Record
			vector<string> record;
			record.clear();
		
			stringstream st(intermediate);
			string data;
			
			while (getline(st, data, ',')) {
				record.push_back(data);
			}
		
			// Store DND Service Type
			serviceType = record[1];
		
			// Block all Calls if Service Type is Global
			if (serviceType == "G") {
			
				msg = "Call Blocked as '" + callNum + "' Receiver has subscribed for Global DND Services!";
				SendData(const_cast<char*>(msg.c_str()));
				break;
				
			// Block selected Calls if Service Type is Selective
			} else if (serviceType == "S") {
				
				for (unsigned int i = 2; i < record.size(); i++) {
					if (record[i] == mobNumber) {
						isNumPresent = 0;
						msg = "Call Blocked as '" + callNum + "' Receiver has subscribed for Selective DND Services!";
						SendData(const_cast<char*>(msg.c_str()));
						break;
					}
				}
				break;
			}
		}
	}
	
	// Establish Call if DND Service isn't Activated for Receiver
	if (isActivated == 1) {
		
		// Send Message to Caller Client
		msg = "Call Successfully Established with '" + callNum + "' Mobile Number!";
		SendData(const_cast<char*>(msg.c_str()));
	}
	
	// Establish Call if Receiver hasn't listed Caller Number in case of Selective
	if (serviceType == "S" && isNumPresent == 1) {
		
		// Send Message to Caller Client
		msg = "Call Successfully Established with '" + callNum + "' Mobile Number!";
		SendData(const_cast<char*>(msg.c_str()));
	}
	
	return SUCCESS;
}

int Server::WriteFile(char* data, char* fname) {

	int status = 0;
    
	// Opening the File
	fd = open(fname, O_WRONLY|O_APPEND, S_IRWXU);
	if (fd < 0) {
		perror("FILE OPENING FAILED!") ;
	        exit(0);
	}

    // Write to File
    status = write(fd, data, strlen(data));
	if (status < 0) {
	 	perror("FILE WRITE FAILURE!");
	        exit(0);
	}
	
	// Close File Descriptor
	close(fd);
	
    return SUCCESS;
}

Server::~Server() {

	int sstatus = 0;

	// Closing the Socket Descriptor
	sstatus = close(sd);
	if (sstatus < 0) {
		perror("SOCKET CLOSE FAILURE");
		exit(0);
	}

}

// Main Function
int main(int argc, char* argv[]) {
	
	//calling the signal handle function to handle the interruption
	signal(SIGINT, signal_handler);	
	
	// Creating object of Server class
	Server s1;
	
	// Creation of a Socket
	s1.OpenSocket();

	// Bind Data
	s1.BindData();
	
	s1.ReadData();
	
	return SUCCESS;
}
