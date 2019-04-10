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

#define  TRESPANEL                        1       /* callback function: panelCB */
#define  TRESPANEL_PATH_RES               2       /* control type: string, callback function: (none) */
#define  TRESPANEL_TABLE                  3       /* control type: table, callback function: (none) */
#define  TRESPANEL_BTN_LOAD               4       /* control type: command, callback function: btn_load */
#define  TRESPANEL_BTN_EXIT               5       /* control type: command, callback function: btn_exit_rv */
#define  TRESPANEL_BTN_CMD                6       /* control type: command, callback function: btn_cmd */
#define  TRESPANEL_MAXDIM                 7       /* control type: ring, callback function: (none) */
#define  TRESPANEL_REFMODE                8       /* control type: ring, callback function: (none) */
#define  TRESPANEL_PHIMODE                9       /* control type: ring, callback function: (none) */
#define  TRESPANEL_GRPMODE                10      /* control type: ring, callback function: (none) */
#define  TRESPANEL_UNCMODE                11      /* control type: ring, callback function: (none) */
#define  TRESPANEL_RING_ALG               12      /* control type: ring, callback function: (none) */
#define  TRESPANEL_RING_RES               13      /* control type: ring, callback function: (none) */
#define  TRESPANEL_MAXDIMSZ               14      /* control type: numeric, callback function: (none) */


     /* Control Arrays: */

#define  CTRLARRAY                        1

     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK btn_cmd(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK btn_exit_rv(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK btn_load(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK panelCB(int panel, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
