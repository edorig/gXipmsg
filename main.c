// created by: geo (March 2012)
#include "gXipmsg.h"

static int gXipmsg_main (int argc, char* argv[])
{	
	
	
	// Get local hostname
	if((gethostname(GXIM_Local_Hostname, GXIM_NAME_MAXLEN)) == -1)
	{
		printf("error: gethostname()");
		exit(1);
	}
	
	// Get local username
	if((GXIM_Passwd = getpwuid (getuid())) == NULL)
	{
		printf("error: getpwuid(getuid())");
		exit(1);
	}
	else
	{
		// Set local username
		strcpy(GXIM_Local_Username, GXIM_Passwd->pw_name);
		
		// Set local handlename
		strcpy(GXIM_Local_Handlename, GXIM_Passwd->pw_name);
	}
	
	// Initialize a socket for UDP
	if((udp_InitSocket(&GXIM_UDP_Socket)) == -1)
	{
		exit(1);
	}
	
	printf("send entry");
	udp_BroadcastString(&GXIM_UDP_Socket, pack_Broadcast(GXIM_IPMSG_NOOPERATION, GXIM_Local_Username, GXIM_Local_Hostname, GXIM_Local_Handlename));
	udp_BroadcastString(&GXIM_UDP_Socket, pack_Broadcast(GXIM_IPMSG_BR_ENTRY, GXIM_Local_Username, GXIM_Local_Hostname, GXIM_Local_Handlename));
		

	
	sendForm_Init(&GXIM_App, &GXIM_TopLevel, argc, argv);
	sendForm_SetupClose(&GXIM_TopLevel, gxipmg_AtExit);
	sendForm_SetupTimeout(&GXIM_App, gxipmg_CheckData);
	sendForm_Run(&GXIM_App);
	
		

	return 0;
}

int main(int argc, char* argv[])
{
	return gXipmsg_main(argc,argv);    
}

void gxipmg_AtExit(Widget w_Widget, XtPointer xp_Client_data, XtPointer xp_Call_data)
{		
	printf("send exit");
	udp_BroadcastString(&GXIM_UDP_Socket, pack_Broadcast(GXIM_IPMSG_NOOPERATION, GXIM_Local_Username, GXIM_Local_Hostname, GXIM_Local_Handlename));
	udp_BroadcastString(&GXIM_UDP_Socket, pack_Broadcast(GXIM_IPMSG_BR_EXIT, GXIM_Local_Username, GXIM_Local_Hostname, GXIM_Local_Handlename));
	udp_BroadcastString(&GXIM_UDP_Socket, pack_Broadcast(GXIM_IPMSG_NOOPERATION, GXIM_Local_Username, GXIM_Local_Hostname, GXIM_Local_Handlename));
	udp_BroadcastString(&GXIM_UDP_Socket, pack_Broadcast(GXIM_IPMSG_BR_EXIT, GXIM_Local_Username, GXIM_Local_Hostname, GXIM_Local_Handlename));
		
	udp_CloseSocket(&GXIM_UDP_Socket);	
	exit(1);
}

void gxipmg_CheckData(XtPointer xp_Client_data, XtIntervalId* id)
{
	char buff[1000];
	printf("check data");
	udp_InquirePackets(&GXIM_UDP_Socket, buff);
	
	sendForm_SetupTimeout(&GXIM_App, gxipmg_CheckData);
}

