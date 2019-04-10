//==============================================================================
//
// Title:		Matlab Module
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
#include "Matlab Module.h"
#include "toolbox.h"
#include "qwtb_alg_select.h"
#include "processing_panel.h"

#include "mlink.h"
#include "twm_matlab.h"
#include "utils.h"
#include "matlab_globals.h"

//==============================================================================
// Constants



//==============================================================================
// Types

//==============================================================================
// Static global variables

//static int panelHandle = 0;


//==============================================================================
// Static functions

//==============================================================================
// Global variables

// last result path
char resfld[MAX_PATH];


//==============================================================================
// Global functions



//==============================================================================
// UI callback function prototypes

/// HIFN Exit when the user dismisses the panel.
int CVICALLBACK panelCB (int panel, int event, void *callbackData,
		int eventData1, int eventData2)
{
	if (event == EVENT_CLOSE)
	{
		DiscardPanel(panel);
	}
	
	return 0;
}


//------------------------------------------------------------------------------
// Refresh result view
//
int CVICALLBACK btn_cmd (int panel, int control, int event,
						 void *callbackData, int eventData1, int eventData2)
{
	
	switch (event)
	{
		case EVENT_COMMIT:
			
			char path[MAX_PATH];
			
			// default alg/res selection:
			int alg_id = -1;
			int res_id = -1;
			int ref_id = -1;
			// results found?
			int res_exist = 0;
			
			// get result path
			GetCtrlVal(panel,TRESPANEL_PATH_RES,(void*)path);
			
			// -- two pass assignement
			// 1) fill result/alg selectors
			// 2) load selected alg and result
			for(int pass = 0;pass < 2;pass++)
			{

				// get last algorithm selection
				// 0 - last used
				// 1,2,... - alg. selection ID
				GetCtrlIndex(panel,TRESPANEL_RING_ALG,&alg_id);
				if(alg_id < 0)
					alg_id = 0;
			
				// get last result selection
				// 0 - last measured
				// 1 - average of all
				// 2,3,... - result selection ID
				GetCtrlIndex(panel,TRESPANEL_RING_RES,&res_id);				
				if(res_id < 0)
					res_id = 0;
				
				// get ref channel mode selection
				// 0 - no ref
				// 1,2,... - channel selection ID
				GetCtrlIndex(panel,TRESPANEL_REFMODE,&ref_id);
				if(ref_id < 0)
					ref_id = 0;
				
				// alg. selection string
				char alg_id_str[256];
				strcpy(alg_id_str,"");
				if(pass && alg_id)
					GetLabelFromIndex(panel,TRESPANEL_RING_ALG,alg_id,(void*)alg_id_str);

				// read results
				char *res_files;
				char *alg_list;
				char *chn_list;
				char *errstr;
				int ret = twm_get_result_info(&mlink,&errstr,path,alg_id_str,&res_exist,&res_files,&alg_list,&chn_list);
				if(ret)
				{
					// failed
					MessageBoxA(NULL,errstr,"Matlab error",0);
					free((void*)errstr);
					return(1);
				}
			
				
			
				// clear result selectors			
				ClearListCtrl(panel,TRESPANEL_RING_RES);
				ClearListCtrl(panel,TRESPANEL_RING_ALG);
				ClearListCtrl(panel,TRESPANEL_REFMODE);
			
			
				if(res_exist)
				{
					// --- something found - fill the dialog
				
					int rows,cols,cc;
					char **cells,*csv;
				
					// fill algorithms list
					csv = (char*)malloc((strlen(alg_list)+100)*sizeof(char));
					strcpy(csv,"Current: ");
					strcat(csv,alg_list);
					free((void*)alg_list);
					cc = csv_parse(csv,';','\t',&rows,&cols,NULL);
					cells = (char**)malloc(cc*sizeof(char*));
					csv_parse(csv,';','\t',&rows,&cols,cells);
					InsertListItem(panel,TRESPANEL_RING_ALG,0,cells[0],0);
					for(int k=2;k<cc;k++)
						InsertListItem(panel,TRESPANEL_RING_ALG,k-1,cells[k],k-1);
					free((void*)cells);
					free((void*)csv);
					if(alg_id >= cc-1)
					{
						SetCtrlIndex(panel,TRESPANEL_RING_ALG,0);
						SetCtrlIndex(panel,TRESPANEL_RING_RES,0);
					}
					else
					{
						SetCtrlIndex(panel,TRESPANEL_RING_ALG,alg_id);
					}

					// fill results list
					csv = (char*)malloc((strlen(res_files)+100)*sizeof(char));
					strcpy(csv,"Current result\tAverage\t");
					strcat(csv,res_files);
					free((void*)res_files);
					cc = csv_parse(csv,';','\t',&rows,&cols,NULL);
					cells = (char**)malloc(cc*sizeof(char*));
					csv_parse(csv,';','\t',&rows,&cols,cells);
					for(int k=0;k<cc;k++)
						InsertListItem(panel,TRESPANEL_RING_RES,k,cells[k],k);
					free((void*)cells);
					free((void*)csv);
					if(res_id >= cc)
					{	
						SetCtrlIndex(panel,TRESPANEL_RING_RES,0);
						res_id = 0;
					}
					else
						SetCtrlIndex(panel,TRESPANEL_RING_RES,res_id);
					
					// fill results list
					csv = (char*)malloc((strlen(chn_list)+100)*sizeof(char));
					strcpy(csv,"None;");
					strcat(csv,chn_list);
					free((void*)chn_list);
					cc = csv_parse(csv,'\t',';',&rows,&cols,NULL);
					cells = (char**)malloc(cc*sizeof(char*));
					csv_parse(csv,'\t',';',&rows,&cols,cells);
					for(int k=0;k<cc;k++)
						InsertListItem(panel,TRESPANEL_REFMODE,k,cells[k],k);
					free((void*)cells);
					free((void*)csv);
					if(ref_id >= cc)
					{
						SetCtrlIndex(panel,TRESPANEL_REFMODE,0);
						ref_id = 0;
					}
					else
						SetCtrlIndex(panel,TRESPANEL_REFMODE,ref_id);
				
				}
				else
				{
					// failed
					MessageBoxA(NULL,"Selected result is invalid or contains no calculated results.","Matlab error",0);
					break;
				}
			}
			
			
			if(res_exist)
			{
				// -- load result data
				// display config
				TResCfg cfg;
				GetCtrlVal(panel,TRESPANEL_MAXDIM,(void*)&cfg.max_dim);
				GetCtrlVal(panel,TRESPANEL_MAXDIMSZ,(void*)&cfg.max_array);
				GetCtrlVal(panel,TRESPANEL_GRPMODE,(void*)&cfg.group_mode);
				GetCtrlVal(panel,TRESPANEL_UNCMODE,(void*)&cfg.unc_mode);
				GetCtrlVal(panel,TRESPANEL_PHIMODE,(void*)&cfg.phi_mode);
				GetCtrlVal(panel,TRESPANEL_REFMODE,(void*)&cfg.phi_ref_chn);
				
				// alg. selection string
				char alg_id_str[256];
				strcpy(alg_id_str,"");
				if(alg_id)
					GetLabelFromIndex(panel,TRESPANEL_RING_ALG,alg_id,(void*)alg_id_str);

				// obtain result data from matlab
				char *csv = NULL;
				char *errstr;
				int ret = twm_get_result_data(&mlink,&errstr,path,res_id-1,alg_id_str,&cfg,&csv);
				if(ret)
				{
					// failed
					MessageBoxA(NULL,errstr,"Matlab error",0);
					free((void*)errstr);
					return(1);
				}
	
				int rows,cols; 
				int cc = csv_parse(csv,'\t','\n',&rows,&cols,NULL);
				char **cells = (char**)malloc(cc*sizeof(char*));
				csv_parse(csv,'\t','\n',&rows,&cols,cells);
	
				
				// disable table refresh
				SetCtrlAttribute(panel, TRESPANEL_TABLE, ATTR_VISIBLE, 0);
				
				// refresh table data size
				DeleteTableColumns(panel,TRESPANEL_TABLE,1,-1);
				DeleteTableRows(panel,TRESPANEL_TABLE,1,-1);
				InsertTableRows(panel,TRESPANEL_TABLE,1,rows-1,VAL_CELL_STRING);
				InsertTableColumns(panel,TRESPANEL_TABLE,1,cols-1,VAL_CELL_STRING);
			
				// refresh headers
				SetTableRowAttribute (panel, TRESPANEL_TABLE, -1, ATTR_USE_LABEL_TEXT, 1);
				for(int r = 1;r<rows;r++)
					SetTableRowAttribute(panel,TRESPANEL_TABLE,r,ATTR_LABEL_TEXT,cells[r*cols]);
				SetTableColumnAttribute (panel, TRESPANEL_TABLE, -1, ATTR_USE_LABEL_TEXT, 1);
				for(int c = 1;c<cols;c++)
					SetTableColumnAttribute(panel,TRESPANEL_TABLE,c,ATTR_LABEL_TEXT,cells[c]);
				
				// refresh data				
				for(int r = 1;r<rows;r++)
					SetTableCellRangeVals(panel,TRESPANEL_TABLE,MakeRect(r,1,1,cols-1),&cells[r*cols+1],VAL_ROW_MAJOR);
				
				// autoscale
				for(int c = 1;c<cols;c++)
					SetColumnWidthToWidestCellContents(panel,TRESPANEL_TABLE,c);
				
				// enable table refresh
				SetCtrlAttribute(panel, TRESPANEL_TABLE, ATTR_VISIBLE, 1);
				
				
				
				
		
			
				free((void*)cells);
				free((void*)csv);
			}
			
			
			
		
			
			

			break;
	}
	return 0;
}




//------------------------------------------------------------------------------
// Select result file
//
int CVICALLBACK btn_load (int panel, int control, int event,
						  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			
			// open result file
			char path[MAX_PATH];
			if(FileSelectPopup(resfld,"*.info","*.info","Select result file",VAL_LOAD_BUTTON,0,0,1,1,path) 
					== VAL_EXISTING_FILE_SELECTED);
			{
				
				// extract folder path
				char *p = strrchr(path, '\\');
    			if(p) *p = '\0';
				
				// remember last result folder
				strcpy(resfld,path);
				
				// write file path to the box
				SetCtrlVal(panel,TRESPANEL_PATH_RES,path);
			}

			break;
	}
	return 0;
}


//------------------------------------------------------------------------------
// Select result file
//
int CVICALLBACK btn_exit_rv(int panel, int control, int event,
						  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			
			panelCB(panel, EVENT_CLOSE, NULL, 0, 0);

			break;
	}
	return 0;
}
