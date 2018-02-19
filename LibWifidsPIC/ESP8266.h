/**
* @file ESP8266.h
* @description Driver to easily manage a ESP8266 wifi module with dsPIC
* @author Felipe Gil
* @date 2018-1-5
* @version 0.4 2018-2-16
* @par Copyright:
* Based upon class ESP8266 by Wu Pengfei for Arduino.
* (Copyright (c) 2015 ITEAD Intelligent Systems Co., Ltd.)
*/

#ifndef _ESP8266_H
#define _ESP8266_H

#include "DriverPicTrainer/uart2.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "timers.h" //manage dsPIC timers for delays and so on
#include "advancedstring.h" //substring and stringToInt functions

#define MAX_STR_LEN 250

// --------------------------------------------------------------
// ---------------- PUBLIC FUNCTIONS PROTOTYPES -----------------
// --------------------------------------------------------------

/**
	 * Send command AT to check if module available.
	 * @return 1 - alive.
	 * @return 0 - dead.
	 */
	int kick(void);

/**
	 * Restart ESP8266 by "AT+RST". 
	 *
	 * This method will take 3 seconds or more. 
	 *
	 * @return 1 - success.
	 * @return 0 - failure.
	 */
	int restart(void);

/**
     * Get the version of AT Command Set. 
     * 
     * @param version: string returned by reference (end with \0)
     */
	void getVersion(char *version);

/**
     * Set operation mode to staion. 
     * 
     * @return 1 - success.
     * @return 0 - failure.
     */
	int setOprToStation(void);

/**
     * Set operation mode to softap. 
     * 
     * @return 1 - success.
     * @return 0 - failure.
     */
    int setOprToSoftAP(void);

/**
     * Set operation mode to station + softap. 
     * 
     * @return 1 - success.
     * @return 0 - failure.
     */
    int setOprToStationSoftAP(void);
    
    /**
     * Search available AP list and return it.
     * 
     * @return the list of available APs. 
     * @note This method will occupy a lot of memeory(hundreds of Bytes to a couple of KBytes). 
     *  Do not call this method unless you must and ensure that your board has enough memery left.
     */
    //String getAPList(void);
    //Not implemented because it requires a lot of memory
    
    /**
     * Join in AP. 
     *
     * @param ssid - SSID of AP to join in. 
     * @param pass - Password of AP to join in. 
     * @return 1 - success.
     * @return 0 - failure.
     * @note This method will take a couple of seconds. 
     */
    int joinAP(char *ssid, char *pass);
    
    
    /**
     * Enable DHCP for client mode. 
     *
     * @param mode - server mode (0=soft AP, 1=station, 2=both)
     * @param enabled - 1 if dhcp should be enabled, otherwise 0
     * 
     * @note This method will enable DHCP but only for client mode!
     */
    int enableClientDHCP(uint8_t mode, int enabled);
    
    /**
     * Leave AP to which device is connected. 
     *
     * @return 1 - success.
     * @return 0 - failure.
     */
    int leaveAP(void);
    
    /**
     * Set SoftAP parameters. 
     * 
     * @param ssid - SSID of SoftAP. 
     * @param pass - PASSWORD of SoftAP. 
     * @param chl - the channel (1 - 13, default: 7). 
     * @param enc - the way of encryption (0 - OPEN, 1 - WEP, 
     *  2 - WPA_PSK, 3 - WPA2_PSK, 4 - WPA_WPA2_PSK, default: 4). 
     * @note This method should not be called when station mode. 
     */
    int setSoftAPParam(char *ssid, char *pass, uint8_t chl = 7, uint8_t enc = 4);
    
    /**
     * Get the IP list of devices connected to SoftAP. 
     * 
     * @return the list of IP.
     * @note This method should not be called when station mode. 
     */
    //String getJoinedDeviceIP(void);
    //Not implemented because it requires a lot of memory

    /**
     * Get the current status of connection(UDP and TCP). 
     * 
     * @param status (return by reference)
     */
    void getIPStatus(char *status);
    
    /**
     * Get the IP address of ESP8266. 
     *
     * @param IPlist (returned by reference) 
     */
    void getLocalIP(char *IPlist);
    
    /**
     * Enable IP MUX(multiple connection mode). 
     *
     * In multiple connection mode, a couple of TCP and UDP communication can be built. 
     * They can be distinguished by the identifier of TCP or UDP named mux_id. 
     * 
     * @return 1 - success.
     * @return 0 - failure.
     */
    int enableMUX(void);
    
    /**
     * Disable IP MUX(single connection mode). 
     *
     * In single connection mode, only one TCP or UDP communication can be built. 
     * 
     * @return 1 - success.
     * @return 0 - failure.
     */
    int disableMUX(void);
    
    
    /**
     * Create TCP connection in single mode. 
     * 
     * @param addr - the IP or domain name of the target host. 
     * @param port - the port number of the target host. 
     * @return 1 - success.
     * @return 0 - failure.
     */
    int createTCP(char *addr, uint32_t port);
    
    /**
     * Release TCP connection in single mode. 
     * 
     * @return 1 - success.
     * @return 0 - failure.
     */
    int releaseTCP(void);
    
    /**
     * Register UDP port number in single mode.
     * 
     * @param addr - the IP or domain name of the target host. 
     * @param port - the port number of the target host. 
     * @return 1 - success.
     * @return 0 - failure.
     */
    int registerUDP(char *addr, uint32_t port);
    
    /**
     * Unregister UDP port number in single mode. 
     * 
     * @return 1 - success.
     * @return 0 - failure.
     */
    int unregisterUDP(void);
  
    /**
     * Create TCP connection in multiple mode. 
     * 
     * @param mux_id - the identifier of this TCP(available value: 0 - 4). 
     * @param addr - the IP or domain name of the target host. 
     * @param port - the port number of the target host. 
     * @return 1 - success.
     * @return 0 - failure.
     */
    int createTCP(uint8_t mux_id, char *addr, uint32_t port);
    
    /**
     * Release TCP connection in multiple mode. 
     * 
     * @param mux_id - the identifier of this TCP(available value: 0 - 4). 
     * @return 1 - success.
     * @return 0 - failure.
     */
    int releaseTCP(uint8_t mux_id);
    
    /**
     * Register UDP port number in multiple mode.
     * 
     * @param mux_id - the identifier of this TCP(available value: 0 - 4). 
     * @param addr - the IP or domain name of the target host. 
     * @param port - the port number of the target host. 
     * @return 1 - success.
     * @return 0 - failure.
     */
    int registerUDP(uint8_t mux_id, char *addr, uint32_t port);
    
    /**
     * Unregister UDP port number in multiple mode. 
     * 
     * @param mux_id - the identifier of this TCP(available value: 0 - 4). 
     * @return 1 - success.
     * @return 0 - failure.
     */
    int unregisterUDP(uint8_t mux_id);


    /**
     * Set the timeout of TCP Server. 
     * 
     * @param timeout - the duration for timeout by second(0 ~ 28800, default:180). 
     * @return 1 - success.
     * @return 0 - failure.
     */
    int setTCPServerTimeout(uint32_t timeout = 180);
    
    /**
     * Start TCP Server(Only in multiple mode). 
     * 
     * After started, user should call method: getIPStatus to know the status of TCP connections. 
     * The methods of receiving data can be called for user's any purpose. After communication, 
     * release the TCP connection is needed by calling method: releaseTCP with mux_id. 
     *
     * @param port - the port number to listen(default: 333).
     * @return 1 - success.
     * @return 0 - failure.
     *
     * @see String getIPStatus(void);
     * @see uint32_t recv(uint8_t *coming_mux_id, uint8_t *buffer, uint32_t len, uint32_t timeout);
     * @see int releaseTCP(uint8_t mux_id);
     */
    int startTCPServer(uint32_t port = 333);

    /**
     * Stop TCP Server(Only in multiple mode). 
     * 
     * @return 1 - success.
     * @return 0 - failure.
     */
    int stopTCPServer(void);
    
    /**
     * Start Server(Only in multiple mode). 
     * 
     * @param port - the port number to listen(default: 333).
     * @return 1 - success.
     * @return 0 - failure.
     *
     * @see String getIPStatus(void);
     * @see uint32_t recv(uint8_t *coming_mux_id, uint8_t *buffer, uint32_t len, uint32_t timeout);
     */
    int startServer(uint32_t port = 333);

    /**
     * Stop Server(Only in multiple mode). 
     * 
     * @return 1 - success.
     * @return 0 - failure.
     */
    int stopServer(void);

    /**
     * Send data based on TCP or UDP builded already in single mode. 
     * 
     * @param buffer - the buffer of data to send. 
     * @param len - the length of data to send. 
     * @return 1 - success.
     * @return 0 - failure.
     */
    int sendSingle(const uint8_t *buffer, uint32_t len);
            
    /**
     * Send data based on one of TCP or UDP builded already in multiple mode. 
     * 
     * @param mux_id - the identifier of this TCP(available value: 0 - 4). 
     * @param buffer - the buffer of data to send. 
     * @param len - the length of data to send. 
     * @return 1 - success.
     * @return 0 - failure.
     */
    int sendMultiple(uint8_t mux_id, const uint8_t *buffer, uint32_t len);
    
    /**
     * Receive data from TCP or UDP builded already in single mode. 
     *
     * @param buffer - the buffer for storing data. 
     * @param buffer_size - the length of the buffer. 
     * @param timeout - the time waiting data. 
     * @return the length of data received actually. 
     */
    uint32_t recvSingle(uint8_t *buffer, uint32_t buffer_size, uint32_t timeout = 1000);
    
    /**
     * Receive data from one of TCP or UDP builded already in multiple mode. 
     *
     * @param mux_id - the identifier of this TCP(available value: 0 - 4). 
     * @param buffer - the buffer for storing data. 
     * @param buffer_size - the length of the buffer. 
     * @param timeout - the time waiting data. 
     * @return the length of data received actually. 
     */
    uint32_t recvMultipleFrom1Conn(uint8_t mux_id, uint8_t *buffer, uint32_t buffer_size, uint32_t timeout = 1000);
    
    /**
     * Receive data from all of TCP or UDP builded already in multiple mode. 
     *
     * After return, coming_mux_id store the id of TCP or UDP from which data coming. 
     * User should read the value of coming_mux_id and decide what next to do. 
     * 
     * @param coming_mux_id - the identifier of TCP or UDP. 
     * @param buffer - the buffer for storing data. 
     * @param buffer_size - the length of the buffer. 
     * @param timeout - the time waiting data. 
     * @return the length of data received actually. 
     */
    uint32_t recvMultipleFromAll(uint8_t *coming_mux_id, uint8_t *buffer, uint32_t buffer_size, uint32_t timeout = 1000);

#endif