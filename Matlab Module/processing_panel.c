//==============================================================================
//
// Title:		qwtb processing panel
// Purpose:		A short description of the application.
//
// Created on:	2.3.2014 at 16:16:57 by .
// Copyright:	. All Rights Reserved.
//
//==============================================================================

//==============================================================================
// Include files

#include <ansi_c.h>
#include <windows.h>
#include <Shlwapi.h>
#include <cvirte.h>		
#include <userint.h>
#include "toolbox.h"
#include "processing_panel.h"
#include "qwtb_alg_select.h"
#include "Matlab Module.h"

#include "mlink.h"
#include "twm_matlab.h"
#include "utils.h"
#include "matlab_globals.h"

//==============================================================================
// Constants

#define INIFILE "config.ini" 

//==============================================================================
// Types

//==============================================================================
// Static global variables

static int panelHandle = 0;


//==============================================================================
// Global variables

// Matlab link handle
TMLink mlink;
// config.ini full path
char ini[MAX_PATHNAME_LEN];


//==============================================================================
// Static functions

//==============================================================================


//==============================================================================
// Global functions

/*
/// HIFN The main entry-point function.
int main (int argc, char *argv[])
{
	int error = 0;
	
	// initialize and load resources
	nullChk (InitCVIRTE (0, argv, 0));
	//errChk (panelHandle = LoadPanel (0, "Matlab Module.uir", PANEL));
	errChk (panelHandle = LoadPanel (0, "processing_panel.uir", PROCPANEL));

	// display the panel and run the user interface
	errChk (DisplayPanel (panelHandle));
	errChk (RunUserInterface ());
	

Error:
	// clean up
	if (panelHandle > 0)
		DiscardPanel (panelHandle);
	
	
	return 0;
}*/



//==============================================================================
// UI callback function prototypes


// --- panel events
int CVICALLBACK cb_proc_panel(int panel, int event, void *callbackData,
		int eventData1, int eventData2)
{
	int ret;
	
	if(event == EVENT_CLOSE)
	{
		// --- EXIT ---
		
		//QuitUserInterface(0);
		
		// try to close Matlab
		//mlink_close(&mlink,1);
		
		// store last path
		WritePrivateProfileString("PATH","last_result_path",resfld,ini);
		
		// destroy panel
		DiscardPanel(panel);
	}
	else if(event == EVENT_GOT_FOCUS)
	{
		// --- window focused ---
		static int first_time = 0;
		if(!first_time)
		{
			first_time = 1;
			// --- window first run: this is called just once per run of the application ---
			
			// ###tobe removed
			//SetCtrlVal(panel, PROCPANEL_TXT_SESSION, "E:\\C\\LW_CVI\\Matlab Module\\test_data\\session.info");
			
			SetCtrlVal(panel, PROCPANEL_TXT_OUTPUT, "Please wait, starting Matlab interface...");
			
			
			// initialize QWTB processing setup
			TTWMqwtbCfg cfg_qwtb;
			memset((void*)&cfg_qwtb,0,sizeof(TTWMqwtbCfg));
			
			
			// get app directory
			char appdir[MAX_PATHNAME_LEN];
			GetProjectDir(appdir);
	
			// build ini path
			strcpy(ini,appdir);
			strcat(ini,"\\config.ini");
	
			// load TWM function path from INI file
			char twm_path[MAX_PATH];
			ret = GetPrivateProfileString("PATH","twm_octave_folder","",twm_path,MAX_PATH,ini);
			if(!ret)
			{
				MessageBoxA(NULL,"Missing INI file or the [PATH],twm_octave_folder value!","Error",0);
				SetCtrlVal(panel, PROCPANEL_TXT_OUTPUT, "Please wait, starting Matlab interface ... failed!");
				return(1);
			}
	
			// load last result folder path from INI file
			ret = GetPrivateProfileString("PATH","last_result_path","",resfld,MAX_PATH,ini);
	
	
			// --- MATLAB INIT ---
			// init
			mlink_init(&mlink,ML_MATLAB);
	
			// try to start Matlab
			ret = mlink_start(&mlink);
			if(ret)
			{
				MessageBoxA(NULL,"Cannot start Matlab! Possibly not available? Or it is different bit version than this application.","Error",0);
				SetCtrlVal(panel, PROCPANEL_TXT_OUTPUT, "Please wait, starting Matlab interface ... failed!");
				return(1);
			}
			
			// initialize TWM link
			char *errstr;
			ret = twm_init(&mlink,&errstr,twm_path);
			// failed?
			if(ret)
			{
				MessageBoxA(NULL,errstr,"TWM initialization module error",0);
				free((void*)errstr);
				SetCtrlVal(panel, PROCPANEL_TXT_OUTPUT, "Please wait, starting Matlab interface ... failed!");
				return(1);
			}
			
			SetCtrlVal(panel, PROCPANEL_TXT_OUTPUT, " done");
		}
		
	}
	
	return 0;
}


// --- user pressed exit
int CVICALLBACK btn_exit (int panel, int control, int event,
						  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:

			cb_proc_panel(panel, EVENT_CLOSE, NULL, 0, 0);
			
			break;
	}
	return 0;
}


// --- user pressed show results: show QWTB results panel
int CVICALLBACK btn_result (int panel, int control, int event,
							void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			
			// initiate QWTB results viewer panel	
			int resPanelHandle = LoadPanel(0, "Matlab Module.uir", TRESPANEL);
			
			// set path
			char path[TWMMAXSTR];
			GetCtrlVal(panel, PROCPANEL_TXT_SESSION, (void*)path);
			char *pstr = strrchr(path,'\\');
			if(pstr)
				*pstr = '\0';
			SetCtrlVal(resPanelHandle, TRESPANEL_PATH_RES, (void*)path);
			
			DisplayPanel(resPanelHandle);
			// no UI run - that should run from main panel?

			break;
	}
	return 0;
}

// --- user pressed QWTB processing setup: start setup algorithm panel
int CVICALLBACK btn_setup (int panel, int control, int event,
						   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:

			// initate QWTB processing setup panel
			int cfg_panel = LoadPanel(0,"qwtb_alg_select.uir",PROCALGSEL);			
			DisplayPanel(cfg_panel);
			
			break;
	}
	return 0;
}

// --- user pressed processing start: start processing the session records
int CVICALLBACK btn_start (int panel, int control, int event,
						   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			
			// clear status box
			ResetTextBox(panel, PROCPANEL_TXT_OUTPUT, "");
			
			// check if the processing is set up
			if(!strlen(cfg_qwtb.alg_id))
			{
				SetCtrlVal(panel, PROCPANEL_TXT_OUTPUT, "QWTB processing setup is not available!\n"
						   								"First run the Algorithm Setup to select the processing algorithm.");
				return(1);
			}
			
			// get session path
			char ssn_path[TWMMAXSTR];
			GetCtrlVal(panel, PROCPANEL_TXT_SESSION, ssn_path);
			// extract folder path
			char *p = strrchr(ssn_path, '\\');
			if(p) *p = '\0';
			
			
			if(!FileExists(ssn_path, NULL))
			{
				char str[1024];
				sprintf(str,"Measurement session '%s' is not found or valid!", ssn_path);
				SetCtrlVal(panel, PROCPANEL_TXT_OUTPUT, str);
				return(1);
			}
			
			// parse session to get some basic elements (e.g. records count)
			TTWMssnInf info;
			if(twm_read_session(ssn_path, &info))
			{
				SetCtrlVal(panel, PROCPANEL_TXT_OUTPUT, "Parsing measurement session failed!");
				return(1);
			}
			
			// store QWTB setup
			if(twm_write_proc_info(ssn_path, &cfg_qwtb))
			{
				SetCtrlVal(panel, PROCPANEL_TXT_OUTPUT, "Writting QWTB processing setup failed!");
				return(1);
			}
			
			
			// start processing for each record
			for(int r = 0;r < info.rec_count;r++)
			{
				char str[1024];
				
				// print status
				sprintf(str, "Processing record %d of %d\nAlgorithm: %s\n", r+1, info.rec_count, cfg_qwtb.alg_id);
				ResetTextBox(panel, PROCPANEL_TXT_OUTPUT, "");
				SetCtrlVal(panel, PROCPANEL_TXT_OUTPUT, str);
				
				// initiate processing for r record
				char *errstr;
				int err = twm_exec_algorithm(&mlink, &errstr, ssn_path, "", (r+1) == info.rec_count, r+1, 0);
				if(err)
				{
					// something fucked up - print error output from QWTB
					
					sprintf(str, "\nProcessing failed with error:\n%s",errstr);
					SetCtrlVal(panel, PROCPANEL_TXT_OUTPUT, str);
					free((void*)errstr);
					return(1);
				}
				
			}
			
			// all done
			SetCtrlVal(panel, PROCPANEL_TXT_OUTPUT, "\nDone.");

			break;
	}
	return 0;
}



// --- manual selection of session folder
int CVICALLBACK btn_select_ssn (int panel, int control, int event,
								void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			
			
			// open result file
			char path[MAX_PATH];
			if(FileSelectPopup(resfld,"*.info","*.info","Select result session file",VAL_LOAD_BUTTON,0,0,1,1,path)
					== VAL_EXISTING_FILE_SELECTED);
			{
				
				// write file path to the path box
				SetCtrlVal(panel, PROCPANEL_TXT_SESSION, path);

				// extract folder path
				char *p = strrchr(path, '\\');
    			if(p) *p = '\0';
				
				// remember last result folder
				strcpy(resfld,path);
				
			}
			
			break;
	}
	return 0;
}





