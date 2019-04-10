/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/* Copyright (c) National Instruments 2019. All Rights Reserved.          */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  PROCPANEL                        1       /* callback function: cb_proc_panel */
#define  PROCPANEL_BTN_CANCEL             2       /* control type: command, callback function: btn_exit */
#define  PROCPANEL_BTN_QWTB               3       /* control type: command, callback function: btn_setup */
#define  PROCPANEL_BTN_RESULT             4       /* control type: command, callback function: btn_result */
#define  PROCPANEL_BTN_START              5       /* control type: command, callback function: btn_start */
#define  PROCPANEL_TXT_SESSION            6       /* control type: string, callback function: (none) */
#define  PROCPANEL_TXT_OUTPUT             7       /* control type: textBox, callback function: (none) */
#define  PROCPANEL_BTN_MAN_SELECT         8       /* control type: pictButton, callback function: btn_select_ssn */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK btn_exit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK btn_result(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK btn_select_ssn(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK btn_setup(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK btn_start(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK cb_proc_panel(int panel, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
