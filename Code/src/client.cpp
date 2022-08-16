#include "client.h"

void signal_handler(int sig) {
	
    	cout << "Ctrl+C Sent: " << endl << "Client EXIT..."<<endl;  
    	exit(EXIT_FAILURE);
}

bool IsNumber(const string & str) {
    
    	// std::find_first_not_of searches the string for the first character 
    	// that does not match any of the characters specified in its arguments

    	return !str.empty () && 
    	(str.find_first_not_of ("[0123456789]") == std::string::npos);	
}


bool IsValidMobileNum(string mobNum) {
    

    	int len = 0;
    	
    	// Verify that Mobile Number contains digit or not
    	if (!IsNumber(mobNum)) { 
        return false;
    }
    
    	// Mobile number should be of 10 digits
    	len = strlen(mobNum.c_str());
    	if (len != 10) {
        return false;
    }
    
    	// Mobile number should start from 7,8 or 9
    	if (mobNum[0] < '7') { 
        return false;    
    }
    

   	 return true;
}

Client::Client() {

	sd = 0;
	mobileNumber = "";
}

Client::Client(string IpAddress, string PortNo, string mobNum) {
    
	//fstream logfile;  
	//logfile.open("/home/sharatbhat/P/p8/dat/clientlog.txt", ios::out|ios::app);

	// Populate the Server Address structure
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(atoi(PortNo.c_str())); 	// PORT NO
	serverAddress.sin_addr.s_addr = inet_addr(IpAddress.c_str());	// IP ADDRESS	
	mobileNumber = mobNum;	// MOBILE NUMBER
}



int Client::OpenSocket() {

	sd = socket(AF_INET,SOCK_DGRAM, IPPROTO_UDP);
	if (sd < 0) {
		perror("SOCKET OPEN FAILURE");
		exit(0);
	}
	
	return SUCCESS;
}

int Client::IsMobNumRegistered(string mobNum) {

	mobileNumber = mobNum;
	
	// '1' if Mobile Number is Registered or Not
	mobNum += ":1";
	
	SendData(const_cast<char*>(mobNum.c_str()));

	cout << "Successfully Connected to the Server!" << endl;
	
	// Read the Data sent by Server
	ReadData();

	return SUCCESS;
}

	
int Client::SendData(char* data) {

	int count = 0, bufferLen = 0;
	char buffer[BUFFERSIZE];
	
	strcpy(buffer, data);
	bufferLen = strlen(buffer);
	
	// Send Data
	count = sendto(sd, buffer, bufferLen, 0,(struct sockaddr*)&serverAddress,server_struct_len);
	if (count < 0 ) {
		perror("DATA NOT SENT TO SERVER");
		close(sd);
		exit(0);
	}
	return SUCCESS;
}

int Client::ReadData() {
	
    	//socklen_t length;
	char buffer[BUFFERSIZE];
	
	//length = sizeof(serverAddress);
	int count = 0;

	// Receive Data
	count = recvfrom(sd, buffer, BLOCKSIZE, 0,(struct sockaddr*)&serverAddress,(socklen_t*)&server_struct_len);
	if (count <= 0) {
		perror("DATA NOT RECEIVED FROM SERVER");
		close(sd);

		exit(0);
	}

	buffer[count] = '\0';
	
	// cout << "Data Received from Server: " << buffer <<  endl;
	cout << buffer << endl;
	return SUCCESS;
}

int Client::MainMenu() {
	
	// Choice
	int choice = 0;
	
	// Exit Message
	string msg = "Exit";
	
	// Performing Operation as per Client Choice
	do {
	    
	    cout << endl;
	    cout << "************ MENU ************" << endl << endl;

	    cout << "1. ACTIVATE DND SERVICE" << endl;
	    cout << "2. DEACTIVATE DND SERVICE" << endl;
	    cout << "3. UPDATE DND SERVICE" << endl;
	    cout << "4. CALL ANOTHER CLIENT" << endl;
	    cout << "5. EXIT" << endl << endl;

	    cout << "******************************" << endl << endl;
	
	    cout << "Hi, which service would you like to avail? (1-5) : ";
	    cin >> choice;
	    
	    cout << endl;
	
	    switch(choice) {
	    
	        case 1	:   // cout << "Activate" << endl;
	                    ActivateService();
	                    break;
						
	        case 2	:   // cout << "Deactivate" << endl;
	                    DeactivateService();
	                    break;
	                    
	        case 3	:   // cout << "Update" << endl;
	                    UpdateService();
	                    break;
	                    
	        case 4	:   // cout << "Call" << endl;
		      	    CallClient();
	                    break;
	                    
	        case 5	:   SendData(const_cast<char*>(msg.c_str()));
			    break;
	                    
	        default :   cout << "Invalid Choice!" << endl; 
	                    break;
	    
	    }
	    
	} while (choice != 5);
	
	return SUCCESS;
}

int Client::IsServiceActivated() {
	
	string mobNum = "";
	char data[BUFFERSIZE];
    
	// '2' if Service is Activated or Not
	mobNum = mobileNumber + ":2";
	
	// Check at Server Side in Database
	// If Service is already Activated for the Mobile Number 
	SendData(const_cast<char*>(mobNum.c_str()));
	
	// Receive Data from Server
	int count = recvfrom(sd, data, BLOCKSIZE, 0,(struct sockaddr*)&serverAddress,(socklen_t*)&server_struct_len);
	if (count <= 0) {
		perror("DATA NOT RECEIVED FROM SERVER");
		close(sd);
		exit(0);
	}
	data[count] = '\0';

	// cout << "Yes/No : " << data << endl;
	if (strcmp(data, "yes") == 0) {
		return SUCCESS;
		
	} else if (strcmp(data, "no") == 0) {
		return FAILURE;
	}
	
	return FAILURE;
}

int Client::ActivateService() {

	int serviceType = 0;
	string mobNum = "";
        int isActivated = 1;

	// Check if Service is Activated
	isActivated = IsServiceActivated();
	
	// Exit if Service is Already Activated
	if (isActivated == 0) {
		cout << "Can't Proceed as Service is Already Activated!" << endl;
		return FAILURE;

	} else {
		
		// Else Display Menu and Proceed Further
		cout << "***** TYPES OF SERVICES *****" << endl;
		cout << "1. GLOBAL DND" << endl;
		cout << "2. SELECTIVE DND" << endl;
		cout << "*****************************" << endl << endl;
			
		cout << "Which type of Service would you like to select?" << endl;
		cout << "Press 1 for Global and 2 for Selective : ";
		cin >> serviceType;

		while(serviceType < 1 || serviceType > 2) { 
			cout << "Invalid Choice! Please Enter Again (1/2) : ";
			cin >> serviceType;
		}
		
		cout << endl;
		
		// '3' if Global Service Activation is Requested
		mobNum = "";
		mobNum = mobileNumber + ":3";
		
		char service[BUFFERSIZE] = {0};
		string ch = "";
		string blockNum = "", num = "";

		switch (serviceType) {
						
			
			case 1	:	// Send Activation Request of 'Global Service' to Server		
					strcpy(service, mobNum.c_str());
					SendData(service);
					ReadData();
	                    		break;

			case 2	:	// Send Activation Request of 'Selective Service' to Server
						do {
							
							// Input from the user the number he wishes to Block
							cout  << "Enter the Number you want to Block: ";
							cin >> num;
				
							while(!IsValidMobileNum(num)) {	
								cout << "Invalid Mobile Number!" << endl;
								cout << "Please Enter a Valid Number: ";
								cin >> num;
							}	
						
							blockNum.append(num);
						
							cout << "Do you want to Block another number?(y/n) : ";
							cin >> ch;
							
							while (ch != "y" && ch != "n") {
								cout << "Invalid Choice! Please Enter y or n : ";
								cin >> ch;
							}
							
							if (ch == "y") {
								blockNum.append(",");
							}

						} while (ch != "n");
						
						// '4' if Selective Service Activation is Requested
						mobNum = "";
						mobNum = mobileNumber + ":4" + ":" + blockNum;
						SendData(const_cast<char*>(mobNum.c_str()));
						ReadData();
						break;
			
			default :   cout << "Invalid Choice!" << endl;
						break;
		}
	} 
	return SUCCESS;
}

int Client::DeactivateService() {
	
	string data = "";
	string msg = "";
	int isActivated = 0;

	// Check if Service is Activated
	isActivated = IsServiceActivated();
	
	// Exit if Service is Already Deactivated
	if (isActivated == 1) {
		cout << "Can't Proceed as Service is Already Deactivated!" << endl;
		return FAILURE;

	} else {
		
		// '5' if Deactivation of Service is Requested
		msg = mobileNumber + ":5";
		SendData(const_cast<char*>(msg.c_str()));
		ReadData();
	}
	
	return SUCCESS;
}

int Client::UpdateService() {

	string msg = "";
	int isDeactivated = 1;
	
	// Deactivate Service
	isDeactivated = DeactivateService();

	if (isDeactivated == 0) {
	
		// Activate Service
		ActivateService();

		// '6' if Update of Service is Requested
		msg = mobileNumber + ":6";
		SendData(const_cast<char*>(msg.c_str()));
		ReadData();
	}

    return SUCCESS;
}

int Client::CallClient() {

	string msg = "";
	string callNum = "";
	
	// Input from the user the number he wishes to Call
	cout << "Enter which Number you want to Call: ";
	cin >> callNum;
	
	while(!IsValidMobileNum(callNum)) {	
		cout << "Invalid Mobile Number!" << endl;
		cout << "Please Enter a Valid Number: ";
		cin >> callNum;
	}	
	
	// '7' if Call to Client is Requested
	msg = mobileNumber + ":7" + ":" + callNum;
	SendData(const_cast<char*>(msg.c_str()));
	ReadData();
	
	return SUCCESS;
}

Client::~Client() {

	int status = 0;
	
	// Closing the Socket Descriptor
	close(sd);
	if (status < 0) {
	    perror("SOCKET CLOSE FAILURE");

	    exit(0);
	}
}
int main(int argc, char* argv[]) {
	
	signal(SIGINT, signal_handler); 

    	// Validate Number of Inputs
    	if (argc < 4) {
        cerr << "Invalid Arguments Passed!" << endl << "Usage: ./<Program_Name> <IP_Address> <Port_No> <Mobile_Number>" << endl;
		exit(0);
    	}
	
    	// Validate Mobile Number
    	if (!IsValidMobileNum(argv[3])) {

        cerr << "Invalid Mobile Number!" << endl;
		exit(3);
   	 }
		
    	// Creating object of Client class
    	// argv[1] = IP Address, argv[2] = Port Number, argv[3] = Mobile Number
    	Client c1(argv[1], argv[2], argv[3]);	
		
    	// Creation of a Socket
    	c1.OpenSocket();
	
 	c1.IsMobNumRegistered(argv[3]);
	
    	c1.MainMenu();

    	return SUCCESS;	
}
