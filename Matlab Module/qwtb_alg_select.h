/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  PROCALGSEL                       1       /* callback function: cb_alg_sel_panel */
#define  PROCALGSEL_RING_UNC_CALC_SEL     2       /* control type: ring, callback function: (none) */
#define  PROCALGSEL_RING_SEL_ALG          3       /* control type: ring, callback function: ring_alg_sel */
#define  PROCALGSEL_PARTAB                4       /* control type: table, callback function: par_tab_mod */
#define  PROCALGSEL_BTN_LOAD_ALG          5       /* control type: command, callback function: btn_proc_load_alg */
#define  PROCALGSEL_BTN_LOAD_ALGS         6       /* control type: command, callback function: btn_proc_load_algs */
#define  PROCALGSEL_TWMPARDESC            7       /* control type: textBox, callback function: (none) */
#define  PROCALGSEL_LED_ALG_MULTI         8       /* control type: LED, callback function: (none) */
#define  PROCALGSEL_LED_ALG_DIFF          9       /* control type: LED, callback function: (none) */
#define  PROCALGSEL_LED_ALG_UI            10      /* control type: LED, callback function: (none) */
#define  PROCALGSEL_TWM_MCM_ITER          11      /* control type: numeric, callback function: (none) */
#define  PROCALGSEL_BTN_ALG_SEL_DONE      12      /* control type: command, callback function: btn_alg_sel_done */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK btn_alg_sel_done(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK btn_proc_load_alg(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK btn_proc_load_algs(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK cb_alg_sel_panel(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK par_tab_mod(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ring_alg_sel(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
