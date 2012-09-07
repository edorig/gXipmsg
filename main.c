// created by: geo (March 2012)
#include "gXipmsg.h"

Position posX, posY;
int n;
Arg args[20];
char bgcolor[10];
char fgcolor[10];


static int gXipmsg_main (int argc, char* argv[])
{	
	// Get local hostname
	if((gethostname(GXIM_Local_Hostname, HOSTNAME_MAXLEN)) == -1)
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
	}
	
	//Get handlename
	if(argc > 1)
	{
		// Set local handlename 
		strcpy(GXIM_Local_Handlename, argv[1]);
	}
	else
	{
		// Set local handlename using username
		strcpy(GXIM_Local_Handlename, GXIM_Passwd->pw_name);
	}
	
	// Initialize a socket for UDP
	if((udp_InitSocket(&GXIM_UDP_Socket, &GXIM_Local_Username, &GXIM_Local_Hostname, &GXIM_Local_Handlename)) == -1)
	{
		exit(1);
	}
	
	// Initialize a socket for TCP
	if((tcp_InitSocket(&GXIM_TCP_Socket)) == -1)
	{
		exit(1);
	}

	appIcon_Init(&GXIM_App, &GXIM_TopLevel, argc, argv);
	appIcon_SetupClose(&GXIM_TopLevel, gxipmg_AtExit);
	appIcon_SetupTimeout(&GXIM_App, gxipmg_CheckData);
	appIcon_Run(&GXIM_App);
			

	return 0;
}

int main(int argc, char* argv[])
{
	return gXipmsg_main(argc,argv);    
}

void gxipmg_AtExit(Widget w_Widget, XtPointer xp_Client_data, XtPointer xp_Call_data)
{		
	udp_BroadcastExit();
	udp_CloseSocket();	
	tcp_CloseSocket(&GXIM_TCP_Socket);
	exit(1);
}

void gxipmg_CheckData(XtPointer xp_Client_data, XtIntervalId* id)
{
	udp_InquirePackets();
	
	appIcon_SetupTimeout(&GXIM_App, gxipmg_CheckData);	
}

void gXipmsg_MapDialog(Widget dialog, XtPointer client_data, XtPointer call_data)
{
	Dimension w, h;
	
	XtVaGetValues(dialog, XmNwidth, &w, XmNheight, &h, NULL);
	if ((posX + w) >= WidthOfScreen (XtScreen (dialog)))
		posX = 0;
	if ((posY + h) >= HeightOfScreen (XtScreen (dialog)))
		posY = 0;
	XtVaSetValues (dialog, XmNx, posX, XmNy, posY, NULL);
	posX += 20;
	posY += 20;	
}

