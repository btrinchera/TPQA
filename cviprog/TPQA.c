/********************************************************************************
/  Configured Acquisition Example, uses GenericMultiDeviceConfiguredAcquisitionTClk.h generic file
/*******************************************************************************/

#include <ansi_c.h>
#include <windows.h>
#include "regexpr.h"
#include "inifile.h"
#include "niModInstCustCtrl.h"
#include <Shlwapi.h>
#include <cvirte.h>		
#include <userint.h>
#include "processing_panel.h"
#include "mlink.h"
#include "twm_matlab.h"
#include "matlab_globals.h"
#include "utils.h"
#include "toolbox.h"
#include <analysis.h>
#include <formatio.h>
#include "TPQA.h"
#include "GenericMultiDeviceConfiguredAcquisitionTClk.h"
#include "visa.h"
#include "tpqa_globals.h"


//==============================================================================
// Constants

#define INIFILE "config.ini"

//==============================================================================
// Types

//==============================================================================
// Static global variables

static int panelHandle_MC = 0;

char pathMeasData[MAX_PATH];

//==============================================================================
// Global functions


/*********************************************************************************************************/

  
/** Global variable for DMMs **/

static ViStatus status;
static ViSession defaultRM;
static ViSession DMM1Handle, DMM2Handle;
static ViSession inst;


char filePath[500];
char filePathMeas[500];
FILE* fileHandle2;
int con_init=0;
int count=0;
int RecordsAcquired=0;

int fileFlagMeas;


// Forward declaration
int DisplayErrorMessageInGUI (ViInt32 error,
                              ViConstString errorMessage);

static int panelHandle, panelDataProcess, panelDigDMM, panelAbout;

//static int panelHWCorrections = -1;
static int panelHWCorrections; 

static int panelHandleGrV, panelHandleGrI;


int flag_DataProcess, flag_HwCorrections;

int SS_Controll;
int SSC=0;
ViBoolean stop = NISCOPE_VAL_FALSE;

double *U1, *U1_ph, *U1_F, *I1, *I1_ph, *I1_F, *d_Ph, *P1, *Q1, *S1, *PF1, *THD_U1, *THD_I1;
double V_rms=0.0, I_rms=0.0; 
double R_VD=0.0,  Ph_VD=0.0, R_SH=0.0,Ph_SH=0.0;


Point NewCell;
int NewCellV, NewCellVPh, NewCellVfreq, NewCellI, NewCellIPh, NewCellIfreq,NewCellP, NewCellQ, NewVellS, NewCellPF, NewCellRatio;  

// last corrections folder
char corr_fld[MAX_PATH];

// ###
// transducer parameters lists from GUI, so far only for two transducers, but can be extended up to 6
const int trlist_path[TWMMAXTR] = {Transducer_TEXTpathRVD_1, Transducer_TEXTpathCSH_1};
const int trlist_phid[TWMMAXTR] = {Transducer_RVD_PHID_1, Transducer_SH_PHID_1};
const int trlist_btnp[TWMMAXTR] = {Transducer_LoaddataRVD_1, Transducer_LoaddataCSH_1};
const int trlist_nomg[TWMMAXTR] = {Transducer_Ratio_RVD_1, Transducer_Ratio_CSH_1};
const int trlist_name[TWMMAXTR] = {Transducer_Name_RVD_1, Transducer_Name_CSH_1};
const int trlist_trsn[TWMMAXTR] = {Transducer_SN_RVD_1, Transducer_SN_CSH_1};
const int trlist_adch[TWMMAXTR] = {Transducer_RVD_ADC_HP_1, Transducer_CSH_ADC_HP_1};
const int trlist_adcl[TWMMAXTR] = {Transducer_RVD_ADC_LP_1, Transducer_CSH_ADC_LP_1};
const int trlist_trph[TWMMAXTR] = {Transducer_Phase_RVD_2, Transducer_Phase_CSH};


// Prototype functions
int DataElaboration(double *diff_ADC1, double *diff_ADC2, ViReal64 actualSampleRate, ViInt32 actualRecordLength);
//int DataElaboration(double diff_ADC1[], double diff_ADC2[], ViReal64 actualSampleRate, ViInt32 actualRecordLength);
int PlotDataChart(int Check_statistic, double U1_rms, double U1_Ph, double U1_freq, double I1_rms, double I1_Ph, double I1_freq, double U1I1_dPh, double P, double Q, double S, double Ratio);
void DMMs_GenericConfiguredAcquisition(void); // Generic acquisition function for handling of synchronized DMMs-HP3458A high-precision digitizers type
int CfgChannelDMM(ViSession Inst_Handle, char *Master_Slave);
int SamplingDMM(ViSession Inst_Handle, char *Master_Slave);


// Load the panel, run the interface, discard the panel 
int main (int argc, char *argv[])
{
   if (InitCVIRTE (0, argv, 0) == 0)
      return -1;  /* out of memory */
   if ((panelHandle = LoadPanel (0, "TPQA.uir", PANEL)) < 0)
      return -1;
   
   // ###note: needed here to cleanup some global variable buffer!
   GetCorrectionsFromGUI(-1,NULL);
   
   
	// get app directory
	char appdir[MAX_PATHNAME_LEN];
	GetProjectDir(appdir);   	

	// build ini path
	char cini[MAX_PATH];
	strcpy(cini,appdir);
	strcat(cini,"\\config.ini");

	// ### load last measurement path
	char meas_fld[MAX_PATH];
	int ret = GetPrivateProfileString("PATH","twm_last_meas_folder","",meas_fld,MAX_PATH,cini);
	if(ret)
		SetCtrlVal(panelHandle, PANEL_pathMeasData, (void*)meas_fld);
	
	// ### get last corrections folder
	GetPrivateProfileString("PATH","twm_last_corr_folder","",corr_fld,MAX_PATH,cini);

   
	niModInstCVICust_NewCtrl (panelHandle, PANEL_resourceName1, "niScope");
	niModInstCVICust_NewCtrl (panelHandle, PANEL_resourceName2, "niScope");

	DisplayPanel (panelHandle);
	RunUserInterface ();
	
	niModInstCVICust_DiscardCtrl(panelHandle, PANEL_resourceName1);
	niModInstCVICust_DiscardCtrl(panelHandle, PANEL_resourceName2); 
	
	// ### store last measurement path
	GetCtrlVal(panelHandle, PANEL_pathMeasData, (void*)meas_fld);
	WritePrivateProfileString("PATH","twm_last_meas_folder",meas_fld,cini);
	
	// ### store last corrections folder
	WritePrivateProfileString("PATH","twm_last_corr_folder",corr_fld,cini);
	
	
	DiscardPanel (panelHandle);
   return 0;
}

 
// Callback for the acquisition button
int CVICALLBACK Acquisition (int panel, int control, int event,
      void *callbackData, int eventData1, int eventData2)
{
   
    typedef double *prova;
	
	int macrosetup_flag=0;

	switch (event)
   {
      case EVENT_COMMIT:
		  
		  GetCtrlVal(panelHandle, PANEL_SaveMeasData, &fileFlagMeas);
		  
		  if (fileFlagMeas==1)
           {
             FileSelectPopup ("", "*.txt", "",
                                "Write Power Measurements", VAL_OK_BUTTON,
                                0, 0, 1, 0, filePathMeas);
			 
			 fileHandle2 = fopen (filePathMeas, "a");
			 con_init=0;  //Condizione per intestazione file
	       }
		
		  
		 GetCtrlVal(panelHandle, PANEL_ModularSetup, &macrosetup_flag);
		  // obtain measurement folder path
		 char meas_fld[MAX_PATH];
		 GetCtrlVal(panel, PANEL_pathMeasData, meas_fld);
         
		 switch(macrosetup_flag)
		 {
				 
			 case 0: //Macrosetup based on PXI 5922 high speed digitizers for HF power and PQ measurements	 
		 
		         DisplayErrorMessageInGUI (VI_SUCCESS, "");
         		 SSC=0;
		 		 // Do not stop until the stop button is pressed
                 stop = NISCOPE_VAL_FALSE;
				 // Disable the acquire button
                 SetCtrlAttribute (panelHandle, PANEL_ACQUIRE, ATTR_DIMMED, 1);
		         SetCtrlAttribute (panelHandle, PANEL_SingleShot, ATTR_DIMMED, 1);
		  
			
                 // Call the generic function to perform an acquisition
                 niScope_GenericMultiDeviceConfiguredAcquisitionTClk(meas_fld);
                 // Enable the acquire button
                 SetCtrlAttribute (panelHandle, PANEL_ACQUIRE, ATTR_DIMMED, 0);
		         SetCtrlAttribute (panelHandle, PANEL_SingleShot, ATTR_DIMMED, 0); 
		 
	            break;
			  
			 case 1:  //Macrosetup based on DMMs-HP3458A high precision digitizers for LF power and PQ measurements  or for future digitizers
				      // DMM_MultideviceConfiguredAcquisition();
			 	      // Call the generic function to perform an acquisition usin two-DMMs 
			 	      //	DMMs_GenericConfiguredAcquisition();
			 
			 break;	 
		 }
	 break;
   }		
	  
   return 0;

}

// Callback for the single shot button 

  int CVICALLBACK SingleShot (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
    
  {
	  switch (event)
	{
		case EVENT_COMMIT:
		
			
			// obtain measurement folder path
		 	char meas_fld[MAX_PATH];
		 	GetCtrlVal(panel, PANEL_pathMeasData, meas_fld);
			
		 
		 GetCtrlVal(panelHandle, PANEL_SaveMeasData, &fileFlagMeas);
		// Button pressed, clear the message panel
         DisplayErrorMessageInGUI (VI_SUCCESS, "");
         
		 SSC=1;
         // Disable the acquire button
         SetCtrlAttribute (panelHandle, PANEL_ACQUIRE, ATTR_DIMMED, 1);
		 SetCtrlAttribute (panelHandle, PANEL_SingleShot, ATTR_DIMMED, 1); 
         // Call the generic function to perform an acquisition
           niScope_GenericMultiDeviceConfiguredAcquisitionTClk(meas_fld);
		   
		   // Enable the acquire button
         SetCtrlAttribute (panelHandle, PANEL_ACQUIRE, ATTR_DIMMED, 0);
		 SetCtrlAttribute (panelHandle, PANEL_SingleShot, ATTR_DIMMED, 0); 
		 
		 break;
	}
	return 0;
}


// Event for single shot control termination
int SingleShotControll (int *SS_Controll) 
{
	
	if (SSC==1)
	 {
	   *SS_Controll=1;
	   stop=NISCOPE_VAL_TRUE;
	 }
	
	return 0;
}
	  
// Callback for the "X" button
int CVICALLBACK Shutdown (int panel, int control, int event,
      void *callbackData, int eventData1, int eventData2)
{
   switch (event)
      {
      case EVENT_COMMIT:
         // Stop before shut down
         stop = NISCOPE_VAL_TRUE;
		 
         // try to close Matlab just in case it is still running...
		 mlink_close(&mlink,1);
		 
         
		 QuitUserInterface (0);
         
		 break;
      }
   return 0;
}

// Callback for the stop button
int CVICALLBACK Stop (int panel, int control, int event,
      void *callbackData, int eventData1, int eventData2)
{
   switch (event)
      {
      case EVENT_COMMIT:
         // change the flag to true
         stop = NISCOPE_VAL_TRUE;
		 //fclose(fileHandle);
		 
		 if(fileFlagMeas==1)
		 {
		   fclose(fileHandle2);
		 
         }
		 
		 break;
      }
   return 0;
}

#define ni_res_empty(res) (strcmp(res,"NIScope - Resource") == 0 || strlen(res) == 0)


// Obtain the resource names of the device from the panel
int GetResourceNamesFromGUI (resourceNameType resourceName)
{
   
	char resourceName1[100];
	char resourceName2[100];
	char defaultResource[100];
	
	// Initialize to zero length
	resourceName1[0]='\0';
	resourceName2[0]='\0';
	defaultResource[0]='\0';
	
	GetCtrlVal (panelHandle, PANEL_resourceName1, resourceName1);
	GetCtrlVal (panelHandle, PANEL_resourceName2, resourceName2);
	
	// ###some changes - it was not correct
	
	if(!ni_res_empty(resourceName1) && !ni_res_empty(resourceName2))   // both PXI1 and PXI2
	 {
	  strcat(resourceName1,", " );
	  strcat(resourceName1, resourceName2);
	  strcpy(resourceName, resourceName1);
	 }
	 
	 if(!ni_res_empty(resourceName1) && ni_res_empty(resourceName2))  //Single PXI1
	 {
	  //strcat(resourceName1,", " );
	  //strcat(resourceName1, resourceName2);
	  strcpy(resourceName, resourceName1);
	 }
	 
	 if(ni_res_empty(resourceName1) && !ni_res_empty(resourceName2))  ////Single PXI2
	 {
	  //strcat(resourceName1,", " );
	  //strcat(resourceName1, resourceName2);
	  strcpy(resourceName, resourceName2);
	 }
	

   return 0;
}


// Obtain corrections configuration from GUIs
int GetCorrectionsFromGUI(int panel,TTWMssnInf *info)
{
	// local static copy of info session
	// note this is used to hold the correction setup when it is called from the corrections panel
	// then, when called without panel handle, it will return last loaded setup
	static TTWMssnInf info_buf;
	
	// clear info buffer
	if(panel < 0 || panel > 0)
		memset((void*)&info_buf,0,sizeof(TTWMssnInf));
	
	if(panel > 0)
	{
		
		// get panels with controls
		int trpan,digpan;
		GetPanelHandleFromTabPage (panel,  PANEL_HW_TAB_Corrections, 0, &trpan);
		GetPanelHandleFromTabPage (panel,  PANEL_HW_TAB_Corrections, 1, &digpan);
		
		// for each potential transducer
		for(int k = 0;k < TWMMAXTR;k++)
		{
			if(!trlist_path[k])
				break; // all tranducers done

			// get transducer path
			char path[TWMMAXSTR];
			int uid = trlist_path[k];
			GetCtrlVal(trpan, trlist_path[k], (void*)path);
		
			if(!strlen(path))
				break; // all transducers loaded
		
			// store transducer path
			strncpy(info_buf.tr_corr[k], path, TWMMAXSTR);
		
			// store phase index number
			GetCtrlVal(trpan, trlist_phid[k], (void*)&info_buf.tr_phase[k]);
			
			// store channel indices
			GetCtrlVal(trpan, trlist_adch[k], (void*)&info_buf.tr_map[k][0]);
			GetCtrlVal(trpan, trlist_adcl[k], (void*)&info_buf.tr_map[k][1]);
			
			// update transducers counter
			info_buf.tr_count++;
		}
		
		// store digitizer correction
		GetCtrlVal(digpan, Digitizer_TEXT_DIG_PATH, info_buf.dig_corr);
		
	}	
	else if(panel == 0)
	{
		// -- return from buffer
		
		info->tr_count = info_buf.tr_count;
		for(int k = 0;k < info->tr_count;k++)
		{
			strcpy(info->tr_corr[k], info_buf.tr_corr[k]);
			info->tr_phase[k] = info_buf.tr_phase[k];
			info->tr_map[k][0] = info_buf.tr_map[k][0];
			info->tr_map[k][1] = info_buf.tr_map[k][1];
		}
		strcpy(info->dig_corr, info_buf.dig_corr);
		
	}

	
	return(0);
}		   

// Obtain the necessary parameters from the panel
int GetParametersFromGUI (ViChar* channel,
                             ViInt32* acquisitionType,
                             ViReal64* verticalRange,
                             ViReal64* verticalOffset,
                             ViInt32* verticalCoupling,
                             ViReal64* probeAttenuation,
                             ViReal64* inputImpedance,
                             ViReal64* maxInputFrequency,
                             ViReal64* minSampleRate,
                             ViInt32* minRecordLength,
                             ViReal64* timeout,
                             ViInt32 *numRecords,
                             ViReal64 *refPos,
                             ViInt32* triggerType,
                             ViChar *triggerSource,
                             ViInt32 *triggerCoupling,
                             ViInt32 *triggerSlope,
                             ViReal64 *triggerLevel,
                             ViReal64 *triggerHoldoff,
                             ViReal64 *triggerDelay,
                             ViInt32 *windowMode,
                             ViReal64 *lowWindowLevel,
                             ViReal64 *highWindowLevel,
                             ViReal64 *hysteresis,
							 ViInt32 *fileFlag)
{
   // Intermediate variables
   ViChar panelTriggerSource[MAX_STRING_SIZE];
   ViInt32 panelAcquisitionType;
   ViInt32 panelVerticalCoupling;
   ViReal64 panelTriggerLevel;
   ViInt32 panelTriggerCoupling;
   ViInt32 panelTriggerSlope;
   ViReal64 panelTriggerHoldoff;
   ViReal64 panelTriggerDelay;
   ViReal64 panelHysteresis;
   ViReal64 panelLowLevelWindow;
   ViReal64 panelHighLevelWindow;
   ViInt32 panelWindowMode;
   ViInt32 panelEnforceRealTime;
   
   
   // Get values from panel
   GetCtrlVal(panelHandle, PANEL_channelName, channel);
   GetCtrlVal(panelHandle, PANEL_acquisitionType, &panelAcquisitionType);
  
   GetCtrlVal(panelHandle, PANEL_SaveDate, fileFlag);
   
   // Preparo il file dove salvare i dati
   
   
   switch(panelAcquisitionType)
   {
   case 0:
      *acquisitionType = NISCOPE_VAL_NORMAL;
      break;
   case 1:
      *acquisitionType = NISCOPE_VAL_FLEXRES;
      break;
   default:
      *acquisitionType = NISCOPE_VAL_NONE;
      break;
   }
   
   GetCtrlVal(panelHandle, PANEL_refPosition, refPos);
   GetCtrlVal(panelHandle, PANEL_numRecords, numRecords);
   GetCtrlVal(panelHandle, PANEL_verticalRange, verticalRange);
   GetCtrlVal(panelHandle, PANEL_verticalOffset, verticalOffset);
   GetCtrlVal(panelHandle, PANEL_verticalCoupling, &panelVerticalCoupling);
   
   

   switch(panelVerticalCoupling)
   {
   case 0:
      *verticalCoupling = NISCOPE_VAL_AC;
      break;
   case 1:
      *verticalCoupling = NISCOPE_VAL_DC;
      break;
   case 2:
      *verticalCoupling = NISCOPE_VAL_GND;
      break;
   }

   GetCtrlVal(panelHandle, PANEL_probeAttenuation, probeAttenuation);
   GetCtrlVal(panelHandle, PANEL_inputImpedance, inputImpedance);
   GetCtrlVal(panelHandle, PANEL_maxInputFrequency, maxInputFrequency);
   GetCtrlVal(panelHandle, PANEL_minSampleRate, minSampleRate);
   GetCtrlVal(panelHandle, PANEL_minRecordLength, minRecordLength);

   GetCtrlVal(panelHandle, PANEL_timeout, timeout);
   GetCtrlVal(panelHandle, PANEL_triggerType, triggerType);
   
   GetCtrlVal(panelHandle, PANEL_triggerLevel, triggerLevel);
   GetCtrlVal(panelHandle, PANEL_triggerCoupling, &panelTriggerCoupling);
   
   switch(panelTriggerCoupling)
   {
   case 0:
      *triggerCoupling = NISCOPE_VAL_AC;
      break;
   case 1:
      *triggerCoupling = NISCOPE_VAL_DC;
      break;
   case 2:
      *triggerCoupling = NISCOPE_VAL_HF_REJECT;
      break;
   case 3:
      *triggerCoupling = NISCOPE_VAL_LF_REJECT;
      break;
   case 4:
      *triggerCoupling = NISCOPE_VAL_AC_PLUS_HF_REJECT;
      break;      
   }

   GetCtrlVal(panelHandle, PANEL_triggerSlope, &panelTriggerSlope);
   
   switch(panelTriggerSlope)
   {
   case 0:
      *triggerSlope = NISCOPE_VAL_POSITIVE;
      break;
   case 1:
      *triggerSlope = NISCOPE_VAL_NEGATIVE;
      break;
   }

   GetCtrlVal(panelHandle, PANEL_triggerSource, triggerSource);
   GetCtrlVal(panelHandle, PANEL_triggerHoldoff, triggerHoldoff);
   GetCtrlVal(panelHandle, PANEL_triggerDelay, triggerDelay);
   GetCtrlVal(panelHandle, PANEL_hysteresis, hysteresis );
   GetCtrlVal(panelHandle, PANEL_lowLevelWindow, lowWindowLevel);
   GetCtrlVal(panelHandle, PANEL_highLevelWindow, highWindowLevel);
   GetCtrlVal(panelHandle, PANEL_windowMode, &panelWindowMode);

   switch(panelWindowMode)
   {
   case 0:
      *windowMode = NISCOPE_VAL_ENTERING_WINDOW;
      break;
   case 1:
      *windowMode = NISCOPE_VAL_LEAVING_WINDOW;
      break;
   }

    return 0;
}


// Find out wether to stop or not
int ProcessEvent (int *stopPtr)
{
   *stopPtr = stop;
   // Take care of any pending operations, like pressing the stop button
   return ProcessSystemEvents();
}

 
// Display message in the text box
int DisplayErrorMessageInGUI (ViInt32 error,
                              ViConstString errorMessage)
{
   ResetTextBox (panelHandle, PANEL_errorMessage, errorMessage);
   return 0;
}

int ClearPlots()
{
   // Delete all the current plots
   DeleteGraphPlot (panelHandle, PANEL_acquisitionGraph, -1, VAL_DELAYED_DRAW);
   DeleteGraphPlot (panelHandle, PANEL_acquisitionDiff, -1, VAL_DELAYED_DRAW);
   
   //DeleteGraphPlot (panelHandle, PANEL_acquisitionGraph_UHF, -1, VAL_DELAYED_DRAW);
   // DeleteGraphPlot (panelHandle, PANEL_acquisitionGraph_IHF, -1, VAL_DELAYED_DRAW);
   return 0;
}

// Plot the waveforms
int PlotWfms (ViInt32 numWaveforms,
              ViReal64 *waveformPtr,
              struct niScope_wfmInfo *wfmInfoPtr,
              ViReal64 actualSampleRate,
              ViInt32 actualRecordLength, 
			  ViReal64 **SaveWavPtr, 
			  ViInt32 Size, 
			  ViInt32 Sessions)
{
   double xmin, xmax;
   
   int mode;
   int controll;
   int DispWavePoints;
   double *diff_ADC1, *diff_ADC2;
  
   
   if (waveformPtr && wfmInfoPtr)
   {
      ViReal64 triggerLevel;
      int i;
      int graphColors[4] = {VAL_BLUE, VAL_RED, VAL_WHITE, VAL_YELLOW}; //{VAL_RED, VAL_GREEN, VAL_BLUE, VAL_YELLOW};
      double x, y;

   
	  // Do not refresh the graphs until finished
      SetCtrlAttribute (panelHandle, PANEL_acquisitionGraph, ATTR_REFRESH_GRAPH, 0);

  	  /////// Sensori di tensione e corrente ///////////////// da mettere a posto
	  
	  GetCtrlVal (panelHandle, PANEL_R_VD, &R_VD); 
	  GetCtrlVal (panelHandle, PANEL_Ph_VD, &Ph_VD);
	  GetCtrlVal (panelHandle, PANEL_R_Shunt, &R_SH); 
	  GetCtrlVal (panelHandle, PANEL_Ph_Shunt, &Ph_SH);
	  
	  /////////////////////////////////////////////////////////
	  
	  GetCtrlVal (panelHandle, PANEL_DispWavPoints, &DispWavePoints);
	  
	  
	  for(i=0; i<numWaveforms; i++)
      {
        // Plot waveform
        PlotWaveform (panelHandle, PANEL_acquisitionGraph, waveformPtr,
                  wfmInfoPtr[i].actualSamples, VAL_DOUBLE, 1.0, 0.0, wfmInfoPtr[i].relativeInitialX,
                  wfmInfoPtr[i].xIncrement, VAL_THIN_LINE, VAL_NO_POINT,
                  VAL_SOLID, 1, graphColors[i%4]);
	
		 // Increment waveform pointers to the next waveform
		 waveformPtr = waveformPtr + wfmInfoPtr[i].actualSamples;
         
	  }	 

      // Display the cursor
       GetAxisScalingMode (panelHandle, PANEL_acquisitionGraph, VAL_XAXIS,
                     &mode, &xmin, &xmax);
                     
       GetCtrlVal(panelHandle, PANEL_triggerLevel, &triggerLevel);
                     
      //  if (xmin <= 0.0 && 0.0 <= xmax)
      //    SetGraphCursor (panelHandle, PANEL_acquisitionGraph, 1, 0.0, triggerLevel);
      //  else if (xmin > 0.0)
      //     SetGraphCursor (panelHandle, PANEL_acquisitionGraph, 1, xmin, triggerLevel);
      //  else
      //     SetGraphCursor (panelHandle, PANEL_acquisitionGraph, 1, xmax, triggerLevel);
   
      // Display sample rate and record length
       SetCtrlVal (panelHandle, PANEL_actualSampleRate, actualSampleRate);
       SetCtrlVal (panelHandle, PANEL_actualRecordLength, actualRecordLength);
	   
	   SetCtrlVal (panelHandle, PANEL_RecordsAcquired, RecordsAcquired+1);
	   
	   
	  
   }
  
   /***********************************************************/
       // Mapping condition for "controll" variable
       // Size     = 2  -  4  
       // Sessions = 1  -  2
       // -----------------------------------------------
       // controll = 3  -  6
   /***********************************************************/
   controll=Size+Sessions; 
   // controll=Size;
	//Size=numberOfSessions* numWaveform; // è il numero delle colonne del vettore SaveWavPtr 
   
  switch (controll)
    {	  
	  case 3: // Case with a single PXI 5922 board - Measurement in single ended 
  	    
	   
		  diff_ADC1 = (double*) malloc (sizeof (double) * (actualRecordLength)); 
	      diff_ADC2 = (double*) malloc (sizeof (double) * (actualRecordLength));
		  
		   // Creo i segnali differenziali
		   
		  Copy1D (SaveWavPtr[0],actualRecordLength , diff_ADC1); 
		  Copy1D (SaveWavPtr[1],actualRecordLength , diff_ADC2);
		  
	
		  
		  if ( DispWavePoints<=actualRecordLength) {
				  PlotWaveform (panelHandle, PANEL_acquisitionDiff, SaveWavPtr[0],
								DispWavePoints, VAL_DOUBLE, 1.0, 0.0, 0.0, 1.0,
								VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_RED);
						
				  PlotWaveform (panelHandle, PANEL_acquisitionDiff, SaveWavPtr[1],
				  DispWavePoints, VAL_DOUBLE, 1.0, 0.0, 0.0, 1.0,
				  VAL_THIN_LINE, VAL_NO_POINT, VAL_SOLID, 1, VAL_BLUE);
		          }
          else {
			      PlotWaveform (panelHandle, PANEL_acquisitionDiff, SaveWavPtr[0],
				  actualRecordLength, VAL_DOUBLE, 1.0, 0.0, 0.0, 1.0,
				  VAL_THIN_LINE, VAL_SMALL_SOLID_SQUARE, VAL_SOLID, 1,
				  VAL_RED);
		  
		          PlotWaveform (panelHandle, PANEL_acquisitionDiff, SaveWavPtr[1],
				  actualRecordLength, VAL_DOUBLE, 1.0, 0.0, 0.0, 1.0,
				  VAL_THIN_LINE, VAL_NO_POINT, VAL_SOLID, 1, VAL_BLUE); 
	        }
		  
		   
		   if (flag_DataProcess==1)
		    {
			   
			  DataElaboration(diff_ADC1, diff_ADC2, actualSampleRate, actualRecordLength); 
		    }
		   
		 
			// Free memomory
		 free(diff_ADC1);
	     free(diff_ADC2);
		 RecordsAcquired++;
		 
		 if(stop==1)
		  {//free (prova);
			 count=0;
			 RecordsAcquired=0;
		  }
		  
	
	  break;
	  
	  case 6: // Case with two Board PXI 5922 active (for differential measure of power and PQ parameters)
	   
          // Allocating dynamic memory for differential signals 
		  
		  diff_ADC1 = (double*) malloc (sizeof (double) * (actualRecordLength)); 
	      diff_ADC2 = (double*) malloc (sizeof (double) * (actualRecordLength));
		  
		  // Creo i segnali differenziali
		  Sub1D (SaveWavPtr[0],SaveWavPtr[1] ,actualRecordLength , diff_ADC1); // Board_1: CH0-High e CH1_Low  
          Sub1D (SaveWavPtr[2],SaveWavPtr[3] ,actualRecordLength , diff_ADC2); // Board 2: CH0_High e CH1_Low
	   
		  
		  if ( DispWavePoints<=actualRecordLength) {
				  PlotWaveform (panelHandle, PANEL_acquisitionDiff, diff_ADC1,
								DispWavePoints, VAL_DOUBLE, 1.0, 0.0, 0.0, 1.0,
								VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_RED);
						
				  PlotWaveform (panelHandle, PANEL_acquisitionDiff, diff_ADC2,
				  DispWavePoints, VAL_DOUBLE, 1.0, 0.0, 0.0, 1.0,
				  VAL_THIN_LINE, VAL_NO_POINT, VAL_SOLID, 1, VAL_BLUE);
		          }
          else {
			      PlotWaveform (panelHandle, PANEL_acquisitionDiff, diff_ADC1,
				  actualRecordLength, VAL_DOUBLE, 1.0, 0.0, 0.0, 1.0,
				  VAL_THIN_LINE, VAL_SMALL_SOLID_SQUARE, VAL_SOLID, 1,
				  VAL_RED);
		  
		          PlotWaveform (panelHandle, PANEL_acquisitionDiff, diff_ADC2,
				  actualRecordLength, VAL_DOUBLE, 1.0, 0.0, 0.0, 1.0,
				  VAL_THIN_LINE, VAL_NO_POINT, VAL_SOLID, 1, VAL_BLUE); 
	        }
		  
		  
		  
		   if (flag_DataProcess==1)
		    {
			   DataElaboration(diff_ADC1, diff_ADC2, actualSampleRate, actualRecordLength);
		    }
		   

		 
         
         // Free memomory
		 free(diff_ADC1);
	     free(diff_ADC2);
		 
		 RecordsAcquired++;
		 
		 if(stop==1)
		  {//free (prova);
			 count=0;
			 RecordsAcquired=0;
		  }
			 
		 break;
 
  } // End Switch
	 
	 return 0;
}


int DataElaboration(double *diff_ADC1, double *diff_ADC2, ViReal64 actualSampleRate, ViInt32 actualRecordLength)
//int DataElaboration(double diff_ADC1[], double diff_ADC2[], ViReal64 actualSampleRate, ViInt32 actualRecordLength)
{


    //////// Local Variables ///////
	
	// Waveforms electrical parameters 
	double RatioVolt, RatioVolt_mean;
	double freq_1=0.0, amp_1=0.0, phase_1=0.0, Ph_U1=0.0, Ph_I1=0.0;
	double Act_Power=0.0, Rea_Power=0.0, S_Power=0.0, PF=0.0l, Sum_Pow=0.0;
	double V1_rms=0.0, V2_rms=0.0;
	double Amp_Rat;
	double freq_2=0.0, amp_2=0.0, phase_2=0.0, d_phase=0.0;      

	SearchType	searchType;
	

   // Harmonics
	double *AutoSpec_U, *HarmonicAmplitude_U, *HarmonicFrequency_U, df;
	double *AutoSpec_I, *HarmonicAmplitude_I, *HarmonicFrequency_I;
	double *Freq_Tones_U, *Amp_Tones_U, *Phase_Tones_U;
	double *Freq_Tones_I, *Amp_Tones_I, *Phase_Tones_I;
	unsigned short int Num_Harmonic=0, ith_Harmonic;
	int HarmAnalyzer=0; 
	int maxNumberOfTones_U, maxNumberOfTones_I;
	int l=0;
	int WindowType;
	
	// Statistic
	int Check_Statistic=0; 
	double mean_U1, Var_U1=0.0, S_U1=0.0, uA_U1=0.0, err_U1;
	double mean_U1_ph, Var_U1_ph=0.0, S_U1_ph=0.0, uA_U1_ph=0.0, err_U1_ph;
	double mean_U1_F, Var_U1_F=0.0, S_U1_F=0.0, uA_U1_F=0.0, err_U1_F;
	double mean_I1, Var_I1=0.0, S_I1=0.0, uA_I1=0.0, err_I1;
	double mean_I1_ph, Var_I1_ph=0.0, S_I1_ph=0.0, uA_I1_ph=0.0, err_I1_ph;
	double mean_I1_F, Var_I1_F=0.0, S_I1_F=0.0, uA_I1_F=0.0, err_I1_F;
	double mean_d_Ph, Var_d_Ph=0.0, S_d_Ph=0.0, uA_d_Ph=0.0, err_d_Ph;
	double mean_P1, Var_P1=0.0, S_P1=0.0, uA_P1=0.0, err_P1;
	double mean_Q1, Var_Q1=0.0, S_Q1=0.0, uA_Q1=0.0, err_Q1;
	double mean_S1, Var_S1=0.0, S_S1=0.0, uA_S1=0.0, err_S1;
	double mean_PF1, Var_PF1, S_PF1, uA_PF1, err_PF1;
    

	
	// Basic Power Quality parameters
	double THD_U, THD_U_Noise;
	double THD_I, THD_I_Noise;

	
    //	SingleToneInfo (diff_ADC1, actualRecordLength, (1./actualSampleRate), &searchType, &freq_1, &amp_1, &phase_1);
    //	SingleToneInfo (diff_ADC2, actualRecordLength, (1./actualSampleRate), &searchType, &freq_2, &amp_2, &phase_2);
	
	SingleToneInfo (diff_ADC1, actualRecordLength, (1./actualSampleRate), NULL, &freq_1, &amp_1, &phase_1);
	SingleToneInfo (diff_ADC2, actualRecordLength, (1./actualSampleRate), NULL, &freq_2, &amp_2, &phase_2);


	//***** Calculation of electrical parameters for the voltage channel
	RMS (diff_ADC1,actualRecordLength, &V1_rms);
	V_rms=V1_rms * R_VD;
	Ph_U1=(phase_1/180)*Pi(); 
	
	//***** Calculation of electrical parameters for the voltage channel
	RMS (diff_ADC2,actualRecordLength, &V2_rms);  
	I_rms=V2_rms/R_SH;
	Ph_I1= (phase_2/180)*Pi();
	
    // Phase difference CH0-CH1
	d_phase=Ph_U1-Ph_I1; 
	
	//**** Calculation of power parameters for sinusoidal waves usimg also the phase error of transducers
	Act_Power=V_rms*I_rms*cos(d_phase-Ph_VD+Ph_SH);
	
	Rea_Power= V_rms*I_rms*sin(d_phase-Ph_VD+Ph_SH);
	
	Sum_Pow=pow(Act_Power,2) + pow(Rea_Power,2);
	S_Power=sqrt(Sum_Pow);
	PF=(Act_Power/S_Power);
	
	Amp_Rat=V1_rms/V2_rms;
	
	
	GetCtrlVal (panelDataProcess, PANEL_DP_Statistic, &Check_Statistic); 
	 
	if (NewCellV==1 && Check_Statistic==0)
	 {
	     SetCtrlAttribute (panelHandleGrV, PANEL_GrV_STRIPCHART_V, ATTR_NUM_TRACES, 1);
		 SetTraceAttributeEx (panelHandleGrV, PANEL_GrV_STRIPCHART_V, 1, ATTR_TRACE_THICKNESS, 2);
		 SetTraceAttributeEx (panelHandleGrV, PANEL_GrV_STRIPCHART_V, 1, ATTR_TRACE_LG_TEXT, "Measured Value"); 
		 
		 PlotStripChart (panelHandleGrV,PANEL_GrV_STRIPCHART_V ,&V_rms, 1, 0, 0, VAL_DOUBLE);
	
	 }


	if (NewCellI==4)
	 {
	  PlotStripChart (panelHandleGrI,PANEL_GrI_STRIPCHART_I ,&I_rms, 1, 0, 0, VAL_DOUBLE); 
	 }


	//SetCtrlVal (panelHandle, PANEL_Acquired_Cycles, count+1);
	
	// Foating mean
	
	 if (flag_DataProcess == 1)
	 {
      
	  SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(1,1), V_rms);  // MakePoint(column index, row index)
	  SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(1,2), Ph_U1);
	  SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(1,3), freq_1); 
	  SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(1,4), I_rms); 
	  SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(1,5), Ph_I1); 
	  SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(1,6), freq_2);
	  SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(1,7), d_phase);
	  SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(1,8), Act_Power); 
	  SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(1,9), Rea_Power);
	  SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(1,10), S_Power);
	  SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(1,11), PF);
	  SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(1,12), (V_rms/I_rms));
	  SetCtrlVal (panelDataProcess, PANEL_DP_Acquired_Cycles, count+1);
 
	 }
 
	 
	 //// Computing of the statistic of data ////
	 //GetCtrlVal (panelDataProcess, PANEL_DP_Statistic, &Check_Statistic);
	 
	 if(Check_Statistic==0)
	 {count=0;}
 	 
	 else{
	  if (count==0)
	  { // Dynamic array allocation
		U1=(double*) malloc (sizeof (double) * 1); 
		U1_ph=(double*) malloc (sizeof (double) * 1);
		U1_F=(double*) malloc (sizeof (double) * 1); 
		I1=(double*) malloc (sizeof (double) * 1); 
		I1_ph=(double*) malloc (sizeof (double) * 1); 
		I1_F=(double*) malloc (sizeof (double) * 1); 
		d_Ph=(double*) malloc (sizeof (double) * 1); 
		P1=(double*) malloc (sizeof (double) * 1); 
		Q1=(double*) malloc (sizeof (double) * 1); 
		S1=(double*) malloc (sizeof (double) * 1);
		PF1=(double*) malloc (sizeof (double) * 1);
		THD_U1=(double*) malloc (sizeof (double) * 1); 
		THD_I1=(double*) malloc (sizeof (double) * 1);
	    
	    if (U1==NULL) {
	    printf("Error allocating memory!\n"); //print an error message
	    return 1; //return with failure
	   }
	  
		U1[0] = V_rms;
		U1_ph[0]= Ph_U1;
		U1_F[0]= freq_1; 
		I1[0]=  I_rms;
		I1_ph[0]= Ph_I1;
		I1_F[0]=  freq_2;
		d_Ph[0]= d_phase; 
		P1[0]=  Act_Power;
		Q1[0]=  Rea_Power;
		S1[0]= S_Power;
	    PF1[0]=PF;
		THD_U1[0]= 0.0;
		THD_I1[0]= 0.0;
	 
	  }
 
	 else{
 
	 U1=(double*) realloc (U1, sizeof (double) * (count+1));
	 U1_ph=(double*) realloc (U1_ph, sizeof (double) * (count+1));
	 U1_F=(double*) realloc (U1_F, sizeof (double) * (count+1));
	 I1=(double*) realloc (I1, sizeof (double) * (count+1));
	 I1_ph=(double*) realloc (I1_ph, sizeof (double) * (count+1));
	 I1_F=(double*) realloc (I1_F, sizeof (double) * (count+1));
	 d_Ph=(double*) realloc (d_Ph, sizeof (double) * (count+1));
	 P1=(double*) realloc (P1, sizeof (double) * (count+1));
	 Q1=(double*) realloc (Q1, sizeof (double) * (count+1));
	 S1=(double*) realloc (S1, sizeof (double) * (count+1));
	 PF1=(double*) realloc (PF1, sizeof (double) * (count+1));
	 THD_U1=(double*) realloc (THD_U1, sizeof (double) * (count+1));
	 THD_I1=(double*) realloc (THD_I1, sizeof (double) * (count+1));
 
	 if (U1==NULL) {
	     printf("Error reallocating memory!\n"); //print an error message
	     return 1; //return with failure
	     }
 
	 U1[count]=V_rms;
	 U1_ph[count]=Ph_U1;
	 U1_F[count]=freq_1;
	 I1[count]=I_rms;
	 I1_ph[count]=Ph_I1;
	 I1_F[count]=freq_2;
	 d_Ph[count]=d_phase;
	 P1[count]=Act_Power;
	 Q1[count]=Rea_Power;
	 S1[count]=S_Power;
	 PF1[count]=PF;
	 THD_U1[count]=0.0;
	 THD_I1[count]=0.0;
 
	 
	 StdDev (U1, count+1, &mean_U1, &S_U1);
	 Var_U1=(S_U1*S_U1)*(count+1);
	 S_U1=sqrt(Var_U1/count);
	 uA_U1=S_U1/sqrt(count+1); // experimental standard deviation of the mean 
	 err_U1=(uA_U1/mean_U1)*1e+2; // in ppm
	 SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(2,1), mean_U1);
	 SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(3,1), S_U1);
	 SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(4,1), uA_U1);
	 SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(5,1), err_U1);
	 //PlotStripChart (panelHandle,PANEL_STRIPCHART_Mean_U1 ,&mean_U1, 1, 0, 0, VAL_DOUBLE);
	 
	 if (NewCellV==1)
	 {
		 double plot_V[2];
		 plot_V[0]=V_rms;
		 plot_V[1]=  mean_U1;
		 SetCtrlAttribute (panelHandleGrV, PANEL_GrV_STRIPCHART_V, ATTR_NUM_TRACES, 2);
		 SetTraceAttributeEx (panelHandleGrV, PANEL_GrV_STRIPCHART_V, 2,ATTR_TRACE_COLOR, VAL_RED);
		 //SetTraceAttributeEx (panelHandleGrV, PANEL_GrV_STRIPCHART_V, 1, ATTR_TRACE_LG_TEXT, "Measured Value");
		 //SetPlot
		 SetTraceAttributeEx (panelHandleGrV, PANEL_GrV_STRIPCHART_V, 2, ATTR_PLOT_STYLE, VAL_CONNECTED_POINTS);
		 SetTraceAttributeEx (panelHandleGrV, PANEL_GrV_STRIPCHART_V, 2, ATTR_TRACE_LG_VISIBLE, 1);
		 SetTraceAttributeEx (panelHandleGrV, PANEL_GrV_STRIPCHART_V, 2, ATTR_TRACE_THICKNESS, 2);
		 SetTraceAttributeEx (panelHandleGrV, PANEL_GrV_STRIPCHART_V, 2, ATTR_TRACE_LG_TEXT, "Mean Value");
		 PlotStripChart (panelHandleGrV, PANEL_GrV_STRIPCHART_V, plot_V, 2, 0, 0, VAL_DOUBLE);
	 }
 

	 StdDev (U1_ph, count+1, &mean_U1_ph, &S_U1_ph);
	 Var_U1_ph=(S_U1_ph*S_U1_ph)*(count+1);
	 S_U1_ph=sqrt(Var_U1_ph/count);
	 uA_U1_ph=S_U1_ph/sqrt(count+1); // experimental standard deviation of the mean 
	 err_U1_ph=(uA_U1_ph/mean_U1_ph)*1e+2; // in ppm
	 SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(2,2), mean_U1_ph);
	 SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(3,2), S_U1_ph);
	 SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(4,2), uA_U1_ph);
	 SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(5,2), err_U1_ph);
 	  
	 
	 StdDev (U1_F, count+1, &mean_U1_F, &S_U1_F);
	 Var_U1_F=(S_U1_F*S_U1_F)*(count+1);
	 S_U1_F=sqrt(Var_U1_F/count);
	 uA_U1_F=S_U1_F/sqrt(count+1); // experimental standard deviation of the mean 
	 err_U1_F=(uA_U1_F/mean_U1_F)*1e+2; // in ppm
	 SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(2,3), mean_U1_F);
	 SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(3,3), S_U1_F);
	 SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(4,3), uA_U1_F);
	 SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(5,3), err_U1_F);
 	  
	 
	 StdDev (I1, count+1, &mean_I1, &S_I1);
	 Var_I1=(S_I1*S_I1)*(count+1);
	 S_I1=sqrt(Var_I1/count);
	 uA_I1=S_I1/sqrt(count+1); // experimental standard deviation of the mean 
	 err_I1=(uA_I1/mean_I1)*1e+2; // in ppm
	 SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(2,4), mean_I1);
	 SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(3,4), S_I1);
	 SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(4,4), uA_I1);
	 SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(5,4), err_I1);
 

	 StdDev (I1_ph, count+1, &mean_I1_ph, &S_I1_ph);
	 Var_I1_ph=(S_I1_ph*S_I1_ph)*(count+1);
	 S_I1_ph=sqrt(Var_I1_ph/count);
	 uA_I1_ph=S_I1_ph/sqrt(count+1); // experimental standard deviation of the mean 
	 err_I1_ph=(uA_I1_ph/mean_I1_ph)*1e+2; // in ppm
	 SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(2,5), mean_I1_ph);
	 SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(3,5), S_I1_ph);
	 SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(4,5), uA_I1_ph);
	 SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(5,5), err_I1_ph);

 
	 StdDev (I1_F, count+1, &mean_I1_F, &S_I1_F);
	 Var_I1_F=(S_I1_F*S_I1_F)*(count+1);
	 S_I1_F=sqrt(Var_I1_F/count);
	 uA_I1_F=S_I1_F/sqrt(count+1); // experimental standard deviation of the mean 
	 err_I1_F=(uA_I1_F/mean_I1_F)*1e+2; // in ppm
	 SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(2,6), mean_I1_F);
	 SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(3,6), S_I1_F);
	 SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(4,6), uA_I1_F);
	 SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(5,6), err_I1_F);
 	  
 
	 StdDev (d_Ph, count+1, &mean_d_Ph, &S_d_Ph);
	 Var_d_Ph=(S_d_Ph*S_d_Ph)*(count+1);
	 S_d_Ph=sqrt(Var_d_Ph/count);
	 uA_d_Ph=S_d_Ph/sqrt(count+1); // experimental standard deviation of the mean 
	 err_d_Ph=(uA_d_Ph/mean_d_Ph)*1e+2; // in ppm
	 SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(2,7), mean_d_Ph);
	 SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(3,7), S_d_Ph);
	 SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(4,7), uA_d_Ph);
	 SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(5,7), err_d_Ph);
 
 	 
	 StdDev (P1, count+1, &mean_P1, &S_P1);
	 Var_P1=(S_P1*S_P1)*(count+1);
	 S_P1=sqrt(Var_P1/count);
	 uA_P1=S_P1/sqrt(count+1); // experimental standard deviation of the mean 
	 err_P1=(uA_P1/mean_P1)*1e+2; // in ppm
	 SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(2,8), mean_P1);
	 SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(3,8), S_P1);
	 SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(4,8), uA_P1);
	 SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(5,8), err_P1);
 	 
 
	 StdDev (Q1, count+1, &mean_Q1, &S_Q1);
	 Var_Q1=(S_Q1*S_Q1)*(count+1);
	 S_Q1=sqrt(Var_Q1/count);
	 uA_Q1=S_Q1/sqrt(count+1); // experimental standard deviation of the mean 
	 err_Q1=(uA_Q1/mean_Q1)*1e+2; // in ppm
	 SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(2,9), mean_Q1);
	 SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(3,9), S_Q1);
	 SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(4,9), uA_Q1);
	 SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(5,9), err_Q1);
 	 
	 StdDev (S1, count+1, &mean_S1, &S_S1);
	 Var_S1=(S_S1*S_S1)*(count+1);
	 S_S1=sqrt(Var_S1/count);
	 uA_S1=S_S1/sqrt(count+1); // experimental standard deviation of the mean 
	 err_S1=(uA_S1/mean_S1)*1e+2; // in ppm
	 SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(2,10), mean_S1);
	 SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(3,10), S_S1);
	 SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(4,10), uA_S1);
	 SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(5,10), err_S1);
 

 
	 StdDev (PF1, count+1, &mean_PF1, &S_PF1);
	 Var_PF1=(S_PF1*S_PF1)*(count+1);
	 S_PF1=sqrt(Var_PF1/count);
	 uA_PF1=S_PF1/sqrt(count+1); // experimental standard deviation of the mean 
	 err_PF1=(uA_PF1/mean_PF1)*100; // in ppm
	 SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(2,11), mean_PF1);
	 SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(3,11), S_PF1);
	 SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(4,11), uA_PF1);
	 SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(5,11), err_PF1);
 
 
	 }
	  count++;
	 } // end if CheckStatistic 
	
	 
	 if(fileFlagMeas==1)
	        {
	 	     
			 if(con_init==0)
	            {
		          fprintf(fileHandle2,"Sample Rate: %f\t Record Length: %i\t \n", actualSampleRate, actualRecordLength); 
				  fprintf(fileHandle2,"V_rms(V)\t Ph_V(rad)\t I_rms(A)\t Ph_I(rad)\t diff_Ph(rad)=Ph_I-Ph_V\t Active_Power(W)\t React_Power(VAR)\t Appar_PowerR(VA)\t R_V_Div(Ohm)\t Ph_VDidid(rad)\t R_Shunt(Ohm)\t Ph_Shunt(rad)\t Frequency_V(Hz)\t \Frequency_I(Hz)\t \n"); 
				  con_init=1;
		        }
			  
			  fprintf(fileHandle2,"%0.12f\t %0.12f\t %0.12f\t %0.12f\t %0.12f\t %0.12f\t %0.12f\t %0.12f\t %0.10f\t %0.10f\t %0.10f\t %0.10f\t %0.14f\t %0.14f\t  \n",
		      V_rms, Ph_U1, I_rms, Ph_I1, d_phase, Act_Power, Rea_Power, S_Power, R_VD, Ph_VD, R_SH, Ph_SH, freq_1, freq_2);    
		     }		  
      		 
			 
		//////////////////////////////////////////////////////////////////////////////////////////// 
        ///////////////////// Voltage and current harmonic analyzer //////////////////////////////////////////////
	 
		 GetCtrlVal (panelDataProcess, PANEL_DP_HarmonicAnalyzer,&HarmAnalyzer);
		 
		 if (HarmAnalyzer==1)
		  { 
			 GetCtrlVal (panelDataProcess, PANEL_DP_Num_Harmonic, &Num_Harmonic);
	 		 		  
		     GetCtrlVal (panelDataProcess, PANEL_DP_ith_harmonic,&ith_Harmonic);
			 
			 AutoSpec_U = (double*) malloc (sizeof (double) * (actualRecordLength));
			 HarmonicAmplitude_U = (double*) malloc (sizeof (double) * (Num_Harmonic));
			 HarmonicFrequency_U = (double*) malloc (sizeof (double) * (Num_Harmonic));
			 
			
			 double Freq_Tones_U=0.0; 
			 double Amp_Tones_U=0.0; 
			 double Phase_Tones_U=0.0; 
 
 
			 AutoSpec_I = (double*) malloc (sizeof (double) * (actualRecordLength));
			 HarmonicAmplitude_I = (double*) malloc (sizeof (double) * (Num_Harmonic));
			 HarmonicFrequency_I = (double*) malloc (sizeof (double) * (Num_Harmonic));
			 
			 double Freq_Tones_I=0.0; 
			 double Amp_Tones_I=0.0; 
			 double Phase_Tones_I=0.0;
			 
			 GetCtrlVal (panelDataProcess, PANEL_DP_WindowHarmonic, &WindowType); 
		     SetCtrlVal (panelDataProcess, PANEL_DP_HarmonicAnalyzer, HarmAnalyzer);
		 
             //Scale the value ov voltage and current 
		     for (l=0; l<actualRecordLength; l++)
		      {
		        diff_ADC1[l]=diff_ADC1[l]*R_VD; // Voltage channel
		        diff_ADC2[l]=diff_ADC2[l]/R_SH; // Current channel
		      }
		 
		    
			 AutoPowerSpectrum(diff_ADC1, actualRecordLength, (1./actualSampleRate), AutoSpec_U, &df);
			 
		     HarmonicAnalyzer (AutoSpec_U, (actualRecordLength/2), actualRecordLength,
						  Num_Harmonic, WindowType, actualSampleRate, freq_1,
						  HarmonicAmplitude_U, HarmonicFrequency_U, &THD_U,
						  &THD_U_Noise);
		
		     DeleteGraphPlot (panelDataProcess, PANEL_DP_acquisitionGraph_UHF, -1,
					  VAL_IMMEDIATE_DRAW);
			 PlotXY (panelDataProcess, PANEL_DP_acquisitionGraph_UHF,
					 HarmonicFrequency_U, HarmonicAmplitude_U, Num_Harmonic,
					 VAL_DOUBLE, VAL_DOUBLE, VAL_VERTICAL_BAR, VAL_EMPTY_SQUARE,
					 VAL_DASH, 1, VAL_RED);
			 PlotXY (panelDataProcess, PANEL_DP_acquisitionGraph_UHF,
					 HarmonicFrequency_U, HarmonicAmplitude_U, Num_Harmonic,
					 VAL_DOUBLE, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE,
					 VAL_DASH, 1, VAL_BLUE);
			 
             //Update new value in the table 
			 SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(1,13), THD_U);
			 SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(3,12), THD_U_Noise);
	 		 
			 
			 double DF=(1/(actualRecordLength*(1/actualSampleRate)));
			 
			
			  searchType.centerFrequency=ith_Harmonic*freq_1;
			  searchType.frequencyWidth=0.005;
			  SingleToneInfo (diff_ADC1, actualRecordLength, (1./actualSampleRate), &searchType, &Freq_Tones_U, &Amp_Tones_U, &Phase_Tones_U);  
			  SingleToneInfo (diff_ADC2, actualRecordLength, (1./actualSampleRate), &searchType, &Freq_Tones_I, &Amp_Tones_I, &Phase_Tones_I);
			
	 		  SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(1,15), Amp_Tones_U/sqrt(2));
			  SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(3,15), HarmonicAmplitude_U[ith_Harmonic]);
			  SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(1,16), (Phase_Tones_U*Pi())/180.0);
			  
			  SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(2,15), HarmonicFrequency_U[ith_Harmonic]);
			  SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(2,16), Freq_Tones_U);
			  
			  // Current channel
			  AutoPowerSpectrum(diff_ADC2, actualRecordLength, (1./actualSampleRate), AutoSpec_I, &DF);
				
			  HarmonicAnalyzer (AutoSpec_I, (actualRecordLength/2), actualRecordLength,
							  Num_Harmonic, WindowType, actualSampleRate, freq_2,
							  HarmonicAmplitude_I, HarmonicFrequency_I, &THD_I,
							  &THD_I_Noise);
		
			  DeleteGraphPlot (panelDataProcess, PANEL_DP_acquisitionGraph_IHF, -1,
							  VAL_IMMEDIATE_DRAW);
			  PlotXY (panelDataProcess, PANEL_DP_acquisitionGraph_IHF, HarmonicFrequency_I,
					HarmonicAmplitude_I, Num_Harmonic, VAL_DOUBLE, VAL_DOUBLE,
					VAL_VERTICAL_BAR, VAL_EMPTY_SQUARE, VAL_DASH, 1, VAL_RED);
			  PlotXY (panelDataProcess, PANEL_DP_acquisitionGraph_IHF,
						HarmonicFrequency_I, HarmonicAmplitude_I, Num_Harmonic,
						VAL_DOUBLE, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE,
						VAL_DASH, 1, VAL_BLUE);
			
				
				//Update new value in the table
				SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(1,14), THD_I);
				SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(3,14), THD_I_Noise);				
				SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(1,17), Amp_Tones_I/sqrt(2));
				SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(3,17), HarmonicAmplitude_I[ith_Harmonic]);
			    SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(1,18), (Phase_Tones_I*Pi())/180.0);
				
				SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(2,17), HarmonicFrequency_I[ith_Harmonic]);
			    SetTableCellVal(panelDataProcess, PANEL_DP_TABLE_data, MakePoint(2,18), Freq_Tones_I);
				
		
				// Free dynamic memory /
				free(AutoSpec_U);
				free(HarmonicAmplitude_U);
				free(HarmonicFrequency_U);
				
				
				free(AutoSpec_I);
				free(HarmonicAmplitude_I);
				free(HarmonicFrequency_I);
				
	 
	        }
			 
			 
			
 return 0;
}



int PlotDataChart(int Check_statistic, double U1_rms, double U1_Ph, double U1_freq, double I1_rms, double I1_Ph, double I1_freq, double U1I1_dPh, double P, double Q, double S, double Ratio)  
{

  
	
	return 0;
}
				  
				 
				  
 
int CommitPlots()
{
   // Refresh the graph now
   RefreshGraph (panelHandle, PANEL_acquisitionGraph);
   RefreshGraph (panelHandle, PANEL_acquisitionDiff);
 
   return 0;
}

int CVICALLBACK Refresh (int panel, int control, int event,
						 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
		

			break;
	}
	return 0;
}


//////////////// HW Corrections ////////////////////////////////

int CVICALLBACK HW_CORRECTIONS (int panel, int control, int event,
								void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:

			
	  		if ((panelHWCorrections = LoadPanel (0, "TPQA.uir", PANEL_HW)) < 0)
      			return -1;
		
			
      		DisplayPanel (panelHWCorrections);
	  
	  	break;
	}
	return 0;
}

int CVICALLBACK CLOSE_Corrections (int panel, int control, int event,
								   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
		 	
			// load corrections to buffer before discarding panel
			GetCorrectionsFromGUI(panel,NULL);
			
			DiscardPanel(panel);

			break;
	}
	return 0;
}

///////////////CVI Data Processing /////////////////////////////

int CVICALLBACK DATA_Processing (int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			
			if ((panelDataProcess = LoadPanel (0, "TPQA.uir", PANEL_DP)) < 0)
            return -1;
			
			flag_DataProcess=1;
            
			SetCtrlVal (panelHandle, PANEL_flag_DataProcess, flag_DataProcess);   
			
			DisplayPanel (panelDataProcess);
			
		//	RunUserInterface ();
			
		//	DiscardPanel(panelDataProcess);
			
			

			break;
	}
	return 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// Function prototype for handling of DMMs HP3458A configured as high-precision digitizers ////////////////////////
//// Develop by B. Trinchera, INRIM, in the framework of the EMPIR 14RPOT-TracePQM, 05, November, 2018  /////////////
/////////////////// email: b.trinchera@inrim.it ////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////// LF-Setup DMMs ////////////////////////////////////

int CVICALLBACK DIGDMM (int panel, int control, int event,
						void *callbackData, int eventData1, int eventData2)
{
	ViStatus status;
	ViSession instr;
    ViFindList fList;
    ViChar InstDesc[VI_FIND_BUFLEN];
    ViUInt32 numInstrs;
	int i=0;
	
	switch (event)
	{
		case EVENT_COMMIT:
		if ((panelDigDMM = LoadPanel (0, "TPQA.uir", PANEL_DMM)) < 0)
        return -1;
        DisplayPanel (panelDigDMM);
			
		 status = viOpenDefaultRM (&defaultRM);
		 if (status < VI_SUCCESS)
          {
            printf("Could not open a session to the VISA Resource Manager!\n");
            exit (EXIT_FAILURE);
          }
		 status = viFindRsrc (defaultRM, "?*INSTR",&fList, &numInstrs, InstDesc);
		 if(numInstrs > 0)
		  {
		    //InsertListItem (panelHandle, PANEL_LISTBOX, -1, InstDesc, numInstrs);
			InsertListItem (panelDigDMM, PANEL_DMM_DMM1, -1, InstDesc, numInstrs);
			InsertListItem (panelDigDMM, PANEL_DMM_DMM2, -1, InstDesc, numInstrs);
			
			for(i=1;i<numInstrs;i++)
			  {
				viFindNext (fList, InstDesc);
				//InsertListItem (panelHandle, PANEL_LISTBOX, -1, InstDesc, numInstrs);
				InsertListItem (panelDigDMM, PANEL_DMM_DMM1, -1, InstDesc, numInstrs);
				InsertListItem (panelDigDMM, PANEL_DMM_DMM2, -1, InstDesc, numInstrs);
			  }
			}
				 
		break;
	}
	return 0;
}

int CVICALLBACK Quit_DMM (int panel, int control, int event,
						  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
	  case EVENT_COMMIT:
	  status = viClose(DMM1Handle);
	  status = viClose(DMM2Handle);
      status = viClose(defaultRM);
	  DiscardPanel (panelDigDMM);
	  break;
	}
	return 0;
}

///// Callback to button Auto-Cal DMMs ///////////   
int CVICALLBACK AUTOCAL_DMMs (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:

			break;
	}
	return 0;
}

///// Callback to button Test DMMs /////////// 
int CVICALLBACK TEST_DMMs (int panel, int control, int event,
						   void *callbackData, int eventData1, int eventData2)
{
	int dimBuffer=256;
	ViChar DMM1Desc[VI_FIND_BUFLEN], DMM2Desc[VI_FIND_BUFLEN];
	ViUInt32 ViOpen_Timeout=1000;
	ViUInt32 retCount;
	int itemIndexDmm1, itemIndexDmm2;
	char bufferDMM1[dimBuffer], bufferDMM2[dimBuffer];
	
	switch (event)
	  {
		case EVENT_COMMIT:
		
        GetCtrlIndex(panelDigDMM, PANEL_DMM_DMM1, &itemIndexDmm1);
	    GetLabelFromIndex(panelDigDMM, PANEL_DMM_DMM2,itemIndexDmm1, DMM1Desc);
			
	    GetCtrlIndex(panelDigDMM, PANEL_DMM_DMM2, &itemIndexDmm2);
	    GetLabelFromIndex(panelDigDMM, PANEL_DMM_DMM2,itemIndexDmm2, DMM2Desc);
		
			
	    /*****************************************************************************************/
	    /****************************** OPEN CHANNELS HP3458A ************************************/
	    /*****************************************************************************************/
		
	    /*   1) Open Visa session   */
	    viOpen (defaultRM, DMM1Desc, VI_NULL, 20000, &DMM1Handle);
	    viOpen (defaultRM, DMM2Desc, VI_NULL, 20000, &DMM2Handle); 
			
	    /*   2) Call device clear   */
	    viClear(DMM1Handle);
	    viClear(DMM2Handle);
			 
	    /*  3) Set Visa timeout 1000 ms */
	    viSetAttribute (DMM1Handle, VI_ATTR_TMO_VALUE, ViOpen_Timeout);
	    viSetAttribute (DMM2Handle, VI_ATTR_TMO_VALUE, ViOpen_Timeout);
			
	    viPrintf(DMM1Handle,"RESET\n");
	    viPrintf(DMM2Handle,"RESET\n");
	 		 
	    viWrite(DMM1Handle, "END ALWAYS\n", 10, &retCount);
	    viWrite(DMM2Handle, "END ALWAYS\n", 10, &retCount);
			 
	    viWrite(DMM1Handle, "ID?\n", 3, &retCount);
	    viRead(DMM1Handle, bufferDMM1, 7, &retCount);
			 
	    Fmt (bufferDMM1, "%s \n",bufferDMM1);
	    SetCtrlVal (panelDigDMM, PANEL_DMM_DMMText, bufferDMM1);
	  
	    viWrite(DMM2Handle, "ID?\n", 3, &retCount);
	    viRead(DMM2Handle, bufferDMM2, 7, &retCount);
	    Fmt (bufferDMM2, "%s \n",bufferDMM2);
	    SetCtrlVal (panelDigDMM, PANEL_DMM_DMMText, bufferDMM2);
		
		viPrintf(DMM1Handle,"PRESET NORM;END ALWAYS;TARM AUTO; TRIG AUTO; FUNC ACV\n");
		viPrintf(DMM2Handle,"PRESET NORM; END ALWAYS;TARM AUTO; TRIG AUTO;FUNC ACV\n");
		
		SetCtrlAttribute (panelDigDMM,PANEL_DMM_SamplingDMMs, ATTR_DIMMED, 0);
		
		SetCtrlAttribute (panelDigDMM,PANEL_DMM_Test_DMMs, ATTR_DIMMED, 1);
		SetCtrlAttribute (panelDigDMM,PANEL_DMM_AutoCal_Dmms, ATTR_DIMMED, 1);
	  
		break;
	}
	return 0;
}


///// Callback function for sampling with synchronized DMMs ///////////
int CVICALLBACK Sampling_DMMs (int panel, int control, int event,
							   void *callbackData, int eventData1, int eventData2)
{
	//ViChar DMM1Desc[VI_FIND_BUFLEN], DMM2Desc[VI_FIND_BUFLEN];
	int actualRecordLength=10000;
	char bufferDMM[256];
	ViUInt32 retCount;
	static char stringinput[256];
	static ViUInt32 BytesToWrite;
	char bufferDMM1[actualRecordLength], bufferDMM2[actualRecordLength];
	short *ReadBuffDmm1, *ReadBuffDmm2;
	long *ReadBuffDmm1_DINT, *ReadBuffDmm2_DINT; 
	float iscaleDmm1=0.0, iscaleDmm2=0.0;
	double *scaledDmm1, *scaledDmm2;
	double *diff_ADC1, *diff_ADC2;
	int i=0, DispWavePoints, l=0;
	char str_master[10]="Master";
	char str_slave[10]="Slave";
	unsigned short int AcqCycles=0;
	char buffer_in[512];
	char append[64];
	int nrdgs, SINT_flag;
	int timer=0;
	 
	
	switch (event)
	{
	  case EVENT_COMMIT:
			
	  GetCtrlVal(panelDigDMM, PANEL_DMM_NRDGS, &nrdgs);
	  GetCtrlVal(panelDigDMM, PANEL_DMM_RepCycles, &AcqCycles);
	  GetCtrlVal(panelDigDMM, PANEL_DMM_Samp_Clock, &timer);
	  GetCtrlVal(panelDigDMM, PANEL_DMM_SINT_flag, &SINT_flag);
	  GetCtrlVal(panelDigDMM, PANEL_DMM_RepCycles, &AcqCycles);
	  GetCtrlVal (panelHandle, PANEL_DispWavPoints, &DispWavePoints); 
	  GetCtrlVal(panelHandle, PANEL_SaveMeasData, &fileFlagMeas);
	  if (fileFlagMeas==1)
           {
             FileSelectPopup ("", "*.txt", "",
                                "Write Power Measurements", VAL_OK_BUTTON,
                                0, 0, 1, 0, filePathMeas);
			 
			 fileHandle2 = fopen (filePathMeas, "a");
			 con_init=0;  //Condizione per intestazione file
	       }
	  
	  
	       for(l=0; l<AcqCycles; l++)
			 {
			  
			  /////// Voltage divider and current shunt/////////////////
	          // Values coming from the main pannel
	          GetCtrlVal (panelHandle, PANEL_R_VD, &R_VD); 
	          GetCtrlVal (panelHandle, PANEL_Ph_VD, &Ph_VD);
	          GetCtrlVal (panelHandle, PANEL_R_Shunt, &R_SH); 
	          GetCtrlVal (panelHandle, PANEL_Ph_Shunt, &Ph_SH);
	          /////////////////////////////////////////////////////////	

			  
              ///////////////////////////////////// CfgCHannels HP3458A ///////////////////////////////////////
			  CfgChannelDMM(DMM2Handle, "Slave");
			  CfgChannelDMM(DMM1Handle, "Master");
			 
			  //bufferDMM={};
			  viPrintf(DMM1Handle,"APER?\n");
			  viRead(DMM1Handle, bufferDMM, 20, &retCount);
			  SetCtrlVal (panelDigDMM, PANEL_DMM_Actual_Aper, bufferDMM);
			 
			  switch (SINT_flag)
			   {
			    
				  case 1:
				    // String format for transfering data to PC
			        strcpy(buffer_in, "%");
                    sprintf(append, "%d", nrdgs); 
                    strcat(buffer_in, append);
                    strcat(buffer_in, "hy\n");		// "h" for short SINT format and "l" for long DINT format
				    ReadBuffDmm1 = (short*) malloc (sizeof (short) * (nrdgs));
			        ReadBuffDmm2 = (short*) malloc (sizeof (short) * (nrdgs));
				  break;
				  
				  case 0:
				  if (atof(bufferDMM) < 1.4E-6)  // Case with SINT data format (2-bytes)
				   {
					// String format for transfering data to PC
			        strcpy(buffer_in, "%");
                    sprintf(append, "%d", nrdgs); 
                    strcat(buffer_in, append);
                    strcat(buffer_in, "hy\n");		// "h" for short SINT format and "l" for long DINT format
				    ReadBuffDmm1 = (short*) malloc (sizeof (short) * (nrdgs));
			        ReadBuffDmm2 = (short*) malloc (sizeof (short) * (nrdgs));
				   
				   }
			  	  
				  else  // case with DINT data format (4-bytes)
				  {
					// String format for transfering data to PC
			        strcpy(buffer_in, "%");
                    sprintf(append, "%d", nrdgs); 
                    strcat(buffer_in, append);
                    strcat(buffer_in, "ly\n");		// "h" for short SINT format and "l" for long DINT format
				    ReadBuffDmm1_DINT = (long*) malloc (sizeof (long) * (nrdgs));
			        ReadBuffDmm2_DINT = (long*) malloc (sizeof (long) * (nrdgs));
				  
				  }
				  break;
			    }  // END Switch for SINT / DINT data format 
			 
			 ////// Prepare array to fill with sampled data /////////////////////
			 scaledDmm1 = (double*) malloc (sizeof (double) * (nrdgs));
			 scaledDmm2 = (double*) malloc (sizeof (double) * (nrdgs));
			 diff_ADC1 = (double*) malloc (sizeof (double) * (nrdgs)); 
			 diff_ADC2 = (double*) malloc (sizeof (double) * (nrdgs)); 
             
             //////////////////////////////////// InitSampling3458() /////////////////////////////////////////
			  SamplingDMM(DMM2Handle, "Slave"); 
			  SamplingDMM(DMM1Handle, "Master"); 
			 
			  //Delay(0.1);
			  
             ////////////////////////////////////// GetDataStream3458() //////////////////////////////////////
			 
			 // Transfer data to PC
			
			 if( SINT_flag==0 && atof(bufferDMM) > 1.4E-6 ) // for DINT format
			  {
			 
				//Master
                //viScanf (DMM1Handle, "%10000hy\n", &ReadBuffDmm1);
			    viScanf(DMM1Handle, buffer_in, ReadBuffDmm1_DINT);
			    viPrintf(DMM1Handle,"ISCALE?\n");
			    //viScanf(Inst_dmm_1,"%f\n",dummyRead_1);
		        if(status=viScanf(DMM1Handle,"%f\n",&iscaleDmm1)<0)
				  return status;
	  
			   // Slave
			   //viScanf (DMM2Handle, "%10000hy\n", &ReadBuffDmm2);
			   viScanf(DMM2Handle, buffer_in, ReadBuffDmm2_DINT);
			   viPrintf(DMM2Handle,"ISCALE?\n");
			   //viScanf(Inst_dmm_1,"%f\n",dummyRead_1);
		       if(status=viScanf(DMM2Handle,"%f\n",&iscaleDmm2)<0)
					  return status;	  
			  
			   i=0;
			   for (int i=0;i<nrdgs;i++)
                {
                  scaledDmm1[i]=iscaleDmm1*ReadBuffDmm1_DINT[i];
	              scaledDmm2[i]=iscaleDmm2*ReadBuffDmm2_DINT[i];
				}
			  
			     free(ReadBuffDmm1_DINT);
			     free(ReadBuffDmm2_DINT);	
			  } 
				  
			  
			  else   // for SINT format
			  {
			  
			  //Master
              //viScanf (DMM1Handle, "%10000hy\n", &ReadBuffDmm1);
			  viScanf(DMM1Handle, buffer_in, ReadBuffDmm1);
			  viPrintf(DMM1Handle,"ISCALE?\n");
			  //viScanf(Inst_dmm_1,"%f\n",dummyRead_1);
		      if(status=viScanf(DMM1Handle,"%f\n",&iscaleDmm1)<0)
				  return status;
	  
			 // Slave
			 //viScanf (DMM2Handle, "%10000hy\n", &ReadBuffDmm2);
			 viScanf(DMM2Handle, buffer_in, ReadBuffDmm2);
			 viPrintf(DMM2Handle,"ISCALE?\n");
			 //viScanf(Inst_dmm_1,"%f\n",dummyRead_1);
		     if(status=viScanf(DMM2Handle,"%f\n",&iscaleDmm2)<0)
					  return status;	  
			 
			 i=0;
			  for (int i=0;i<nrdgs;i++)
                {
                 scaledDmm1[i]=iscaleDmm1*ReadBuffDmm1[i];
	             scaledDmm2[i]=iscaleDmm2*ReadBuffDmm2[i];
	            }
			    
			  
			   free(ReadBuffDmm1);
			   free(ReadBuffDmm2);
				
			  }
			 

			  viPrintf(DMM1Handle,"TARM HOLD\n");
			  viPrintf(DMM2Handle,"TARM HOLD\n");
			 
			  
		
		   DeleteGraphPlot (panelHandle, PANEL_acquisitionDiff, -1, VAL_DELAYED_DRAW);
		   DeleteGraphPlot (panelHandle, PANEL_acquisitionGraph, -1, VAL_DELAYED_DRAW);
			  
			  /******* CloseChannel3458A *************************/
			  
		
			PlotWaveform (panelHandle, PANEL_acquisitionGraph, scaledDmm1,
                  nrdgs, VAL_DOUBLE, 1.0, 0.0, 0.0, 1.0, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_RED); 
			
			PlotWaveform (panelHandle, PANEL_acquisitionGraph, scaledDmm2,
                  nrdgs, VAL_DOUBLE, 1.0, 0.0, 0.0, 1.0, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_BLUE);
			  
			  
			PlotWaveform (panelHandle, PANEL_acquisitionDiff, scaledDmm1,
						  DispWavePoints, VAL_DOUBLE, 1.0, 0.0, 0.0, 1.0, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_RED);
						
			PlotWaveform (panelHandle, PANEL_acquisitionDiff, scaledDmm2,
						  DispWavePoints, VAL_DOUBLE, 1.0, 0.0, 0.0, 1.0,
						  VAL_THIN_LINE, VAL_NO_POINT, VAL_SOLID, 1, VAL_BLUE);
			
			SetCtrlVal (panelHandle, PANEL_RecordsAcquired, l+1); 
		
			if (flag_DataProcess==1)
		    {
			  
				Copy1D (scaledDmm1,nrdgs, diff_ADC1);
				Copy1D (scaledDmm2,nrdgs, diff_ADC2);
				
				DataElaboration(diff_ADC1, diff_ADC2, (ViReal64)(1.0*timer), (ViInt32)nrdgs);
		    }
			
			
			free(scaledDmm1);
			free(scaledDmm2);
			
			free(diff_ADC1); 
			free(diff_ADC2); 
			
			ProcessSystemEvents();
			
			} // and for loop REPETITIVE ACQUISITIONS 
			  
			count=0;
			viPrintf(DMM1Handle,"PRESET NORM;END ALWAYS;TARM AUTO; TRIG AUTO; FUNC ACV\n");
			viPrintf(DMM2Handle,"PRESET NORM; END ALWAYS;TARM AUTO; TRIG AUTO;FUNC ACV\n");
			
			if(fileFlagMeas==1)
		     {
		       fclose(fileHandle2);
		 	 }
			 
			break;
	}
	return 0;
}


//int CfgChannelDMM(ViSession Inst_Handle, int Mas_Slav, char Dig_mode, 
//char ExtOut, char Tarm, char Trg_mode, int rgd_cnt, int rdg_mode, int streaming)
int CfgChannelDMM(ViSession Inst_Handle, char *Master_Slave)
{
   ViStatus status;
   int timer=0;
   double aper_time=0.0;
   int DMM_mode=0; // 0-->DCV; 1-->DSDC; 2-->DSAC
   double DMM1_range=0.0;
   double DMM2_range=0.0;
   int DMM_Synch_mode=0;
   int i=0;
   char buffer_in[512];
   char append[64];
   
   //GetCtrlVal(panelDigDMM, PANEL_DMM_NRDGS, &nrdgs); 
   GetCtrlVal(panelDigDMM, PANEL_DMM_Samp_Clock, &timer); 
   GetCtrlVal(panelDigDMM, PANEL_DMM_Aper_Time, &aper_time);
   GetCtrlVal(panelDigDMM, PANEL_DMM_DMM2_range, &DMM2_range);
   GetCtrlVal(panelDigDMM, PANEL_DMM_DMMDigMode, &DMM_mode);
  // GetCtrlAttribute (panelHandle, PANEL_DMM_DMMDigMode, ATTR_CTRL_VAL, DMM_mode);
   GetCtrlVal(panelDigDMM, PANEL_DMM_Synch_mode, &DMM_Synch_mode);
   
   if (!strcmp (Master_Slave, "Master")) // Master case
   {
			
	  //Master
      //  viPrintf(DMM1Handle,"PRESET DIG; FUNC DCV,10; TARM HOLD; TIMER 10E-6; APER 1.4E-6; MEM FIFO; MFORMAT SINT; OFORMAT SINT; TRIG LEVEL; SLOPE POS; LEVEL 0, DC; DELAY -1; EXTOUT APER,NEG; TBUFF ON\n");  
			
	  // 1) Write: "PRESET DIG; FUNC func_name,range". func_name = {DCV or DSDC or DSAC}, range = range voltage 
      GetCtrlVal(panelDigDMM, PANEL_DMM_DMM1_range, &DMM1_range);
	  status = viPrintf(Inst_Handle,"PRESET DIG;FUNC DCV,%f\n", DMM1_range);
			
      // 2) Write:"TARM HOLD;TRIG trg_mode;NRDGS rdg_cnt,rdg_mode;". For parameter values see table below. rdg_cnt = samples count to make.
      status = viPrintf(Inst_Handle,"TARM HOLD\n");
			
			// 3)	In TIMER clocked mode write: “TIMER tim_period”. tim_period = timer period in seconds.
           // The TIMER command defines the time interval for the TIMER sample event in the
           // NRDGS command. When using the TIMER event, the time interval is inserted
           // between readings.
           status = viPrintf(Inst_Handle,"TIMER %g; APER %g\n", (1.0/timer), aper_time);
		   
		   status = viPrintf(DMM1Handle,"MEM FIFO; MFORMAT SINT; OFORMAT SINT; TRIG LEVEL; SLOPE POS; LEVEL 0, DC; DELAY -1; EXTOUT APER,NEG; TBUFF ON\n"); 	
			
	  }		
		
	else if (!strcmp (Master_Slave, "Slave"))// Slave case
       {
		   // Working Sequence Slave
           //viPrintf(DMM2Handle,"PRESET DIG; FUNC DCV,10; TARM HOLD; TIMER 10E-6; APER 1.4E-6; MEM FIFO; MFORMAT SINT; OFORMAT SINT; TRIG EXT; DELAY -1;  EXTOUT APER,NEG; TBUFF ON\n"); 
		   // 1) Write: "PRESET DIG; FUNC func_name,range". func_name = {DCV or DSDC or DSAC}, range = range voltage 
           GetCtrlVal(panelDigDMM, PANEL_DMM_DMM2_range, &DMM2_range);
		   status = viPrintf(Inst_Handle,"PRESET DIG;FUNC DCV,%f\n", DMM2_range);
			
		   // 2) Write:"TARM HOLD;TRIG trg_mode;NRDGS rdg_cnt,rdg_mode;". For parameter values see table below. rdg_cnt = samples count to make.
           status = viPrintf(Inst_Handle,"TARM HOLD\n");
		   
		   // 3)	In TIMER clocked mode write: “TIMER tim_period”. tim_period = timer period in seconds.
           // The TIMER command defines the time interval for the TIMER sample event in the
           // NRDGS command. When using the TIMER event, the time interval is inserted
           // between readings.
           status = viPrintf(Inst_Handle,"TIMER %g; APER %g\n", (1.0/timer), aper_time);
           status= viPrintf(DMM2Handle,"MEM FIFO; MFORMAT SINT; OFORMAT SINT; TRIG EXT; DELAY -1; EXTOUT APER,NEG; TBUFF ON\n");
	    }  

  return 0;
}


///// Sampling routine with synchronized DMMs /////////// 
int SamplingDMM(ViSession Inst_Handle, char *Master_Slave)
{

	ViStatus status;
	int nrdgs=0;
	
	GetCtrlVal(panelDigDMM, PANEL_DMM_NRDGS, &nrdgs);
	
	if (!strcmp (Master_Slave, "Master")) // Master case
     {
	  viPrintf(DMM1Handle,"NRDGS %d, TIMER; TARM SGL\n", nrdgs); // in non-streaming mode use "SGL", in streaming use "SYN".
	 }
	 

	else if (!strcmp (Master_Slave, "Slave"))// Slave case
     {
	  viPrintf(DMM2Handle,"NRDGS %d, EXT; TARM SGL\n", nrdgs); 
	 }
   return 0;  
   
}

int CVICALLBACK CLOSE_DP (int panel, int control, int event,
						  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			
			flag_DataProcess=0;
            	   \
			SetCtrlVal (panelHandle, PANEL_flag_DataProcess, flag_DataProcess);  
			
			DiscardPanel (panelDataProcess);

			break;
	}
	return 0;
}



int CVICALLBACK OK_DMMs (int panel, int control, int event,
						 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:

			break;
	}
	return 0;
}







////////////////// Future virtual Digitizers ////////////////////////////// 

void DMMs_GenericConfiguredAcquisition(void)
{

}


/////////////////////////////////////////////////// End functions for handling of DMMs HP3458A ///////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////////////////////////
//////////// Function prototype for CVI-Matlab Control //////////////////////////////////////////
/////////// Developed by CMI: for further information contact "smaslan@cmi.cz" ////////////////// 
/////////////////////////////////////////////////////////////////////////////////////////////////

int CVICALLBACK Launch_Matlab (int panel, int control, int event,
							   void *callbackData, int eventData1, int eventData2)
{

    // ###note: removed original code and instead starting the new Matlab Module
	
	switch (event)
	{
		case EVENT_COMMIT:
	
			// --- create and show the panel
			int procHandle;
			if((procHandle = LoadPanel (0,"processing_panel.uir",PROCPANEL)) < 0) /* ###note this may be changed based on location of the files in your project*/
				return 1;
	
			// --- set measurement session path to the processing panel
			// get folder path from panel
			char ssn_path[MAX_PATH];
			GetCtrlVal(panelHandle, PANEL_pathMeasData, (void*)ssn_path);
			// merge with session.info file name
			merge_path(ssn_path, ssn_path, TWMSSNINFO);
			// set it to the processing panel
			SetCtrlVal(procHandle, PROCPANEL_TXT_SESSION, (void*)ssn_path);
	
			DisplayPanel(procHandle);
			//RunUserInterface(); // I think this is not needed? It will block GUI which is not needed here
	
			break;
	}
	  
	return 0;
		   
}



/*************************************** End Of Matlab control ******************************/

int CVICALLBACK FolderQWTB (int panel, int control, int event,
							void *callbackData, int eventData1, int eventData2)
{
    char path[MAX_PATH]; 
	char algfld[MAX_PATH];
	
	switch (event)
	{
		case EVENT_COMMIT:
			
			DirSelectPopup (algfld, "Select Directory", 1, 1, path);
			
			// write file path to the box
		
			// ###note: removed, I don't what that supposed to do, but it is a link to old Matlab Module which is no longer valid?
			//SetCtrlVal(panelHandle_MC,PANEL_MC_StrFolderQWTB,path);
			
			
			//FileSelectPopup()
			
			//SetDir("c:\\TWM_old\\CVI\\Pol_Watt_LF_HF_v2_Matlab\\octprog\\qwtb\\alg_TWM-PSFE");


			break;
	}
	return 0;
}

int CVICALLBACK Qwtb_exec (int panel, int control, int event,
						   void *callbackData, int eventData1, int eventData2)
{
	
	//char path[MAX_PATH]="C:\\TWM_old\\CVI\\Pol_Watt_LF_HF_v2_Matlab_Stud\\Matlab Module\\cvidata\\session";
	
	char path_dir[MAX_PATH];
	char path_alg[MAX_PATH];
	char path_raw[MAX_PATH];
	char fileName[MAX_PATH];
	int group_id = 0; 
	int is_last_avg = 1;
	int avg_id = 1;
	int n = 0, l = 0;
	char calc_unc[5]=""; 
    char *errstr;
	
	switch (event)
	{
		case EVENT_COMMIT:
			
		break;
	}
	return 0;
}


////////////////////////////////////// END CVI-MATALB LINK//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





int CVICALLBACK ShowGraphCellTable (int panel, int control, int event,
									void *callbackData, int eventData1, int eventData2)
{
	//Point NewCell;
	double wfmData[100];
    int wfmHandle;
	int graphCtrl;
	int panelGraphV;
	int parentpanelDataProcess=0;
	//static int panelHandleGrV;
	
	//int NewCellV;
	
	switch (event)
	{
		case EVENT_COMMIT:
			
		
		//SetTableCellVal(panelDataProcess, PANEL_TABLE_data, MakePoint(1,1), V_rms);  // MakePonit(column index, row index) 
		
		GetActiveTableCell (panelDataProcess, PANEL_DP_TABLE_data, &NewCell);
		
		
		switch(NewCell.y)
          {
            case 1:
            
			  SetTableCellAttribute (panelDataProcess,PANEL_DP_TABLE_data, NewCell, ATTR_CTRL_VAL,"Discard Graph");
			  if ((panelHandleGrV = LoadPanel (0, "TPQA.uir", PANEL_GrV)) < 0)
              return -1;
			  DisplayPanel (panelHandleGrV);
              NewCellV=NewCell.y;
			break;
   
		     case 2:
             //*verticalCoupling = NISCOPE_VAL_DC;
             break;
           
			 case 4:
              SetTableCellAttribute (panelDataProcess,PANEL_DP_TABLE_data, NewCell, ATTR_CTRL_VAL,"Discard Graph");
			  if ((panelHandleGrI = LoadPanel (0, "TPQA.uir", PANEL_GrI)) < 0)
              return -1;
			  DisplayPanel (panelHandleGrI);
			  NewCellI=NewCell.y;
             break;
          }
		
			break;
	}
	return 0;
}


//==============================================================================
// UI callback function prototypes

/// HIFN Exit when the user dismisses the panel.
int CVICALLBACK panel_GrV (int panel, int event, void *callbackData,int eventData1, int eventData2)
{
	if (event == EVENT_CLOSE)
	{
		//QuitUserInterface (0);
		
		NewCell.y=NewCellV;
		
		SetTableCellAttribute (panelDataProcess,PANEL_DP_TABLE_data, NewCell, ATTR_CTRL_VAL,"Show Graph");
		DiscardPanel (panelHandleGrV);
		
		NewCellV=0;
		
	
		
	}
	
	return 0;
}

int CVICALLBACK panel_GrI (int panel, int event, void *callbackData,int eventData1, int eventData2)
{
	if (event == EVENT_CLOSE)
	{
		//QuitUserInterface (0);
		
		NewCell.y=NewCellI;
		SetTableCellAttribute (panelDataProcess,PANEL_DP_TABLE_data, NewCell, ATTR_CTRL_VAL,"Show Graph");
		DiscardPanel (panelHandleGrI);
		NewCellI=0;
		
	
	}
	
	return 0;
}




int CVICALLBACK About (int panel, int control, int event,
					   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
	
	    if ((panelAbout = LoadPanel (0, "TPQA.uir", PANEL_Abou)) < 0)
        return -1;
        
		DisplayPanel (panelAbout);
		
		break;
	}
	return 0;
}



int CVICALLBACK CloseAbout (int panel, int event, void *callbackData,
							int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
			
			DiscardPanel (panelAbout);

			break;
	}
	return 0;
}

int CVICALLBACK FolderMeasData (int panel, int control, int event,
								void *callbackData, int eventData1, int eventData2)
{
	char path[MAX_PATH];
	
	switch (event)
	 {
		 case EVENT_COMMIT:
		 case EVENT_LEFT_CLICK:
	
	        if(DirSelectPopup(pathMeasData,"Select folder for measurement data",1,1,path) == VAL_NO_DIRECTORY_SELECTED)
				break;
				
		     // remember last result folder
		     strcpy(pathMeasData,path);
				
		     // write file path to the box
		     SetCtrlVal(PANEL,PANEL_pathMeasData,path);
		  
	 }
	return 0;
}




// ###note: common for all tranducers
// --- loader of tranducer
int CVICALLBACK LoadTranData (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			// --- path select ---
			
			char path[MAX_PATH];
			if(!FileSelectPopup(corr_fld, "*.info", "*.info","Select transducer correction file",VAL_LOAD_BUTTON, 0, 0, 1, 1, path))
				break;
			
			// identify transducer to which this event belongs
			int trid = -1;
			for(int k = 0;k < TWMMAXTR;k++)
				if(trlist_btnp[k] == control)
				{
					trid = k;
					break;
				}
			if(trid<0)
				break;
			
			// set path to textbox
			
			ResetTextBox(panel, trlist_path[trid], (void*)path);
			//SetCtrlVal(panel, trlist_path[trid], (void*)path);
			
			FILE *fptr;
			if((fptr=fopen(path, "r"))==NULL)
			{
				printf("Warning: Problem opening file or file not found\n");
	    		return -1;
			}
		 	  
			char linebuffer[INFOMAXSTR];
        
		 
		 while(fgets(linebuffer, INFOMAXSTR, fptr)!=0)
		 {
			  int idx = FindPattern (linebuffer, 0, -1, "::", 0, 0); 
			  //strLength = strlen (linebuffer);
		  
			  char *TagString= (char*) malloc (sizeof (char) * (idx+1)+1);
			  char *ValString= (char*) malloc (sizeof (char) * (strlen(linebuffer)-(idx+2))+1);
		  
			  if (idx>0)
			  {
				  //n = Scan(linebuffer, "%s>%s[t59]", x);
				  CopyString (TagString, 0, linebuffer, 0, idx);
		  	  
			      if (!strcmp (TagString, "type")){  // type of trasducers n.b from *.info file type:: shunt or RVD
				  //some code here;
                  }
                  else if (!strcmp (TagString, "name")) {
                  //some code here;
				   CopyBytes (ValString, 0, linebuffer, idx+3, strlen(linebuffer)-(idx+2));
				   SetCtrlVal (panel, trlist_name[trid], ValString);
				  }
			
				  else if (!strcmp (TagString, "serial number")) {
                  //some code here;
                   CopyBytes (ValString, 0, linebuffer, idx+3, strlen(linebuffer)-(idx+2));
				   SetCtrlVal (panel, trlist_trsn[trid], ValString);
				  }
			
			 	  else if (!strcmp (TagString, "nominal ratio")) {
                  //some code here;
                  CopyBytes (ValString, 0, linebuffer, idx+3, strlen(linebuffer)-(idx+2));
				  SetCtrlVal (panel, trlist_nomg[trid], ValString);
				  
				  
				  }
		  
				 else if (!strcmp (TagString, "nominal phase")) {
                 // some code here;
				 CopyBytes (ValString, 0, linebuffer, idx+3, strlen(linebuffer)-(idx+2));
				 SetCtrlVal (panel, trlist_trph[trid], ValString);
				 
                 }
		  
				 // else if (!strcmp (myString, "nominal ratio")) {
                 // some code here;
                 // }
		  
			  
			  }
			   
         
			 free (TagString);
			 free (ValString);
		   }
		
		  fclose(fptr); // close the file and save it
		  
		  	// store last folder path
			char *pstr;
			pstr = strrchr(path,'\\');
			if(pstr)
				*pstr = '\0';
			strcpy(corr_fld,path);
			
			break;
	}
	return 0;
}

// --- select digitizer correction file
int CVICALLBACK btn_load_dig (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			
			char path[MAX_PATH];
			if(!FileSelectPopup(corr_fld, "*.info", "*.info","Select digitizer correction file",VAL_LOAD_BUTTON, 0, 0, 1, 1, path))
				break;
			
			// set path to textbox
			SetCtrlVal(panel, Digitizer_TEXT_DIG_PATH, (void*)path);
			
			// store last folder path
			char *pstr;
			pstr = strrchr(path,'\\');
			if(pstr)
				*pstr = '\0';
			strcpy(corr_fld,path);
			
			
			break;
	}
	return 0;
}



////////////// Callback to change the precision of Y-axis Plotchart in CVI processing block ///////////////////

int CVICALLBACK YStripChartPrecision (int panel, int control, int event,
									  void *callbackData, int eventData1, int eventData2)
{   unsigned short int Y_Precision;
	switch (event)
	{
		case EVENT_COMMIT:
		
			GetCtrlVal(panelHandleGrV, PANEL_GrV_Y_Precision, &Y_Precision);
			SetCtrlAttribute (panelHandleGrV, PANEL_GrV_STRIPCHART_V, ATTR_YPRECISION, Y_Precision); 
			break;
	}
	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////// Callback to change the number of points per screen of X-axis Plotchart in CVI processing block ///////////////////  
int CVICALLBACK PointsStripChartScreen (int panel, int control, int event,
										void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{   unsigned short int PScreen;
		case EVENT_COMMIT:
			
			GetCtrlVal(panelHandleGrV, PANEL_GrV_PScreen, &PScreen);
			SetCtrlAttribute (panelHandleGrV, PANEL_GrV_STRIPCHART_V, ATTR_POINTS_PER_SCREEN, PScreen);

			break;
	}
	return 0;
}

int CVICALLBACK ModularSETUP (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	int modularsetup_flag;
	
	switch (event)
	{
		case EVENT_COMMIT:
			
			
			break;
	}
	return 0;
}

 
///////////// Callback for statstic of data when CVI algorithms are used //////////////////////////
int CVICALLBACK CheckStatistic (int panel, int control, int event,
								void *callbackData, int eventData1, int eventData2)
{
	int newCheckStatistic=0, Check_Statistic=0;
	
	switch (event)
	{
		case EVENT_COMMIT:
		
		GetCtrlVal (panelDataProcess, PANEL_DP_Statistic, &Check_Statistic);
		
		switch (Check_Statistic)
		 {
		  case 0:
			newCheckStatistic=1;
			//SetCtrlVal (panelDataProcess, PANEL_DP_Statistic, newCheckStatistic);
		    break;
	      
          case 1:
	        newCheckStatistic=0;
			//SetCtrlVal (panelDataProcess, PANEL_DP_Statistic, newCheckStatistic);
		    break;  
	     }
	 
		 break;
    }
	return 0;
}


 
int CVICALLBACK PutCorrections (int panel, int control, int event,
								void *callbackData, int eventData1, int eventData2)
{
  	
	char Ratio_RVD_1[20], Phase_RVD_2[20],  Ratio_CSH_1[20],  Phase_CSH[20]; 
	
	switch (event)
	{
		case EVENT_COMMIT:
			
			// get panels with controls
		    int trpan,digpan;
	  	    GetPanelHandleFromTabPage (panelHWCorrections,  PANEL_HW_TAB_Corrections, 0, &trpan);
			GetCtrlVal(trpan, Transducer_Ratio_RVD_1, Ratio_RVD_1);
			SetCtrlVal(panelHandle, PANEL_R_VD, atof(Ratio_RVD_1));
			
			//GetPanelHandleFromTabPage (panelHWCorrections,  PANEL_HW_TAB_Corrections, 0, &trpan);
			GetCtrlVal(trpan, Transducer_Phase_RVD_2, Phase_RVD_2);
			SetCtrlVal(panelHandle, PANEL_Ph_VD, atof(Phase_RVD_2));
			
			//GetPanelHandleFromTabPage (panelHWCorrections,  PANEL_HW_TAB_Corrections, 0, &trpan);
			GetCtrlVal(trpan, Transducer_Ratio_CSH_1, Ratio_CSH_1);
			SetCtrlVal(panelHandle, PANEL_R_Shunt, atof(Ratio_CSH_1));
			
			//GetPanelHandleFromTabPage (panelHWCorrections,  PANEL_HW_TAB_Corrections, 0, &trpan);
			GetCtrlVal(trpan, Transducer_Phase_CSH , Phase_CSH);
			SetCtrlVal(panelHandle, PANEL_Ph_Shunt, atof(Phase_CSH));
			
		
			break;
	}
	return 0;
}
