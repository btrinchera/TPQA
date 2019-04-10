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
#include "qwtb_alg_select.h"
#include "Matlab Module.h"

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

char **alg_list = NULL;
char *alg_list_csv = NULL;
int alg_list_n = 0;


//==============================================================================
// Static functions

//==============================================================================
// Global variables
char **par_names = NULL;

// QWTB processing setup
TTWMqwtbCfg cfg_qwtb;


//==============================================================================
// Global functions


//==============================================================================
// UI callback function prototypes



/// HIFN Exit when the user dismisses the panel.
int CVICALLBACK cb_alg_sel_panel(int panel, int event, void *callbackData,
		int eventData1, int eventData2)
{
	if(event == EVENT_CLOSE)
	{
		DiscardPanel(panel);
	}
	else if(event == EVENT_GOT_FOCUS)
	{
		static int first_time = 1;
		
		if(first_time)
		{
			first_time = 0;
			// first run - load algorithms
			btn_proc_load_algs(panel, PROCALGSEL_BTN_LOAD_ALGS, EVENT_COMMIT, NULL, 0, 0);
		}
		
		
		if(alg_list && alg_list_csv && alg_list_n)
		{
			// clear result selectors			
			ClearListCtrl(panel,PROCALGSEL_RING_SEL_ALG);
			
			// fill algorithms list with last known list
			for(int k = 0;k < alg_list_n;k++)
				InsertListItem(panel,PROCALGSEL_RING_SEL_ALG,k,alg_list[k],k);

		}
		
	}
	
	return 0;
}


//------------------------------------------------------------------------------
// load algorithms list
int CVICALLBACK btn_proc_load_algs (int panel, int control, int event,
									void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			
			ResetTextBox(panel, PROCALGSEL_TWMPARDESC, "Please wait, loading list of algorithms...");
			
			if(alg_list)
				free((void*)alg_list);
			alg_list = NULL;
			if(alg_list_csv)
				free((void*)alg_list_csv);
			alg_list_csv = NULL;
			
			
			twm_get_alg_list(&mlink,NULL,&alg_list_csv,NULL);
			
			// clear result selectors			
			ClearListCtrl(panel,PROCALGSEL_RING_SEL_ALG);
			
			// fill algorithms list
			int rows,cols;
			alg_list_n = csv_parse(alg_list_csv,';','\t',&rows,&cols,NULL);
			alg_list = (char**)malloc(alg_list_n*sizeof(char*));
			csv_parse(alg_list_csv,';','\t',&rows,&cols,alg_list);
			for(int k = 0;k < alg_list_n;k++)
				InsertListItem(panel,PROCALGSEL_RING_SEL_ALG,k,alg_list[k],k);
			
			
			ResetTextBox(panel, PROCALGSEL_TWMPARDESC, "Done.");

			break;
	}
	return 0;
}


//------------------------------------------------------------------------------
// load selected algorithm
int CVICALLBACK ring_alg_sel (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_VAL_CHANGED:

			btn_proc_load_alg(panel, 0, EVENT_COMMIT, NULL, 0, 0);
			
			break;
	}
	return 0;
}
//------------------------------------------------------------------------------
// load selected algorithm
int CVICALLBACK btn_proc_load_alg (int panel, int control, int event,
								   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:

			int rows,cols;
			int cc;
			char **cells;
			
			int cnt;
			GetNumListItems(panel,PROCALGSEL_RING_SEL_ALG,&cnt);
			
			
			if(cnt)
			{
				int alg_id;
				GetCtrlVal(panel,PROCALGSEL_RING_SEL_ALG,(void*)&alg_id);
				char alg_id_str[1024] = {'\0'};
				GetLabelFromIndex(panel,PROCALGSEL_RING_SEL_ALG,alg_id,alg_id_str);
			
				char *par_tab;
				char *par_list;
				int has_ui;
				int is_diff;
				int is_multi;
				int unc_guf;
				int unc_mcm;
				twm_get_alg_info(&mlink, alg_id_str, NULL, &par_tab, &par_list, &has_ui, &is_diff, &is_multi, &unc_guf, &unc_mcm);
				
				// show flags
				SetCtrlVal(panel, PROCALGSEL_LED_ALG_UI, has_ui);
				SetCtrlVal(panel, PROCALGSEL_LED_ALG_DIFF, is_diff);
				SetCtrlVal(panel, PROCALGSEL_LED_ALG_MULTI, is_multi);
				
				// --- uncertainty menu
				ClearListCtrl(panel,PROCALGSEL_RING_UNC_CALC_SEL);
				int num = 0;
				InsertListItem(panel,PROCALGSEL_RING_UNC_CALC_SEL, num,"none",num); num++;
				if(unc_guf)
					InsertListItem(panel,PROCALGSEL_RING_UNC_CALC_SEL, num,"guf",num); num++;
				if(unc_mcm)
					InsertListItem(panel,PROCALGSEL_RING_UNC_CALC_SEL, num,"mcm",num); num++;
				SetCtrlIndex(panel,PROCALGSEL_RING_UNC_CALC_SEL,0);
				
			
				// --- parse and fill parameters list
				cc = csv_parse(par_tab,'\t','\n',&rows,&cols,NULL);
				cells = (char**)malloc(cc*sizeof(char*));
				csv_parse(par_tab,'\t','\n',&rows,&cols,cells);
				
				// disable table refresh
				SetCtrlAttribute(panel, PROCALGSEL_PARTAB, ATTR_VISIBLE, 0);
			
				// refresh table data size
				DeleteTableColumns(panel,PROCALGSEL_PARTAB,1,-1);
				DeleteTableRows(panel,PROCALGSEL_PARTAB,1,-1);
				InsertTableRows(panel,PROCALGSEL_PARTAB,1,rows-1,VAL_CELL_STRING);
				InsertTableColumns(panel,PROCALGSEL_PARTAB,1,cols-1,VAL_CELL_STRING);
		
				// refresh headers
				SetTableRowAttribute (panel, PROCALGSEL_PARTAB, -1, ATTR_USE_LABEL_TEXT, 1);
				for(int r = 1;r<rows;r++)
					SetTableRowAttribute(panel,PROCALGSEL_PARTAB,r,ATTR_LABEL_TEXT,cells[r*cols]);
				SetTableColumnAttribute (panel, PROCALGSEL_PARTAB, -1, ATTR_USE_LABEL_TEXT, 1);
				for(int c = 1;c<cols;c++)
					SetTableColumnAttribute(panel,PROCALGSEL_PARTAB,c,ATTR_LABEL_TEXT,cells[c]);
			
				// refresh data				
				for(int r = 1;r<rows;r++)
					SetTableCellRangeVals(panel,PROCALGSEL_PARTAB,MakeRect(r,1,1,cols-1),&cells[r*cols+1],VAL_ROW_MAJOR);
			
				// autoscale
				for(int c = 1;c<cols;c++)
					SetColumnWidthToWidestCellContents(panel,PROCALGSEL_PARTAB,c);
			
				// inser empty rows
				InsertTableColumns(panel,PROCALGSEL_PARTAB,cols,20,VAL_CELL_STRING);
				
				// enable table refresh
				SetCtrlAttribute(panel, PROCALGSEL_PARTAB, ATTR_VISIBLE, 1);
				
				
				
				
			
				free((void*)par_tab);
				
				
				// --- parse parameter names
				cc = csv_parse(par_list,'\t','\n',&rows,&cols,NULL);
				cells = (char**)malloc(cc*sizeof(char*));
				csv_parse(par_list,'\t','\n',&rows,&cols,cells);
				
				if(par_names)
				{
					// get rid of old list from memory
					char **p = par_names;
					while(*p)
						free((void*)*p++);
					free((void*)par_names);
					par_names = NULL;
				}
				// allocate list of parameter names
				par_names = (char**)malloc((cols+1)*sizeof(char*));
				memset((void*)par_names,0,(cols+1)*sizeof(char**));
				// fill the list with parameter names
				char **p = par_names;
				for(int k = 0;k < cols;k++)
				{
					*p = (char*)malloc((strlen(cells[k])+1)*sizeof(char));
					strcpy(*p++,cells[k]);
				}
				
				free((void*)par_list);
			}
			
			break;
	}
	return 0;
}


//------------------------------------------------------------------------------
// parameter selection
int CVICALLBACK par_tab_mod (int panel, int control, int event,
							 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_SELECTION_CHANGE:
			
			if(par_names)
			{
				Rect cell;
				GetTableSelection(panel, PROCALGSEL_PARTAB, &cell);

				char **p = par_names;
				while(*p++);
				int max_names = (int)(p - par_names);
				
				ResetTextBox(panel, PROCALGSEL_TWMPARDESC, "");
				if(cell.top > 0 && cell.top < max_names)
					SetCtrlVal(panel, PROCALGSEL_TWMPARDESC, par_names[cell.top-1]);
			}

			break;
	}
	return 0;
}


//------------------------------------------------------------------------------
// algorithm selection done - fill in setup structure and leave
int CVICALLBACK btn_alg_sel_done (int panel, int control, int event,
								  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			
			
			
			memset((void*)&cfg_qwtb,0,sizeof(cfg_qwtb));
			
			// get algorithm selection
			int cnt;
			GetNumListItems(panel,PROCALGSEL_RING_SEL_ALG,&cnt);
			if(!cnt)
				break;
			int alg_id;
			GetCtrlVal(panel,PROCALGSEL_RING_SEL_ALG,(void*)&alg_id);
			GetLabelFromIndex(panel,PROCALGSEL_RING_SEL_ALG,alg_id,cfg_qwtb.alg_id);
			
			// do all records at once (nope, we have no support for multiple records yet)
			cfg_qwtb.all_rec = 0;
			
			// fixed level of confidence
			cfg_qwtb.loc = 0.95;
			
			// monte carlo cycles
			GetCtrlVal(panel,PROCALGSEL_TWM_MCM_ITER,(void*)&cfg_qwtb.mcm_cyc);
			
			// uncertainty mode
			GetNumListItems(panel,PROCALGSEL_RING_UNC_CALC_SEL,&cnt);
			if(!cnt)
				break;
			int unc_id;
			GetCtrlVal(panel,PROCALGSEL_RING_UNC_CALC_SEL,(void*)&unc_id);
			GetLabelFromIndex(panel,PROCALGSEL_RING_UNC_CALC_SEL,unc_id,(void*)&cfg_qwtb.unc_mode);
			
			// --- here should be parsed of QWTB parameters
			
			int rows,cols;
			GetNumTableRows(panel, PROCALGSEL_PARTAB, &rows);
			GetNumTableColumns(panel, PROCALGSEL_PARTAB, &cols);
			
			int par = 0;
			for(int r = 0;r < rows;r++)
			{
				Point cell;
				char row[TWMMAXSTR];
				char str[TWMMAXSTR];
				
				row[0] = '\0';
				for(int c = 0;c < cols;c++)
				{
					str[0] = '\0';
					cell.x = c+1;
					cell.y = r+1;
					GetTableCellVal(panel, PROCALGSEL_PARTAB, cell, (void*)str);
					int noempty = (int)strlen(str);
					if(strlen(row) && noempty)
						strcat(row,"; ");
					if(noempty)
						strcat(row, str);
				}
				
				if(strlen(row))
				{
					// store parameter name
					GetTableRowAttribute(panel,PROCALGSEL_PARTAB,r+1,ATTR_LABEL_TEXT,str);
					char *pstr = strchr(str,' ');
					if(pstr)
						*pstr = '\0';
					strcpy(cfg_qwtb.par_names[par],str);

					// store parameter data
					strcpy(cfg_qwtb.par_data[par],row);
					
					par++;
				}
			}
			
			// parameters count
			cfg_qwtb.par_count = par;
			
			
			
			
			
			
			// we are here, so leave panel
			//DiscardPanel(panel);
			cb_alg_sel_panel(panel, EVENT_CLOSE, NULL, 0, 0);
			

			break;
	}
	return 0;
}

