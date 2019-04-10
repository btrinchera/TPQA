#ifndef __tpqa_globals_H__
#define __tpqa_globals_H__

extern int GetCorrectionsFromGUI(int panel,TTWMssnInf *info);

// helper function to obtain stuff from TPQA GUI
extern int GetParametersFromGUI (ViChar* channel,
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
							 ViInt32 *fileFlag);

#endif
