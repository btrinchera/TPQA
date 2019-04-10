//==============================================================================
//
// Title:		matlab_module.h
// Purpose:		A short description of the interface.
//
// Created on:	2.3.2014 at 17:46:31 by .
// Copyright:	. All Rights Reserved.
//
//==============================================================================

#ifndef __mlink_H__
#define __mlink_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

#include "cvidef.h"
#include "engine.h"

//==============================================================================
// Constants
		
#define ML_MATLAB 0
#define ML_OCTAVE 1

//==============================================================================
// Types
		
typedef struct{
	int mode;
	Engine *eng;
}TMLink;

//==============================================================================
// External variables


//==============================================================================
// Global functions

int mlink_init(TMLink *lnk,int mode);
int mlink_start(TMLink *lnk);
int mlink_close(TMLink *lnk,int close);
int mlink_cmd(TMLink *lnk,char *cmd,char *ret,int retmax);

int mlink_get_var_str(TMLink *lnk,char *var_name,char **data);
int mlink_get_var_dbl_vec(TMLink *lnk,char *var_name,double **data,int *size);
int mlink_get_var_int_vec(TMLink *lnk,char *var_name,int **data,int *size);


#ifdef __cplusplus
    }
#endif

#endif  /* ndef __matlab_module_H__ */
