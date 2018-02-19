/**
* @file ESP8266.c
* @description Driver to easily manage a ESP8266 wifi module with dsPIC
* @author Felipe Gil
* @date 2018-1-8
* @version 0.5 2018-2-16  (ALL AT COMMAND FUNCTIONS)
* @par Copyright:
* Based upon class ESP8266 by Wu Pengfei for Arduino.
* (Copyright (c) 2015 ITEAD Intelligent Systems Co., Ltd.)
*/

#include "ESP8266.h"

//wait time after resetting ESP8266
#define DELAY_RST 2000			//ms
#define DELAY_RST_STABLE 1500	//ms
#define RST_TIMEOUT 3000		//ms
#define DATABUF_TIMEOUT 3000	//ms


/*
*************************************************************
*************** NECESSARY CODE IMPROVEMENTS: ****************
*************************************************************

* Check if the ESP8266 response to AT+CWJAP command
* in the manual is coherent with current AT command functions.
* --> Test the commands with the module and modify functions
* according to actual responses.

*************************************************************
*************************************************************
*/


// ---------------------------------------------------------------
// --------------- PRIVATE FUNCTIONS PROTOTYPES ------------------
// ---------------------------------------------------------------


///------------- Character Strings Handling Functions  -----------------------

/**

	* INITIAL COMMENT: C does not support booleans by default,
	* so all boolean variables in original library have been 
	* replaced by "int". Be careful because this may result confusing.
	*/

/**
	* Empty the UART2 reception queue.
	*/
void emptyUART2queue(void);
/**
	* Receive data from uart and search a string "target".
	* When target is found or timeout is reached, all received
	* data are returned by overwriting target.
	* The function returns a coonfirmation (1 - target found, 0 - timeout reached / other error).
	* NOTE: All strings must end with '\0'
	*/
int recvString(char *target, uint32_t timeout = 1000);

/**
	* The same as recvString, but now, there are 2 target strings.
	* Data is returned by overwriting target1.
	*/
int recv2Strings(char *target1, char *target2, uint32_t timeout = 1000);

/**
	* The same as recv2Strings, but now, there are 3 target strings.
	* Data is returned by overwriting target1.
	*/
int recv3Strings(char *target1, char *target2,	char *target3, uint32_t timeout = 1000);

/** 
	* Receive data from uart and search first target. 
	* Return true if target found before timeout.
	*/
int recvFind(char *target, uint32_t timeout = 1000);

/**
	* Receive data from uart, search target and cut out the substring 
	* between begin and end (excluding begin and end themselves). 
	* Return true if target found, false if timeout reached.
	* WARNING: end must be after begin within the string. If end is found
	* BEFORE begin, data will return all the characters received through the UART
	* from "begin"
	*/
int recvFindAndFilter(char *target, char *begin, char *end, char *data, uint32_t timeout = 1000);

/**
	* Receive network data (+IPD command).
	* When the ESP8266 module receives network data, it will
	* send the data through the serial port using the +IPD command
	*
	* @param buffer - buffer which stores the data
	* @param buffer_size - specify the size of the buffer (number of characters)
	* @param data_len - length of data actually received(if bigger than buffer_size, remaining data will be abandoned).
					  - provide a NOT NULL value in this field to return the data_len by reference
	* @param timeout
	* @param coming_mux_id - single connection mode: NULL; multiple connection: not NULL. 
	* 
	* @return uint32_t Lenght of the data actually received (0: error/no data returned)
	*/
uint32_t recvPkg(uint8_t *buffer, uint32_t buffer_size, 
	uint32_t *data_len, uint32_t timeout, uint8_t *coming_mux_id);

///------------  AT-COMMAND FUNCTIONS PROTOTYPES  ----------------

/// Basic AT Commands

//test AT startup
int eAT(void);
//restart the module
int eATRST(void);
//check version information
int eATGMR(char *version);

/// Wi-Fi AT Commands

//check current WiFi mode (Station/SoftAP/Station+SoftAP)
int qATCWMODE(uint8_t *mode);
//set WiFi mode
int sATCWMODE(uint8_t mode);
//connect to an AP
int sATCWJAP(char *ssid, char *pwd);
//enable/disable DHCP
int sATCWDHCP(uint8_t mode, int enabled);
//list available APs
int eATCWLAP(char *list);
//disconnect from the AP
int eATCWQAP(void);
//configure the ESP8266 SoftAP
int sATCWSAP(char *ssid, char *pwd, uint8_t chl, uint8_t ecn);
//list the IP of stations to which the ESP8266 SoftAP is connected
int eATCWLIF(char *list);

/// TCP/IP-Related AT Commands

//get the connection status 
//(connected to AP or not, TCP/UDP transmission created or not)
int eATCIPSTATUS(char *list);

//start single TCP/UDP/SSL connection
int sATCIPSTARTSingle(char *type, char *addr, uint32_t port);

//start multiple TCP/UDP connections
//(ESP8266 can only set one SSL connection)
int sATCIPSTARTMultiple(uint8_t mux_id, char *type, char *addr, uint32_t port);

//send data of length "len", for a single connection
int sATCIPSENDSingle(const uint8_t *buffer, uint32_t len);

//send data of length "len", through the connection specified by "mux_id"
int sATCIPSENDMultiple(uint8_t mux_id, const uint8_t *buffer, uint32_t len);

//close a connection when in multiple connection mode
//(mux_id = 5 closes all connections)
int sATCIPCLOSEMulitple(uint8_t mux_id);

//close the connection when in single connection mode
int eATCIPCLOSESingle(void);

//get local IP Address
int eATCIFSR(char *list);

//enable or disable multiple connections
//(mode = 0 -> single connection)
//(mode = 1 -> multiple connections)
/**	NOTES:
	* This mode can only be changed after all connections
	* have been disconnected. 
	* Multiple connections can only be set when transparent 
	* transmission is disabled (AT+CIPMODE=0).
*/
int sATCIPMUX(uint8_t mode);

//delete/create TCP server
//(mode = 0 deletes server, mode = 1 creates server)
int sATCIPSERVER(uint8_t mode, uint32_t port = 333);

//set TCP server timeout
int sATCIPSTO(uint32_t timeout);

// ---------------------------------------------------------------
// ---------------------- PUBLIC FUNCTIONS -----------------------
// ---------------------------------------------------------------

int kick(void)
{
	return eAT();
}

//possibility of programming the timer directly
//within this module, whithoout a specific driver
//(commented lines)
int restart(void)
{
	int success = 0;
	if(eATRST()){
		// iniTimer2(DELAY_RST);
		// while(!IFS0bits.T2IF);
		// IFS0bits.T2IF = 0;
		delay(DELAY_RST);
		iniTimer2(RST_TIMEOUT);
		while(!endTimer2() && !success){
			if(eAT()){
				//iniTimer3(DELAY_RST_STABLE);
				//while(!IFS0bits.T3IF);
				//IFSObits.T3IF = 0;
				delay(DELAY_RST_STABLE);
				success = 1;
			}
			//iniTimer3(100);
			//while(!IFS0bits.T3IF);
			//IFS0bits.T3IF = 0;
			delay(100);
		}
	}
	return success;
}

void getVersion(char *version)
{
	eATGMR(version);
	return;
}

// ---------------------------------------------------------------
// ---------------------- PRIVATE FUNCTIONS ----------------------
// ---------------------------------------------------------------

void emptyUART2queue(void)
{
	char a;
	a = getcharUART2();
	while(a != '\0'){
		a = getcharUART2();
	}
}
int recvString(char *target, uint32_t timeout)
{
	char a;//each character received via UART
	char data[MAX_STR_LEN];//data received via UART
	uint16_t idata = 0;	//data iterator
	uint16_t found = 0;	//turns to 1 if target is found
	//pointer to the result of searching 
	//the target in the received data
	//char *result = NULL;

	//1. Start timeout
	iniTimer2(timeout);
	//2. Start receiving and searching for target
	while(!endTimer2() && !found && idata < MAX_STR_LEN){
		a = getcharUART2();
		if(a != '\0'){
			data[idata] = a;
			idata++;
			//search target within data
			if(strstr(data,target) != NULL){
				found = 1;
				data[idata] = '\0';
			}
		}

	}
	//3. Copy received data to "target"
	strcpy(target,data);
	return found;
}


int recv2Strings(char *target1, char *target2, uint32_t timeout)
{
	char a = '0';//each character received via UART
	char data[MAX_STR_LEN];//data received via UART
	uint16_t idata = 0;	//data iterator
	uint16_t found = 0;	//turns to 1 if target is found
	//pointer to the result of searching 
	//the target in the received data
	//char *result = NULL;

	//1. Start timeout
	iniTimer2(timeout);
	//2. Start receiving and searching for target
	while(!endTimer2() && !found && idata < MAX_STR_LEN){
		a = getcharUART2();
		if(a != '\0'){
			data[idata] = a;
			idata++;
			//search target within data
			if(strstr(data,target1) != NULL){
				found = 1;
			}else if(strstr(data,target2) != NULL){
				found = 1;
			} 
		}

	}
	//3. Copy received data to "target1"
	strcpy(target1,data);
	return found;
}

int recv3Strings(char *target1, char *target2,	char *target3, uint32_t timeout)
{
	char a = '0';//each character received via UART
	char data[MAX_STR_LEN];//data received via UART
	uint16_t idata = 0;	//data iterator
	uint16_t found = 0;	//turns to 1 if target is found
	//pointer to the result of searching 
	//the target in the received data
	//char *result = NULL;

	//1. Start timeout
	iniTimer2(timeout);
	//2. Start receiving and searching for target
	while(!endTimer2() && !found && idata < MAX_STR_LEN){
		a = getcharUART2();
		if(a != '\0'){
			data[idata] = a;
			idata++;
			//search target within data
			if(strstr(data,target1) != NULL){
				found = 1;
			}else if(strstr(data,target2) != NULL){
				found = 1;
			}else if(strstr(data,target3) != NULL){
				found = 1;
			}
		}

	}
	//3. Copy received data to "target1"
	strcpy(target1,data);
	return found;
}

int recvFind(char *target, uint32_t timeout){
	char data_tmp[MAX_STR_LEN];
	int found = 0;

	//make a copy of the target to receive from UART
	strcpy(data_tmp,target);
	found = recvString(data_tmp,timeout);
	return found;
}

int recvFindAndFilter(char *target, char *begin, char *end, char *data, uint32_t timeout = 1000)
{
	char data_tmp[MAX_STR_LEN];
	int correct = 0;
	int target_found = 0;
	//pointers to locate begin and end
	char *index1;
	char *index2;
	int idata = 0;	//"iterator" for data
	//make a copy of the target to receive from UART
	strcpy(data_tmp,target);
	target_found = recvString(data_tmp,timeout);
	//check if target has been found,
	//and cut between begin and end
	if(target_found){
		index1 = strstr(data_tmp,begin);
		index2 = strstr(data_tmp,end);
		//check if begin and end have been found
		if(index1 != NULL && index2 != NULL){
			//move pointer AFTER begin
			//(because the "begin" string is not included)
			index1 += strlen(begin);
			//fill "data"
			/*while(index1 != index2 && (*data_tmp) != '\0'){
				data[idata] = *data_tmp;
				index1++;
				data_tmp++;
				idata++;
			}*/
			correct = substring(data_tmp,data,index1,index2);
			//data[idata] = '\0';
		}else{	//begin or end not found
			*data = '\0';
			correct = 0;
		}
	}else{	//target not found
		*data = '\0';
		correct = 0;
	}
	return correct;
}

uint32_t recvPkg(uint8_t *buffer, uint32_t buffer_size, 
	uint32_t *data_len, uint32_t timeout, uint8_t *coming_mux_id)
{
	char data[20];	//to store +IPD command header
	char a;
	//indexes to search key characters in received data
	char *index_PIPDcomma = NULL;
	char *index_colon = NULL;
	char *index_comma = NULL;
	char *charlen = NULL;	//lenght of message in string format
	char *charid = NULL;	//id in string format
	int32_t len = -1;		//length in integer form
	int16_t id = -1;		//id in integer form
	int has_data = 0;		//package with correct header detected
	uint32_t ret = 0;			//size of the data returned (#characters)
	unsigned long start;
	uint32_t i;
	int idata = 0;	//data iterator
	//logical variables to check if data correct:
	int id_read = 0;
	int id_correct = 0;
	int len_read = 0;
	int len_correct = 0;

	if(buffer != NULL){
		// I. RECEIVE DATA

		iniTimer2(timeout);	//1. Start timeout
		//2. Start receiving and searching for target
		while(!endTimer2() && !has_data && idata < 20){
			a = getcharUART2();
			if(a != '\0'){
				data[idata] = a;
				idata++;
			}
			index_PIPDcomma = strstr(data,"+IPD,");
			if(index_PIPDcomma != NULL){
				index_colon = strstr(index_PIPDcomma + 5, ':');
				if(index_colon != NULL){
					index_comma = strstr(index_PIPDcomma + 5, ',');
					//if network data command with format: "+IPD,<id>,<len>:<data>"
					if(index_comma != NULL && index_comma < index_colon){
						//get the ID:
						id_read = substring(data,charid,index_PIPDcomma + 5, index_comma);
						if(id_read){
							id = stringToUint(charid,&id_read);
							if(id_read && id >= 0 && id <= 4)	id_correct = 1;
						}
						len_read = substring(data,charlen,index_comma + 1, index_colon);
						if(len_read){
							len = stringToUint(charlen,&len_read);
							if(len_read && len > 0)	len_correct = 1;
						}
					}else{//network command format: "+IPD,<len>:<data>""
						len_read = substring(data,charlen,index_PIPDcomma + 5, index_colon);
						if(len_read){
							len = stringToUint(charlen,&len_read);
							if(len_read && len > 0)	len_correct = 1;
						}
					}
					has_data = 1;
				}//colon detected

			}//"+IPD" detected
		}//timer loop

		if(has_data && len_correct){//frame header detected: start reading data
			i = 0;
			ret = len > buffer_size ? buffer_size : len;
			iniTimer2(DATABUF_TIMEOUT);
			while(!endTimer2()){
				a = getcharUART2();
				while(a != '\0'){
					buffer[i++] = a;
					a = getcharUART2();
				}
				if(i == ret){
					emptyUART2queue();
					if(data_len){
						*data_len = len;
					}
					if(index_comma != NULL && coming_mux_id){
						*coming_mux_id = id;
					}
				}	//if("last character")
			}
		}	//if("frame detected")

	}	//if("memory buffer exists")
	return ret;
}


/// Basic AT Commands

//test AT startup
int eAT(void)
{
	emptyUART2queue();
	putsUART2("AT\0");
	return recvFind("OK");
}

//restart the module
int eATRST(void)
{
	emptyUART2queue();
	putsUART2("AT+RST\0");
	return recvFind("OK");
}
//check version information
int eATGMR(char *version)
{
	emptyUART2queue();
	putsUART2("AT+GMR\0");
	return recvFindAndFilter("OK","\r\r\n", "\r\n\r\nOK",version);
}

/// Wi-Fi AT Commands

//check current WiFi mode (Station/SoftAP/Station+SoftAP)
int qATCWMODE(uint8_t *mode)
{
	//"+CWMODE:<mode>\r\n\r\nOK"
	char str_mode[20];	//parameter <mode> (character)
	int ret;	//confirmation of command response detected
	int confirm = 0;	//to return (1 if mode correctly read)
	//only return something if mode != 0
	if(mode){
		emptyUART2queue();
		putsUART2("AT+CWMODE?");	//send command
		//receive and read response
		ret = recvFindAndFilter("OK","+CWMODE:", "\r\n\r\nOK", str_mode);
		if(ret){
			//convert the read mode to integer
			//writes 1 to "confirm" if correct
			*mode = (uint8_t) stringToUint(str_mode,&confirm);
		}
	}
	return confirm;
}

//set WiFi mode
int sATCWMODE(uint8_t mode)
{
	char data[20];	//ESP8266 response
	char command[15];
	int confirm = 0;

	emptyUART2queue();

	sprintf(command,"AT+CWMODE=%u",mode);
	putsUART2(command);	//send command
	//receive and read response
	data = recv2Strings("OK","no change");
	if(strstr(data,"OK") != NULL || strstr(data,"no change") != NULL){
		confirm = 1;
	}
	return confirm;
}

//connect to an AP
int sATCWJAP(char *ssid, char *pwd)
{
	//this variable will store target1 ("OK")
	//and ESP8266 response after received
	char data[30] = "OK";	
	char command[30];
	int correct = 0;
	int resp_ok = 0;

	emptyUART2queue();
	sprintf(command,"AT+CWJAP=\"%s\",\"%s\"",ssid,pwd);
	putsUART2(command);
	//receive and analyze response
	resp_ok = recv2Strings(data,"FAIL",10000);
	if(resp_ok){
		if(strstr(data,"OK") != NULL){
			correct = 1;
		}else{
			//to be determined
			//maybe return error code
		}
	}
	return correct;
}

//enable/disable DHCP
int sATCWDHCP(uint8_t mode, int enabled)
{
	
	char command[30];
	char data[30] = "OK";
	int correct = 0;
	int resp_ok = 0;

	emptyUART2queue();
	sprintf(command,"AT+CWDHCP=%u,%u",enabled,mode);
	putsUART2(command);
	//receive and analyze response
	resp_ok = recv2Strings(data,"FAIL",10000);
	if(resp_ok){
		if(strstr(data,"OK") != NULL){
			correct = 1;
		}else{
			//to be determined
			//maybe return error code
		}
	}
	return correct;
}
//list available APs
int eATCWLAP(char *list)
{
	char command[10] = "AT+CWLAP";
	emptyUART2queue();
	putsUART2(command);
	return recvFindAndFilter("OK","\r\r\n","\r\n\r\nOK",list,10000);
}
//disconnect from the AP
int eATCWQAP(void)
{
	emptyUART2queue();
	putsUART2("AT+CWQAP");
	return recvFind("OK");
}
//configure the ESP8266 SoftAP
int sATCWSAP(char *ssid, char *pwd, uint8_t chl, uint8_t ecn)
{
	char command[30];
	char data[30] = "OK";
	int correct = 0;
	int resp_ok = 0;

	emptyUART2queue();
	sprintf(command, "AT+CWSAP=\"%s\",\"%s\",%u,%u",ssid,pwd,chl,ecn);
	//receive and analyze response
	resp_ok = recv2Strings(data,"FAIL",10000);
	if(resp_ok){
		if(strstr(data,"OK") != NULL){
			correct = 1;
		}else{
			//to be determined
			//maybe return error code
		}
	}
	return correct;
}
//list the IP of stations to which the ESP8266 SoftAP is connected
int eATCWLIF(char *list)
{
	emptyUART2queue();
	putsUART2("AT+CWLIF");
	return recvFindAndFilter("OK", "\r\r\n", "\r\n\r\nOK", list);
}

/// TCP/IP-Related AT Commands

//get the connection status 
//(connected to AP or not, TCP/UDP transmission created or not)
int eATCIPSTATUS(char *list)
{
	delay(100);
	emptyUART2queue();
	putsUART2("AT+CIPSTATUS");
	return recvFindAndFilter("OK", "\r\r\n", "\r\n\r\nOK", list);
}

//start single TCP/UDP/SSL connection
int sATCIPSTARTSingle(char *type, char *addr, uint32_t port)
{
	char command[30];
	char data[20] = "OK";
	int correct = 0;
	int resp_ok = 0;

	emptyUART2queue();
	sprintf(command, "AT+CIPSTART=\"%s\",\"%s\",%u",type,addr,port);
	putsUART2(command);

	resp_ok = recv3Strings(data,"ERROR","ALREADY CONNECT", 10000);
	if(resp_ok){
		if(strstr(data,"OK") != NULL || strstr(data,"ALREADY CONNECT") != NULL){
			correct = 1;
		}
	}
	return correct;
}

//start multiple TCP/UDP connections
//(ESP8266 can only set one SSL connection)
int sATCIPSTARTMultiple(uint8_t mux_id, char *type, char *addr, uint32_t port)
{
	char command[30];
	char data[20] = "OK";
	int correct = 0;
	int resp_ok = 0;

	emptyUART2queue();
	sprintf(command, "AT+CIPSTART=%u,\"%s\",\"%s\",%u",mux_id,type,addr,port);
	putsUART2(command);

	resp_ok = recv3Strings(data,"ERROR","ALREADY CONNECT", 10000);
	if(resp_ok){
		if(strstr(data,"OK") != NULL || strstr(data,"ALREADY CONNECT") != NULL){
			correct = 1;
		}
	}
	return correct;
}

//send data of length "len", for a single connection
int sATCIPSENDSingle(const uint8_t *buffer, uint32_t len)
{
	char command[20];
	int correct = 0;
	int i = 0;	//buffer iterator


	emptyUART2queue();
	sprintf(command, "AT+CIPSEND=%u",len);
	putsUART2(command);
	if(recvFind(">",5000)){
		emptyUART2queue();
		while(i < len){
			//test if this check is necessary
			if(!UART2TXQueueFull()){
				//is this correct?
				ponerEnColaTransmisionUART2(buffer[i]);
				transmitirUART2();
				i++;
			}else{
				transmitirUART2();
			}
		}
		correct = recvFind("SEND OK", 10000);
	}
	return correct;
}

//send data of length "len", through the connection specified by "mux_id"
int sATCIPSENDMultiple(uint8_t mux_id, const uint8_t *buffer, uint32_t len)
{
	char command[20];
	int correct = 0;
	int i = 0;	//buffer iterator


	emptyUART2queue();
	sprintf(command, "AT+CIPSEND=%u,%u",mux_id,len);
	putsUART2(command);
	if(recvFind(">",5000)){
		emptyUART2queue();
		while(i < len){
			//test if this check is necessary
			if(!UART2TXQueueFull()){
				//is this correct?
				ponerEnColaTransmisionUART2(buffer[i]);
				transmitirUART2();
				i++;
			}else{
				transmitirUART2();
			}
		}
		correct = recvFind("SEND OK", 10000);
	}
	return correct;	
}

//close a connection when in multiple connection mode
//(mux_id = 5 closes all connections)
int sATCIPCLOSEMulitple(uint8_t mux_id)
{
	char data[30] = "OK";
	char command[20];
	int correct = 0;	//1:connection successfully closed

	emptyUART2queue();
	sprintf(command,"AT+CIPCLOSE=%u",mux_id);
	putsUART2(command);
	correct = recv2Strings(data, "link is not", 5000);
	if(correct){
		correct = 0;
		if(strstr(data, "OK") != NULL || strstr(data,"link is not") != NULL){
			correct = 1;
		}
	}
	return correct;
}

//close the connection when in single connection mode
int eATCIPCLOSESingle(void)
{
	emptyUART2queue();
	putsUART2("AT+CIPCLOSE");
	return recvFind("OK",5000);
}

//get local IP Address
int eATCIFSR(char *list)
{
	emptyUART2queue();
	putsUART2("AT+CIFSR");
	return recvFindAndFilter("OK","\r\r\n","\r\n\r\nOK",list);
}
//enable or disable multiple connections
//(mode = 0 -> single connection)
//(mode = 1 -> multiple connections)
/**	NOTES:
	* This mode can only be changed after all connections
	* have been disconnected. 
	* Multiple connections can only be set when transparent 
	* transmission is disabled (AT+CIPMODE=0).
*/
int sATCIPMUX(uint8_t mode)
{
	char data[20]  = "OK";
	char command[20];
	int correct;

	emptyUART2queue();
	sprintf(command,"AT+CIPMUX=%u",mode);
	putsUART2(command);

	correct = recv2Strings(data,"Liink is builded");
	if(correct){
		correct = 0;
		if(strstr(data,"OK") != NULL){
			correct = 1;
		}
	}
	return correct;
}

//delete/create TCP server
//(mode = 0 deletes server, mode = 1 creates server)
int sATCIPSERVER(uint8_t mode, uint32_t port)
{
	char data[20]  = "OK";
	char command[20];
	int correct;
	if(mode){	//create server
		emptyUART2queue();
		sprintf(command,"AT+CIPSERVER=1,%u",port);
		putsUART2(command);
		//check response
		correct = recv2Strings(data,"no change");
	}	else{
		emptyUART2queue();
		sprintf(command,"AT+CIPSERVER=0");
		putsUART2(command);
		correct = recvFind("\r\r\n");
	}
	return correct;
}

//set TCP server timeout
int sATCIPSTO(uint32_t timeout)
{
	char command[20];

	emptyUART2queue();
	sprintf(command,"AT+CIPSTO=%u",timeout);
	putsUART2(command);
	return recvFind("OK");
}