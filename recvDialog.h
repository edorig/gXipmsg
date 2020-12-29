// created by: geo (April 2012)
#ifndef RECVDIALOG_H_
#define RECVDIALOG_H_

#include "main.h"

// Constants


// Variable declerations


// Function declerations
void recvDialog_Create(XtPointer xt_List, struct Broadcast_Packet* p_Item);
void recvDialog_ReplyCallBack(Widget widget, XtPointer client_data, XtPointer call_data);
void recvDialog_CloseCallBack(Widget widget, XtPointer client_data, XtPointer call_data);
void recvDialog_DownloadCallBack(Widget widget, XtPointer client_data, XtPointer call_data);
void recvDialog_Destroy(Widget dialog, XtPointer client_data, XtPointer call_data);

void recvDialog_ComposeFilenames(char* strDest, char* strSrc);
Boolean recvDialog_DLProcedure(XtPointer client_data);
void recvDialog_UpdateBtnLabel(Widget widget, char* strLabel);
void recvDialog_AtExit(Widget w_Widget, XtPointer xp_Client_data, XtPointer xp_Call_data);

void recvDialog_CancelAddCallBack(Widget widget, XtPointer client_data, XtPointer call_data);
void recvDialog_SaveFileCallBack(Widget widget, XtPointer client_data, XtPointer call_data);

#endif /*RECVDIALOG_H_*/
