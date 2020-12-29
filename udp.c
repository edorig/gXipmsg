// created by: geo (March 2012)
#include "udp.h"
#include "pack.h"

#ifdef IP_ONESBCAST

void udp_GetInfo(int* p_Socket)
{
	struct ifconf ifc; /* holds IOCTL return value for SIOCGIFCONF */
	int	return_val,	numreqs = 30, n;
	struct ifreq *ifr; /* points to one interface returned from ioctl */
			
	memset (&ifc, 0, sizeof(ifc));

	ifc.ifc_buf = NULL;
	ifc.ifc_len =  sizeof(struct ifreq) * numreqs;
	ifc.ifc_buf = malloc(ifc.ifc_len);

	/* This code attempts to handle an arbitrary number of interfaces,
	   it keeps trying the ioctl until it comes back OK and the size
	   returned is less than the size we sent it.
	 */
	for (;;) {
		ifc.ifc_len = sizeof(struct ifreq) * numreqs;
		ifc.ifc_buf = realloc(ifc.ifc_buf, ifc.ifc_len);
		
		if ((return_val = ioctl(*p_Socket, SIOCGIFCONF, &ifc)) < 0) {
			perror("SIOCGIFCONF");
			break;
		}
		if (ifc.ifc_len == sizeof(struct ifreq) * numreqs) {
			/* assume it overflowed and try again */
			numreqs += 10;
			continue;
		}
		break;
	}
	
	if (return_val < 0) {
		fprintf (stderr, "ioctl:");		
		exit(1);
	}


	/* loop through interfaces returned from SIOCGIFCONF */
	ifr=ifc.ifc_req;
	for (n=0; n < ifc.ifc_len; n+=sizeof(struct ifreq)) {

		
		
		/* Get the BROADCAST address */
		return_val = ioctl(*p_Socket,SIOCGIFBRDADDR, ifr);
		if (return_val == 0 ) {
			if (ifr->ifr_broadaddr.sa_family == AF_INET) {
				struct sockaddr_in
					*sin = (struct sockaddr_in *)
					&ifr->ifr_broadaddr;
				
				if(n == 0)
				{
					UDP_Info.sin_addr = sin->sin_addr;
				}
			}			
			
		} 

		/* check the next entry returned */
		ifr++;
	}

	/* we don't need this memory any more */
	free (ifc.ifc_buf);
}

#endif

int udp_InitSocket(int* p_Socket)
{
	int broadcast = 1;
	int buff_size;
	int buff_minsize;
	
	// Create socket
	if((*p_Socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		printf("error: socket(SOCK_DGRAM)");
		return -1;
	}
		
#ifdef IP_ONESBCAST
	
	// Get network info
	udp_GetInfo(p_Socket);
	
#endif
		
	// Create socket address from
	UDP_AddrFrom.sin_family = UDP_DEFAULT_FAMILY;
	UDP_AddrFrom.sin_port = htons(UDP_DEFAULT_PORT);
	UDP_AddrFrom.sin_addr.s_addr = UDP_DEFAULT_IPADDR;
	
	// Bind socket 
	if((bind(*p_Socket, (struct sockaddr*)&UDP_AddrFrom, sizeof(UDP_AddrFrom))) == -1)
	{
		printf("error: bind(udp)");
		return -1;
	}
	
	// Enable broadcast option 
	if((setsockopt(*p_Socket, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast))) == -1)
	{
		printf("error: setsockopt(udp, SO_BROADCAST)");
		return -1;
	}
	
	// Create socket address to
	UDP_AddrTo.sin_family = UDP_DEFAULT_FAMILY;
	UDP_AddrTo.sin_port = htons(UDP_DEFAULT_PORT);		
		
	UDP_LocalSocket = p_Socket;
	
	// Set UDP send buffer size
	buff_size = _MSG_BUF_SIZE;
	buff_minsize = _MSG_BUF_MIN_SIZE;
	if((setsockopt(*p_Socket, SOL_SOCKET, SO_SNDBUF, (void*)&buff_size, sizeof(int))) != 0 && (setsockopt(*p_Socket, SOL_SOCKET, SO_SNDBUF, (void*)&buff_minsize, sizeof(int))) != 0)
	{
		printf("error: setsockopt(udp, SOL_SOCKET, SENDSIZE)");
		return -1;
	}
		 
	// Set UDP recv buffer size
	buff_size = _MSG_BUF_SIZE;
	buff_minsize = _MSG_BUF_MIN_SIZE;
	if((setsockopt(*p_Socket, SOL_SOCKET, SO_RCVBUF, (void*)&buff_size, sizeof(int))) != 0 && (setsockopt(*p_Socket, SOL_SOCKET, SO_RCVBUF, (void*)&buff_minsize, sizeof(int))) != 0)
	{
		printf("error: setsockopt(udp, SOL_SOCKET, RECVSIZE)");
		return -1;
	}
		
	udp_BroadcastEntry();
		
	return 0;
}

void udp_BroadcastEntry(void)
{
	udp_BroadcastString((char*)pack_PackBroadcast(IPMSG_NOOPERATION, GXIM_Local_Username, GXIM_Local_Hostname, GXIM_Local_Handlename, NULL, NULL));
	pack_CleanPacketBuffer();		
	
	udp_BroadcastString((char*)pack_PackBroadcast(IPMSG_BR_ENTRY, GXIM_Local_Username, GXIM_Local_Hostname, GXIM_Local_Handlename, NULL, NULL));
	pack_CleanPacketBuffer();		
}

void udp_BroadcastExit(void)
{
	udp_BroadcastString((char*)pack_PackBroadcast(IPMSG_NOOPERATION, GXIM_Local_Username, GXIM_Local_Hostname, GXIM_Local_Handlename, NULL, NULL));
	pack_CleanPacketBuffer();
		
	udp_BroadcastString((char*)pack_PackBroadcast(IPMSG_BR_EXIT, GXIM_Local_Username, GXIM_Local_Hostname, GXIM_Local_Handlename, NULL, NULL));
	pack_CleanPacketBuffer();
			
	udp_BroadcastString((char*)pack_PackBroadcast(IPMSG_NOOPERATION, GXIM_Local_Username, GXIM_Local_Hostname, GXIM_Local_Handlename, NULL, NULL));
	pack_CleanPacketBuffer();		
	
	udp_BroadcastString((char*)pack_PackBroadcast(IPMSG_BR_EXIT, GXIM_Local_Username, GXIM_Local_Hostname, GXIM_Local_Handlename, NULL, NULL));
	pack_CleanPacketBuffer();
}

int udp_BroadcastString(char* p_String)
{
#ifdef IP_ONESBCAST
	
	int onesbcast = 1;   /* 0 = disable (default), 1 = enable */
	setsockopt(*UDP_LocalSocket, IPPROTO_IP, IP_ONESBCAST, &onesbcast, sizeof(onesbcast));
	UDP_AddrTo.sin_addr = UDP_Info.sin_addr;
	
#else
		
	// Set IP of socket address for broadcast
	UDP_AddrTo.sin_addr.s_addr = UDP_BROADCAST_IPADDR;
		
#endif	
	//	printf("DEBUG sending: %s\n",p_String);  
	// Send string to address to
	if((sendto(*UDP_LocalSocket, p_String, strlen(p_String)+1, 0, (struct sockaddr*)&UDP_AddrTo, sizeof(UDP_AddrTo))) == -1)
	{
		printf("error: sendto()\n");		
		return -1;
	}
	
	return 0;
}

int udp_SendToString(char* p_IPAddress, char* p_String, int m_Flags, char* p_extended, unsigned long* p_packetID)
{
	char* p_Buffer = (char*)pack_PackBroadcast(m_Flags, GXIM_Local_Username, GXIM_Local_Hostname, p_String, p_extended, p_packetID);
	int extlen = 0;
	
	if(p_extended != NULL)
	{
		extlen = strlen(p_extended);
	}
	
#ifdef IP_ONESBCAST
	
	int onesbcast = 0;   /* 0 = disable (default), 1 = enable */
	setsockopt(*UDP_LocalSocket, IPPROTO_IP, IP_ONESBCAST, &onesbcast, sizeof(onesbcast));
	
#endif
	
		
	// Set IP of socket address for send
	if((UDP_AddrTo.sin_addr.s_addr = inet_addr(p_IPAddress)) == -1)
	{
		printf("error: inet_addr(%s)\n", p_IPAddress);		
		return -1;
	}
		
	// Send string to address to
	if((sendto(*UDP_LocalSocket, p_Buffer, strlen(p_Buffer) + extlen + 1, 0, (struct sockaddr*)&UDP_AddrTo, sizeof(UDP_AddrTo))) == -1)
	{
		printf("error: sendto()\n");		
		return -1;
	}
	
	// Clean packed packet buffer
	pack_CleanPacketBuffer();
	
	return 0;
}

void udp_InquirePackets(void)
{
	int m_AddrLen = sizeof(UDP_AddrFrom);
	char* UDP_Buffer;
	
	// Allocate mem for buffer
	UDP_Buffer = malloc(_MSG_BUF_SIZE);
	
	while(recvfromTimeOutUDP(*UDP_LocalSocket, 0, 0) > 0)
	{		
		// Clear buffer
		memset(UDP_Buffer,'\0',_MSG_BUF_SIZE);
			
		// Ok the data is ready, call recvfrom() to get it then
	    recvfrom(*UDP_LocalSocket, UDP_Buffer, _MSG_BUF_SIZE, 0, (struct sockaddr*)&UDP_AddrFrom, &m_AddrLen);
	    
	    pack_UnpackBroadcast(UDP_Buffer, &UDP_DataFrom);
	    strcpy(UDP_DataFrom.IP_Address, inet_ntoa(UDP_AddrFrom.sin_addr));
	    
	    // Decode command
	    switch(GET_MODE(UDP_DataFrom.IP_Flags))
		{
			case IPMSG_NOOPERATION:
				break;			
				
			case IPMSG_BR_ENTRY:
				udp_SendToString(UDP_DataFrom.IP_Address, GXIM_Local_Handlename, IPMSG_ANSENTRY, NULL, NULL);
				appIcon_AddRemoveList(&UDP_DataFrom, 1);
				break;
				
			case IPMSG_ANSENTRY:
				appIcon_AddRemoveList(&UDP_DataFrom, 1);
				break;
				
			case IPMSG_BR_EXIT:				
				appIcon_AddRemoveList(&UDP_DataFrom, 0);
				break;
				
			case IPMSG_SENDMSG:
				// Pop message
				appIcon_RecieveDialog(&UDP_DataFrom);
								
				// Confirm send
				udp_SendToString(UDP_DataFrom.IP_Address, UDP_DataFrom.UNIX_Time, IPMSG_RECVMSG, NULL, NULL);
				break;
				
			case IPMSG_RECVMSG:
				printf("IPMSG_RECVMSG\n");
				break;
				
			default:
				printf("unknown UDP command: %s\n", UDP_Buffer);
		}	    
	}	
	
	// Release buffer memory
	free(UDP_Buffer);
}

void udp_CloseSocket(void)
{
	if(*UDP_LocalSocket != -1)
	{
		// Close socket
		close(*UDP_LocalSocket);
		*UDP_LocalSocket = -1;
	}
}

// A sample of the select() return value
int recvfromTimeOutUDP(int socket, long sec, long usec)
{
    // Setup timeval variable
    struct timeval timeout;
    fd_set fds;
 
    timeout.tv_sec = sec;
    timeout.tv_usec = usec;
    // Setup fd_set structure
    FD_ZERO(&fds);
    FD_SET(socket, &fds);
    // Return value:
    // -1: error occurred
    // 0: timed out
    // > 0: data ready to be read
    return select(socket+1, &fds, 0, 0, &timeout);
}
