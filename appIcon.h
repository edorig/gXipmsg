// created by: geo (April 2012)
#ifndef APPICON_H_
#define APPICON_H_

#include "main.h"

// Constants


// Variable declerations
Widget APPICON_Form;
Widget APPICON_BtnG_Icon;
Widget APPICON_List_Users;

Pixmap pixmap;
Pixel fg, bg;

Atom APPICON_DelWin_protocol;

// Function declerations
void appIcon_Init(Widget* w_TopLevel, int argc, char* argv[]);
void appIcon_SetupClose(Widget* w_TopLevel, XtCallbackProc xcp_CloseProc, XtPointer client_data);
void appIcon_SetupTimeout(XtTimerCallbackProc xcp_TOProc);
void appIcon_Run(void);
void appIcon_IconCallBack(Widget widget, XtPointer client_data, XtPointer call_data);
void appIcon_AddRemoveItem(Widget wList, struct Broadcast_Packet* p_Item, char m_Option, XtPointer xtLabel);

void appIcon_AddRemoveList(struct Broadcast_Packet* p_Item, char m_Option);


void appIcon_RecieveDialog(struct Broadcast_Packet* p_Item);
void appIcon_ReplyDialog(int mPos, char* qText);


void appIcon_UpdateLists(struct NODE *llist, struct Broadcast_Packet* p_Item, char m_Option);

Boolean appIcon_SearchList(long* pNum, char* strPacket);
Boolean appIcon_SearchNode(struct SendClientData* num, int* mIdx, char* strHostname);
Boolean appIcon_SearchItems(struct SendClientData* num, int* mIdx, char* strFileID);

void append_node(struct NODE *llist, struct SendClientData* num);
void delete_node(struct NODE *llist, struct SendClientData* num);

void appIcon_Unreg(struct SendClientData* num);
void appIcon_ClearUserList(XtPointer clientList);

#endif /*APPICON_H_*/
