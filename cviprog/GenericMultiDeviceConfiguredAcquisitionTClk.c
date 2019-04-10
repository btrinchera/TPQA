#include <ansi_c.h>
#include <formatio.h>
#include <userint.h>
#include "TPQA.h"
#include "niTClk.h"

#include "twm_matlab.h"
#include "tpqa_globals.h"

/********************************************************************************
/  Configured Acquisition generic file, used by examples in CVI, C, and C++
/*******************************************************************************/

#include "GenericMultiDeviceConfiguredAcquisitionTClk.h"

ViStatus error = VI_SUCCESS;
ViChar errorMessage[MAX_ERROR_DESCRIPTION];
ViChar errorSource[MAX_FUNCTION_NAME_SIZE];

ViUInt32 numberOfSessions=0;

ViSession sessions[MAX_NUM_DEVICES] = { VI_NULL }; // Session for each device 

//int check=0; // Controlla se inizializzare una nuova sessione 

ViReal64 **SaveWavPtr; // è in forma matriciale
ViInt32 Size, Sessions;

ViInt32 fileFlag;

ViInt32 stat;
char filePath[500]; 

FILE* fileHandle;

static int panelHandle; 

int Init=0; // variabile di controllo per la funzione _initSession (......)
int LengthPathName;

// Helper function that
// - gets comma-separated list of resource names from GUI,
// - parses the list
// - initializes resources and
// - reports initialized sessions and the number of sessions.
ViStatus _initSessions(ViUInt32* numberOfSessions,
                       ViSession sessions[])
{
   resourceNameType commaSeparatedResourceNames;
   ViChar * currentResource = commaSeparatedResourceNames;
   ViChar * currentResourceTrimmed = currentResource;
   unsigned int i = 0;
   unsigned int j = 0;
   unsigned int numWhitespace=0;
   // Obtain the comma-separated resource names from the user interface
   GetResourceNamesFromGUI(commaSeparatedResourceNames);

   // Set up strtok to iterate on the comma separated list
   // NOTE: if you care about thread-safety, try strsep instead of strtok
   currentResource = strtok(commaSeparatedResourceNames,",");

   // Now parse resource names into an array and establish the number of resources
   while(currentResource != NULL && i < MAX_NUM_DEVICES)
   {
      j=0;
      numWhitespace=0;
      // Removes whitespace from the identifier
      while(currentResource[j])
      {
         if(currentResource[j]!=' ')
            currentResourceTrimmed[j-numWhitespace]=currentResource[j];
         else numWhitespace++;
         j++;
      }
      currentResourceTrimmed[j-numWhitespace]= '\0';
      // Initialize the digitizer
      handleNIScopeErr(niScope_init (currentResourceTrimmed, NISCOPE_VAL_FALSE, NISCOPE_VAL_TRUE, &sessions[i]));
      // communicate where the next token starts to the next iteration
      currentResource = strtok(NULL,",");
      // By the loop invariant, we have an additional session
      i++;
   }


 Error:
   // even if an error occurs, return the number of opened sessions so that
   // they can be closed in the caller
   DisplayErrorMessageInGUI (error, errorMessage);
   (*numberOfSessions) = i;
   return error;
}

ViStatus _fetchAndPlotData(ViSession vi, ViChar* channelName, ViReal64 timeout)
{
   // Waveforms
   struct niScope_wfmInfo *wfmInfoPtr = NULL;
   ViReal64 *waveformPtr = NULL;
   ViInt32 actualRecordLength;
   ViReal64 actualSampleRate;
   ViInt32 numWaveform;
   // ViReal64 **SaveWavPtr;
   int i=0;
   int m=0;
   
   // Find out the current record length and number of waveforms
   checkErr (niScope_ActualNumWfms (vi, channelName, &numWaveform));

   checkErr (niScope_ActualRecordLength (vi, &actualRecordLength));

   // Allocate space for the waveform and waveform info according to the
   // record length and number of waveforms
   wfmInfoPtr = malloc (sizeof (struct niScope_wfmInfo) * numWaveform);
  
   waveformPtr = (ViReal64*)malloc (sizeof (ViReal64) * actualRecordLength * numWaveform);
   
   // If it doesn't have enough memory, give an error message
   if (waveformPtr == NULL || wfmInfoPtr == NULL)
      checkErr (NISCOPE_ERROR_INSUFFICIENT_MEMORY);

   // Get the actual sample rate
   checkErr (niScope_SampleRate (vi, &actualSampleRate));

   // Fetch the data
   checkErr (niScope_Fetch (vi, channelName, timeout, actualRecordLength,
                            waveformPtr, wfmInfoPtr));

   // Group the data in the matrix form
   switch (vi)
       {
		  case 1: //First PCI5922 module
           
		     SaveWavPtr = (double**) malloc (numberOfSessions* numWaveform*sizeof (double*));
		     Size=numberOfSessions* numWaveform; // è il numero delle colonne del vettore SaveWavPtr
         
		     // Spazio dinamico MxN in memoria viene creato all'inizio 
		  
             for (m=0; m<(numberOfSessions * numWaveform); m++)
               {
	             SaveWavPtr[m] = (ViReal64*)malloc (sizeof (ViReal64) * actualRecordLength);  
               } 	 
			 
			 for (i = 0; i<actualRecordLength; i++)
		       {
			     SaveWavPtr[0][i] = waveformPtr[i];
	           }
		  
		     if (numWaveform==2)
		      {
			    waveformPtr = waveformPtr + wfmInfoPtr[0].actualSamples;
		        for (i = 0; i<actualRecordLength; i++)
		          {
				    SaveWavPtr[1][i]=waveformPtr[i]; 
			      }
		        waveformPtr = waveformPtr - wfmInfoPtr[0].actualSamples;
		      } 
          
		     Sessions=vi;
		  break;
			
		 
		  case 2: //Second PCI5922 module
            for (i = 0; i<actualRecordLength; i++)
		      {
				  
			    SaveWavPtr[2][i]=waveformPtr[i];
	          }
		
		     if (numWaveform==2)
		      {
		        waveformPtr = waveformPtr + wfmInfoPtr[0].actualSamples;
			    for (i = 0; i<actualRecordLength; i++)
		          {
				    SaveWavPtr[3][i]=waveformPtr[i]; 
			      }
		        waveformPtr = waveformPtr - wfmInfoPtr[0].actualSamples;
		      }	 
           
		    Sessions=vi;
		  break; // 
		
		   
		  case 3: //Third PCI5922 module
            
		  for (i = 0; i<actualRecordLength; i++)
		  {
			SaveWavPtr[4][i]=waveformPtr[i]; 
	      }
		
		  if (numWaveform==2)
		   {
			 waveformPtr = waveformPtr + wfmInfoPtr[0].actualSamples;
		     for (i = 0; i<actualRecordLength; i++)
		       {
		         SaveWavPtr[5][i]=waveformPtr[i]; 
	           }
		     waveformPtr = waveformPtr - wfmInfoPtr[0].actualSamples;
		   } 
          Sessions=vi;
		  break;
	  
		}
	
	
	if (fileFlag==1)
    {
	// Salvo i dati su file
	
	  if(vi==numberOfSessions)   // quando Vi diventa uguale al numero delle sessioni
	  {
	       
		  switch (vi)
			{
			 case 1:  // Caso con una solo board PXI
			        
			 
				 fprintf(fileHandle,"Sample Rate: %f\t Record Length: %i\t \n", actualSampleRate, actualRecordLength);
				 fprintf(fileHandle,"N°P\t  CH1\t  CH2\t  \n\n");
				 for (i = 0; i<actualRecordLength; i++)
		               {
		            	 fprintf(fileHandle,"%i\t %0.12f\t %0.12f\t  \n ",i, SaveWavPtr[0][i], SaveWavPtr[1][i] );
	                   }
	     
			break;
			
			case 2:  // Caso con due schede PXI 5922 
			        
				fprintf(fileHandle,"Sample Rate: %f\t Record Length: %i\t \n", actualSampleRate, actualRecordLength);
				fprintf(fileHandle,"N°P\t  CH1\t  CH2\t CH3\t CH4\t  \n\n");
				for (i = 0; i<actualRecordLength; i++)
		               {
		            	 fprintf(fileHandle,"%i\t %0.12f\t %0.12f\t %0.12f\t %0.12f\t \n ",i, SaveWavPtr[0][i], SaveWavPtr[1][i], SaveWavPtr[2][i], SaveWavPtr[3][i] );
	                   }
	     
			break;
			
			case 3:  // Caso con tre board PXI
			        
				fprintf(fileHandle,"Sample Rate: %f\t Record Length: %i\t \n", actualSampleRate, actualRecordLength);
				fprintf(fileHandle,"N°P\t  CH1\t  CH2\t CH3\t CH4\t CH5\t CH6\t  \n\n");
				for (i = 0; i<actualRecordLength; i++)
		               {
		            	 fprintf(fileHandle,"%i\t %0.12f\t %0.12f\t %0.12f\t %0.12f\t %0.12f\t %0.12f\t \n ",
							 i,SaveWavPtr[0][i], SaveWavPtr[1][i], SaveWavPtr[2][i], SaveWavPtr[3][i], SaveWavPtr[4][i], SaveWavPtr[5][i]);
	                   }
	     
		    break;
			
			} // End Switch	
	
         } //end if (vi==numberOfSessions)
   
		// fclose(fileHandle);
   } // end if flagFile
	
   // Plot the waveform
   PlotWfms (numWaveform, waveformPtr, wfmInfoPtr, actualSampleRate, actualRecordLength, SaveWavPtr, Size, Sessions);

//	  PlotXY (panelHandle, PANEL_acquisitionGraph_XY, SaveWavPtr[0], SaveWavPtr[1],1000,
//	   VAL_DOUBLE, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_RED);


Error:
   // Error display is handled by caller
   if (wfmInfoPtr)
      free (wfmInfoPtr);

   if (waveformPtr)
      free (waveformPtr);
  
    if (vi==numberOfSessions)
	{	
		
	if (fileFlag==1)
		{
		  fclose(fileHandle);
		}	 
		  for (m=0; m< (numberOfSessions * numWaveform); m++)
			  {
			    free (SaveWavPtr[m]);
               // free (SaveWavPtr);
			  }
	
	  //  }
	}
   
	return error;
}


//////////////////////////////////////////////////////////////////////////
// niScope_GenericMultiDeviceConfiguredAcquisitionTClk
// ///////////////////////////////////////////////////////////////////////
// ###note: added meas_folder option to pass in measurement folder from main panel
ViStatus _VI_FUNC niScope_GenericMultiDeviceConfiguredAcquisitionTClk(char *meas_folder)
{
   ViSession vi;
   ViUInt32 i = 0;
   ViUInt32 s=0;
   //char *Str;
   //char ext_file[5]=".txt";
   

   // Variables used to get values from the GUI
   ViChar channelName[MAX_STRING_SIZE];
   ViChar triggerSource[MAX_STRING_SIZE];

   ViInt32 acquisitionType;
   ViReal64 verticalRange;
   ViReal64 verticalOffset;
   ViInt32 verticalCoupling;
   ViReal64 probeAttenuation;
   ViReal64 inputImpedance;
   ViReal64 maxInputFrequency, previousMaxInputFrequency = -10;
   ViReal64 minSampleRate, previousMinSampleRate = -10;
   ViInt32 minRecordLength, previousMinRecordLength = -10;
   ViReal64 timeout;
   ViInt32 triggerType, previousTriggerType = -10;
   ViInt32 triggerCoupling;
   ViInt32 triggerSlope;
   ViReal64 triggerLevel;
   ViReal64 triggerHoldoff;
   ViReal64 triggerDelay;
   ViInt32 windowMode;
   ViReal64 lowWindowLevel;
   ViReal64 highWindowLevel;
   ViReal64 hysteresis;
   ViReal64 refPosition;
   ViInt32 numWaveform;
   ViInt32 SaveData;

 //  ViUInt32 numberOfSessions=0;
 
 //  ViSession sessions[MAX_NUM_DEVICES] = { VI_NULL }; // Session for each device
	
   // Default values used in this example
   ViInt32  numRecords;
   ViInt32 stop = NISCOPE_VAL_FALSE;
   
   int SS_Controll=0;
   
   // Clear our error messages
   error = VI_SUCCESS;
   strcpy(errorMessage,"");
   strcpy(errorSource,"");
  
   /* Opening file when data-logging is enabled */
   ////////////////////////////////////////////////////////////////////////////////////////////
   
    GetParametersFromGUI (channelName, &acquisitionType, &verticalRange, &verticalOffset,
                            &verticalCoupling, &probeAttenuation, &inputImpedance, &maxInputFrequency,
                            &minSampleRate, &minRecordLength, &timeout, &numRecords,
                            &refPosition, &triggerType, triggerSource, &triggerCoupling, &triggerSlope,
                            &triggerLevel, &triggerHoldoff, &triggerDelay, &windowMode, &lowWindowLevel,
                            &highWindowLevel, &hysteresis, &fileFlag);
   
	// ###note: clear status by default!
	stat = 0;	
	
    char filePathDef[TWMMAXSTR];
	if (fileFlag==1)
    {
        stat = FileSelectPopup (meas_folder, "*.txt", "",
                                "Write ADC data to file", VAL_OK_BUTTON,
                                0, 0, 1, 0, filePathDef);
	}
  
   ////////////////////////////////////////////////////////////////////////////////////////  
   
  if(Init==0)
   {
     checkErr(_initSessions(&numberOfSessions,sessions));
   }
   
   // Loop until the stop flag is set
   while (!stop)
   {
      // Obtain the necessary parameters from the user interface
      GetParametersFromGUI (channelName, &acquisitionType, &verticalRange, &verticalOffset,
                            &verticalCoupling, &probeAttenuation, &inputImpedance, &maxInputFrequency,
                            &minSampleRate, &minRecordLength, &timeout, &numRecords,
                            &refPosition, &triggerType, triggerSource, &triggerCoupling, &triggerSlope,
                            &triggerLevel, &triggerHoldoff, &triggerDelay, &windowMode, &lowWindowLevel,
                            &highWindowLevel, &hysteresis, &fileFlag);

	  
	 /////// per i file in progressione ///////////////////////////////////////////////////7 
	 
	   if (fileFlag==1 && stat > 0)
         {
         
			//###note: removed old naming style
			
			 /*if(s==0)
			{
			  LengthPathName=StringLength(filePath)-4; //E la lunghezza del file meno ".txt"  
			}
			
			if(s>0)
			{	
				Str= (char*) malloc (sizeof (s)+1);
				sprintf (Str, "%i", s);
				CopyString (filePath, LengthPathName, "_", 0, 1);
				CopyString (filePath, LengthPathName+1, Str, 0, StringLength(Str));
				CopyString (filePath, StringLength(filePath), ext_file, 0, StringLength(ext_file));
				free(Str);
			}*/
			 
			
			if(s >= TWMMAXREC)
			{
				// error - too many records!
				stop = NISCOPE_VAL_TRUE;
				continue;
			}
			

			//###note: inserted TWM style session.info header generator
			//###note: this should actually happen when all records are done, so each record has "actual"
			//         parameters obtained from channel session, but that would require to rewrite this whole file...
			//         So let's stick with minimum solution for now
			 
			// create record info structure
			TTWMssnInf info;
			memset((void*)&info, 0, sizeof(info)); // always clear it before filling new stuff!!!
		
			// now fill in the basic sampling informations
			info.N = minRecordLength; // samples count
			info.fs = minSampleRate; // sampling rate
			info.chn_count = 2*numberOfSessions; // digitizer channels count (not transducers!)
			info.chn_data_type = TWMMATFMT_DBL; // sample data in real64 format
			info.rec_count = (s+1); // records count (increases every time this is run, so session.info stays up to date)
		
			
			// -- now build list of digitizer channel setups for up to TWMMAXTR = 6 channels
			for(int k = 0;k < info.chn_count;k++)
			{
				sprintf(info.chn_idns[k],"channel %d",k+1); // create name of dig. channel 1
				info.chn_gains[0][k] = 1.0; // sample gain factor
				info.chn_offs[0][k] = 0.0; // sample offset
				info.chn_rng[k] = 1.0; // nominal range of channel
				info.time_stamps[0][k] = 0.0; // relative timestamp of channel (may be zero)
			}
			
			// obtain correction from GUI
			GetCorrectionsFromGUI(0,&info);
			
			// creates list of all record names to this moment to the session.info
			// note: at the same time create record file path for s-th record
			strcpy(filePath,filePathDef);
			twm_gen_session_rec_names(&info, filePath, s);
			
			// extract measurement folder and create relative record path
			char meas_fld[TWMMAXSTR];
			strcpy(meas_fld, filePath);
			char *pstr = strrchr(meas_fld, '\\');
			if(pstr)
				*pstr++ = '\0';
			
			// generate measurement session
			twm_write_session(meas_fld, &info);
			
			//###note: end of modification
			
			
			
			
			 fileHandle = fopen(filePath, "w"); 
         }
       else
          {
            fileFlag = 0;
            //return -1;
          }
    
   
  
	  

      ///////////////////////////////////// Configure the common device parameters ///////////////////////////////////////////////////////////
      for(i=0; i<numberOfSessions; i++)
      {
         vi = sessions[i];
         // Configure the acquisition type
		//  handleNIScopeErr (niScope_ConfigureAcquisitionType (vi, acquisitionType));  
         handleNIScopeErr (niScope_ConfigureAcquisition (vi, acquisitionType));

         // Configure the vertical parameters
         handleNIScopeErr (niScope_ConfigureVertical (vi, channelName, verticalRange, verticalOffset,
                                                      verticalCoupling, probeAttenuation, NISCOPE_VAL_TRUE));

         // Configure the channel characteristics
         handleNIScopeErr (niScope_ConfigureChanCharacteristics (vi, channelName, inputImpedance,
                                                                 maxInputFrequency));

         // Configure the horizontal parameters
         handleNIScopeErr (niScope_ConfigureHorizontalTiming (vi, minSampleRate, minRecordLength,
                                                             refPosition, numRecords, VI_TRUE));
     	 
	  }

	  
	  /////////////////////////////////////////////CONFIGURE TRIGGER//////////////////////////////////////////////////////////////////////////////
      vi = sessions[0];
      // Configure the trigger type for the master only
      switch (triggerType)
      {
         case 0: //Edge Trigger
            handleNIScopeErr (niScope_ConfigureTriggerEdge (vi, triggerSource, triggerLevel,
                                                            triggerSlope, triggerCoupling,
                                                            triggerHoldoff, triggerDelay));
	
            break;

         case 1: //Hysteresis Trigger
            handleNIScopeErr (niScope_ConfigureTriggerHysteresis (vi, triggerSource, triggerLevel,
                                                                  hysteresis, triggerSlope,
                                                                  triggerCoupling, triggerHoldoff,
                                                                  triggerDelay));
            break;

         case 2: //Digital Trigger
            handleNIScopeErr (niScope_ConfigureTriggerDigital (vi, triggerSource, triggerSlope,
                                                               triggerHoldoff, triggerDelay));
            break;

         case 3: //Window Trigger
            handleNIScopeErr (niScope_ConfigureTriggerWindow (vi, triggerSource, lowWindowLevel,
                                                              highWindowLevel, windowMode,
                                                              triggerCoupling, triggerHoldoff,
                                                              triggerDelay));
            break;

         case 4: //Immediate Triggering
            handleNIScopeErr (niScope_ConfigureTriggerImmediate (vi));
            break;

         default:
            //don't do anything
            break;
      }

	 /////////////////////////////////////////////Resync the devices/////////////////////////////////////////////////////////////////////////////////////// 
      // Resync the devices if any sync related properties change
      // Note: the NI 5922 digitizer requires resynchronization after changing any
      // channel parameters.
      if(    previousTriggerType != triggerType
             || previousMinSampleRate != minSampleRate
             || previousMaxInputFrequency != maxInputFrequency
             || previousMinRecordLength != minRecordLength )
      {
         // Use NI-TClk to configure appropriate parameters, synchronize digitizers, and initiate operation.
         handleNITClkErr(niTClk_ConfigureForHomogeneousTriggers(numberOfSessions, sessions));
         handleNITClkErr(niTClk_Synchronize(numberOfSessions, sessions, 0.0));
      }
      previousTriggerType = triggerType;
      previousMinSampleRate = minSampleRate;
      previousMaxInputFrequency = maxInputFrequency;
      previousMinRecordLength = minRecordLength;
	  
	  
	  /*////////////////////////////////////////Initiates the acquisition or generation sessions specified,
taking into consideration any special requirements needed for
synchronization.
////////////////////////////////////////////////////////////////////////////////////////////////////////*/

      
	  handleNITClkErr(niTClk_Initiate(numberOfSessions, sessions));

      ClearPlots();

	  
	//////////////////////////////////////////////////////////////////////////////////////////////////  
	  for(i=0; i<numberOfSessions; i++)
      {
		 handleNIScopeErr(_fetchAndPlotData(sessions[i], channelName, timeout));
      }
      
	  CommitPlots();
																	 
      // Find out whether to stop or not
      
	  SingleShotControll (&SS_Controll);
	  
	  switch (SS_Controll)
	  {
		  case 1:   // caso singolo shot
			     stop = NISCOPE_VAL_TRUE;
			   break;
			  
		  case 0:  // caso acquisizione continua
				stop = NISCOPE_VAL_FALSE;
			   break;
	  } // end switch
	  
	  if(fileFlag==1)
	  {
	    s++; // Variabile per il controllo del nome file
	  }
	  
	  
	  ProcessEvent ((int*)&stop);
	  
	  
   } // End while

   
   Error :
   // Even on error, redraw the graph
   CommitPlots();
   // Display messages
   if (error == VI_SUCCESS)
      strcpy (errorMessage, "Acquisition successful!");

   DisplayErrorMessageInGUI (error, errorMessage);
   Init=1;
   //ViUInt32 numberOfSessions=0;
  // vi=0;
   
   // Close the sessions
  //  for(i=0; i<numberOfSessions; i++)
	//{
	  	
     // niScope_close (sessions[i]); 
	 // sessions[i]=0;
//	}

 return error;
}






