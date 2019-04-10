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

#define  PANEL                            1
#define  PANEL_resourceName2              2       /* control type: string, callback function: (none) */
#define  PANEL_resourceName1              3       /* control type: string, callback function: (none) */
#define  PANEL_channelName                4       /* control type: string, callback function: (none) */
#define  PANEL_timeout                    5       /* control type: numeric, callback function: (none) */
#define  PANEL_acquisitionType            6       /* control type: ring, callback function: (none) */
#define  PANEL_verticalRange              7       /* control type: numeric, callback function: (none) */
#define  PANEL_verticalOffset             8       /* control type: numeric, callback function: (none) */
#define  PANEL_probeAttenuation           9       /* control type: numeric, callback function: (none) */
#define  PANEL_verticalCoupling           10      /* control type: ring, callback function: (none) */
#define  PANEL_minSampleRate              11      /* control type: numeric, callback function: (none) */
#define  PANEL_actualSampleRate           12      /* control type: numeric, callback function: (none) */
#define  PANEL_DispWavPoints              13      /* control type: numeric, callback function: (none) */
#define  PANEL_minRecordLength            14      /* control type: numeric, callback function: (none) */
#define  PANEL_actualRecordLength         15      /* control type: numeric, callback function: (none) */
#define  PANEL_numRecords                 16      /* control type: numeric, callback function: (none) */
#define  PANEL_maxInputFrequency          17      /* control type: numeric, callback function: (none) */
#define  PANEL_inputImpedance             18      /* control type: ring, callback function: (none) */
#define  PANEL_SingleShot                 19      /* control type: command, callback function: SingleShot */
#define  PANEL_ACQUIRE                    20      /* control type: command, callback function: Acquisition */
#define  PANEL_About                      21      /* control type: command, callback function: About */
#define  PANEL_STOP                       22      /* control type: command, callback function: Stop */
#define  PANEL_QUIT                       23      /* control type: command, callback function: Shutdown */
#define  PANEL_FolderMeasData             24      /* control type: picture, callback function: FolderMeasData */
#define  PANEL_triggerType                25      /* control type: ring, callback function: (none) */
#define  PANEL_triggerSource              26      /* control type: ring, callback function: (none) */
#define  PANEL_refPosition                27      /* control type: numeric, callback function: (none) */
#define  PANEL_triggerHoldoff             28      /* control type: numeric, callback function: (none) */
#define  PANEL_triggerDelay               29      /* control type: numeric, callback function: (none) */
#define  PANEL_Ph_Shunt                   30      /* control type: numeric, callback function: (none) */
#define  PANEL_Ph_VD                      31      /* control type: numeric, callback function: (none) */
#define  PANEL_R_Shunt                    32      /* control type: numeric, callback function: (none) */
#define  PANEL_R_VD                       33      /* control type: numeric, callback function: (none) */
#define  PANEL_triggerLevel               34      /* control type: numeric, callback function: (none) */
#define  PANEL_triggerCoupling            35      /* control type: ring, callback function: (none) */
#define  PANEL_triggerSlope               36      /* control type: ring, callback function: (none) */
#define  PANEL_hysteresis                 37      /* control type: numeric, callback function: (none) */
#define  PANEL_windowMode                 38      /* control type: ring, callback function: (none) */
#define  PANEL_lowLevelWindow             39      /* control type: numeric, callback function: (none) */
#define  PANEL_highLevelWindow            40      /* control type: numeric, callback function: (none) */
#define  PANEL_errorMessage               41      /* control type: textBox, callback function: (none) */
#define  PANEL_acquisitionDiff            42      /* control type: graph, callback function: (none) */
#define  PANEL_acquisitionGraph           43      /* control type: graph, callback function: (none) */
#define  PANEL_DECORATION_VERTICAL_2      44      /* control type: deco, callback function: (none) */
#define  PANEL_TEXTMSG_10                 45      /* control type: textMsg, callback function: (none) */
#define  PANEL_TEXTMSG                    46      /* control type: textMsg, callback function: (none) */
#define  PANEL_DECORATION_HORIZONTAL      47      /* control type: deco, callback function: (none) */
#define  PANEL_TEXTMSG_3                  48      /* control type: textMsg, callback function: (none) */
#define  PANEL_TEXTMSG_5                  49      /* control type: textMsg, callback function: (none) */
#define  PANEL_TEXTMSG_6                  50      /* control type: textMsg, callback function: (none) */
#define  PANEL_TEXTMSG_7                  51      /* control type: textMsg, callback function: (none) */
#define  PANEL_DECORATION_TRIGGERING      52      /* control type: deco, callback function: (none) */
#define  PANEL_TEXTMSG_9                  53      /* control type: textMsg, callback function: (none) */
#define  PANEL_TEXTMSG_8                  54      /* control type: textMsg, callback function: (none) */
#define  PANEL_DECORATION_TIMING_3        55      /* control type: deco, callback function: (none) */
#define  PANEL_DECORATION_TIMING_2        56      /* control type: deco, callback function: (none) */
#define  PANEL_DECORATION_TIMING          57      /* control type: deco, callback function: (none) */
#define  PANEL_SaveMeasData               58      /* control type: radioButton, callback function: (none) */
#define  PANEL_SaveDate                   59      /* control type: radioButton, callback function: (none) */
#define  PANEL_NUMERIC                    60      /* control type: numeric, callback function: (none) */
#define  PANEL_Data_Processing            61      /* control type: command, callback function: DATA_Processing */
#define  PANEL_PICTURE_4                  62      /* control type: picture, callback function: (none) */
#define  PANEL_PICTURE_3                  63      /* control type: picture, callback function: (none) */
#define  PANEL_PICTURE_2                  64      /* control type: picture, callback function: (none) */
#define  PANEL_HW_Corrcetions             65      /* control type: command, callback function: HW_CORRECTIONS */
#define  PANEL_RecordsAcquired            66      /* control type: numeric, callback function: (none) */
#define  PANEL_flag_HwCorrections         67      /* control type: radioButton, callback function: (none) */
#define  PANEL_flag_DataProcess           68      /* control type: radioButton, callback function: (none) */
#define  PANEL_DigDMM                     69      /* control type: command, callback function: DIGDMM */
#define  PANEL_LaunchMatlab               70      /* control type: command, callback function: Launch_Matlab */
#define  PANEL_pathMeasData               71      /* control type: string, callback function: (none) */
#define  PANEL_DECORATION                 72      /* control type: deco, callback function: (none) */
#define  PANEL_PICTURE                    73      /* control type: picture, callback function: (none) */
#define  PANEL_SPLITTER                   74      /* control type: splitter, callback function: (none) */
#define  PANEL_ModularSetup               75      /* control type: binary, callback function: ModularSETUP */

#define  PANEL_Abou                       2       /* callback function: CloseAbout */
#define  PANEL_Abou_PICTURE_2             2       /* control type: picture, callback function: (none) */

#define  PANEL_DMM                        3
#define  PANEL_DMM_DMM2                   2       /* control type: ring, callback function: (none) */
#define  PANEL_DMM_AWG_Visa_2             3       /* control type: ring, callback function: (none) */
#define  PANEL_DMM_AWG_Visa               4       /* control type: ring, callback function: (none) */
#define  PANEL_DMM_DMM1                   5       /* control type: ring, callback function: (none) */
#define  PANEL_DMM_QUIT_DMM               6       /* control type: command, callback function: Quit_DMM */
#define  PANEL_DMM_DMM2_range             7       /* control type: numeric, callback function: (none) */
#define  PANEL_DMM_DMM1_range             8       /* control type: numeric, callback function: (none) */
#define  PANEL_DMM_Synch_mode             9       /* control type: ring, callback function: (none) */
#define  PANEL_DMM_Counter                10      /* control type: ring, callback function: (none) */
#define  PANEL_DMM_AWG                    11      /* control type: ring, callback function: (none) */
#define  PANEL_DMM_DMMDigMode             12      /* control type: ring, callback function: (none) */
#define  PANEL_DMM_AutoCal_Dmms           13      /* control type: command, callback function: AUTOCAL_DMMs */
#define  PANEL_DMM_NRDGS                  14      /* control type: numeric, callback function: (none) */
#define  PANEL_DMM_Samp_Clock             15      /* control type: numeric, callback function: (none) */
#define  PANEL_DMM_SamplingDMMs           16      /* control type: command, callback function: Sampling_DMMs */
#define  PANEL_DMM_Test_DMMs              17      /* control type: command, callback function: TEST_DMMs */
#define  PANEL_DMM_Ext_Ref                18      /* control type: radioButton, callback function: (none) */
#define  PANEL_DMM_PresetNorm             19      /* control type: radioButton, callback function: (none) */
#define  PANEL_DMM_DMM_Streaming          20      /* control type: radioButton, callback function: (none) */
#define  PANEL_DMM_DECORATION_TIMING_2    21      /* control type: deco, callback function: (none) */
#define  PANEL_DMM_TEXTMSG_9              22      /* control type: textMsg, callback function: (none) */
#define  PANEL_DMM_STRING                 23      /* control type: string, callback function: (none) */
#define  PANEL_DMM_STRING_2               24      /* control type: string, callback function: (none) */
#define  PANEL_DMM_MAXSAMPLESCOUNT_4      25      /* control type: string, callback function: (none) */
#define  PANEL_DMM_MAXSAMPLESCOUNT_5      26      /* control type: string, callback function: (none) */
#define  PANEL_DMM_MAXSAMPLESCOUNT_6      27      /* control type: string, callback function: (none) */
#define  PANEL_DMM_MAXSAMPLESCOUNT_3      28      /* control type: string, callback function: (none) */
#define  PANEL_DMM_MAXSAMPLESCOUNT_2      29      /* control type: string, callback function: (none) */
#define  PANEL_DMM_MAXSAMPLESCOUNT        30      /* control type: string, callback function: (none) */
#define  PANEL_DMM_APERTURETIMERANGES     31      /* control type: string, callback function: (none) */
#define  PANEL_DMM_SAMPLINGRATERANGEHZ    32      /* control type: string, callback function: (none) */
#define  PANEL_DMM_DMMTriggerCoup         33      /* control type: ring, callback function: (none) */
#define  PANEL_DMM_DMMTriggerSlope        34      /* control type: ring, callback function: (none) */
#define  PANEL_DMM_DMMTrigger             35      /* control type: ring, callback function: (none) */
#define  PANEL_DMM_DMMTrigLevel           36      /* control type: numeric, callback function: (none) */
#define  PANEL_DMM_RepCycles              37      /* control type: numeric, callback function: (none) */
#define  PANEL_DMM_DMMText                38      /* control type: textBox, callback function: (none) */
#define  PANEL_DMM_Aper_Time              39      /* control type: numeric, callback function: (none) */
#define  PANEL_DMM_TEXTMSG                40      /* control type: textMsg, callback function: (none) */
#define  PANEL_DMM_TEXTMSG_10             41      /* control type: textMsg, callback function: (none) */
#define  PANEL_DMM_SINT_flag              42      /* control type: radioButton, callback function: (none) */
#define  PANEL_DMM_Actual_Aper            43      /* control type: string, callback function: (none) */

#define  PANEL_DP                         4
#define  PANEL_DP_TABLE_data              2       /* control type: table, callback function: ShowGraphCellTable */
#define  PANEL_DP_Close_DP                3       /* control type: command, callback function: CLOSE_DP */
#define  PANEL_DP_HarmonicAnalyzer        4       /* control type: textButton, callback function: (none) */
#define  PANEL_DP_Num_Harmonic_2          5       /* control type: numeric, callback function: (none) */
#define  PANEL_DP_ith_harmonic            6       /* control type: numeric, callback function: (none) */
#define  PANEL_DP_Num_Harmonic            7       /* control type: numeric, callback function: (none) */
#define  PANEL_DP_WindowHarmonic          8       /* control type: ring, callback function: (none) */
#define  PANEL_DP_acquisitionGraph_IHF    9       /* control type: graph, callback function: (none) */
#define  PANEL_DP_acquisitionGraph_UHF    10      /* control type: graph, callback function: (none) */
#define  PANEL_DP_Statistic               11      /* control type: radioButton, callback function: CheckStatistic */
#define  PANEL_DP_PICTURE_4               12      /* control type: picture, callback function: (none) */
#define  PANEL_DP_DECORATION              13      /* control type: deco, callback function: (none) */
#define  PANEL_DP_TEXTMSG_10              14      /* control type: textMsg, callback function: (none) */
#define  PANEL_DP_Acquired_Cycles         15      /* control type: numeric, callback function: (none) */
#define  PANEL_DP_Refresh                 16      /* control type: command, callback function: Refresh */

#define  PANEL_GrI                        5       /* callback function: panel_GrI */
#define  PANEL_GrI_STRIPCHART_I           2       /* control type: strip, callback function: (none) */

#define  PANEL_GrV                        6       /* callback function: panel_GrV */
#define  PANEL_GrV_STRIPCHART_V           2       /* control type: strip, callback function: (none) */
#define  PANEL_GrV_Y_Precision            3       /* control type: numeric, callback function: YStripChartPrecision */
#define  PANEL_GrV_PScreen                4       /* control type: numeric, callback function: PointsStripChartScreen */

#define  PANEL_HW                         7
#define  PANEL_HW_TAB_Corrections         2       /* control type: tab, callback function: (none) */
#define  PANEL_HW_Close_Corrections       3       /* control type: command, callback function: CLOSE_Corrections */
#define  PANEL_HW_PutCorrections          4       /* control type: command, callback function: PutCorrections */
#define  PANEL_HW_Status_Corrections      5       /* control type: textBox, callback function: (none) */

     /* tab page panel controls */
#define  Digitizer_BTN_LOAD_DIG           2       /* control type: command, callback function: btn_load_dig */
#define  Digitizer_imm_3                  3       /* control type: picture, callback function: (none) */
#define  Digitizer_TEXT_DIG_PATH          4       /* control type: textBox, callback function: (none) */

     /* tab page panel controls */
#define  Transducer_SN_CSH_1              2       /* control type: string, callback function: (none) */
#define  Transducer_Name_CSH_1            3       /* control type: string, callback function: (none) */
#define  Transducer_SN_RVD_1              4       /* control type: string, callback function: (none) */
#define  Transducer_Phase_CSH             5       /* control type: string, callback function: (none) */
#define  Transducer_SH_PHID_1             6       /* control type: numeric, callback function: (none) */
#define  Transducer_RVD_PHID_1            7       /* control type: numeric, callback function: (none) */
#define  Transducer_Ratio_CSH_1           8       /* control type: string, callback function: (none) */
#define  Transducer_CSH_ADC_LP_1          9       /* control type: ring, callback function: (none) */
#define  Transducer_CSH_ADC_HP_1          10      /* control type: ring, callback function: (none) */
#define  Transducer_TEXTpathCSH_1         11      /* control type: textBox, callback function: (none) */
#define  Transducer_Name_RVD_1            12      /* control type: string, callback function: (none) */
#define  Transducer_Phase_RVD_2           13      /* control type: string, callback function: (none) */
#define  Transducer_Ratio_RVD_1           14      /* control type: string, callback function: (none) */
#define  Transducer_RVD_ADC_LP_1          15      /* control type: ring, callback function: (none) */
#define  Transducer_RVD_ADC_HP_1          16      /* control type: ring, callback function: (none) */
#define  Transducer_TEXTMSG               17      /* control type: textMsg, callback function: (none) */
#define  Transducer_TEXTpathRVD_1         18      /* control type: textBox, callback function: (none) */
#define  Transducer_DECORATION            19      /* control type: deco, callback function: (none) */
#define  Transducer_VOLTAGETTRANSDUCER    20      /* control type: textMsg, callback function: (none) */
#define  Transducer_TEXTMSG_3             21      /* control type: textMsg, callback function: (none) */
#define  Transducer_TEXTMSG_2             22      /* control type: textMsg, callback function: (none) */
#define  Transducer_DECORATION_2          23      /* control type: deco, callback function: (none) */
#define  Transducer_imm_3                 24      /* control type: picture, callback function: (none) */
#define  Transducer_imm_2                 25      /* control type: picture, callback function: (none) */
#define  Transducer_LoaddataRVD_1         26      /* control type: command, callback function: LoadTranData */
#define  Transducer_LoaddataCSH_1         27      /* control type: command, callback function: LoadTranData */
#define  Transducer_TEXTMSG_4             28      /* control type: textMsg, callback function: (none) */
#define  Transducer_TEXTMSG_5             29      /* control type: textMsg, callback function: (none) */


     /* Control Arrays: */

#define  CTRLARRAY                        1
#define  CTRLARRAY_2                      2
#define  CTRLARRAY_3                      3
#define  CTRLARRAY_4                      4

     /* Menu Bars, Menus, and Menu Items: */

#define  MENU_HWCor                       1
#define  MENU_HWCor_File                  2
#define  MENU_HWCor_File_Open             3
#define  MENU_HWCor_File_Close            4
#define  MENU_HWCor_File_SEPARATOR        5
#define  MENU_HWCor_File_Exit             6
#define  MENU_HWCor_Edit                  7
#define  MENU_HWCor_Operate               8
#define  MENU_HWCor_Tools                 9
#define  MENU_HWCor_Window                10
#define  MENU_HWCor_Help                  11


     /* Callback Prototypes: */

int  CVICALLBACK About(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Acquisition(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK AUTOCAL_DMMs(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK btn_load_dig(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CheckStatistic(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CLOSE_Corrections(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CLOSE_DP(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CloseAbout(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK DATA_Processing(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK DIGDMM(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK FolderMeasData(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK HW_CORRECTIONS(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Launch_Matlab(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK LoadTranData(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ModularSETUP(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK panel_GrI(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK panel_GrV(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK PointsStripChartScreen(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK PutCorrections(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Quit_DMM(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Refresh(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Sampling_DMMs(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ShowGraphCellTable(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Shutdown(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SingleShot(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Stop(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK TEST_DMMs(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK YStripChartPrecision(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
