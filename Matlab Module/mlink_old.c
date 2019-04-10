//==============================================================================
//
// Title:		matlab_module.c
// Purpose:		A short description of the implementation.
//
// Created on:	2.3.2014 at 17:46:31 by .
// Copyright:	. All Rights Reserved.
//
//==============================================================================

//==============================================================================
// Include files

#include <ansi_c.h>
#include "mlink.h"
#include "engine.h"
#include "matrix.h"


//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// Static global variables

//==============================================================================
// Static functions

//==============================================================================
// Global variables

//==============================================================================
// Global functions


// --- initialize Matlab link lib ---
int mlink_init(TMLink *lnk,int mode)
{
	
	lnk->eng = NULL;
	lnk->mode = mode;
	
	// Octave not done yet
	return(mode != ML_MATLAB/* && mode != ML_OCTAVE*/);
}


// --- start Matlab ---
int mlink_start(TMLink *lnk)
{
	if(lnk->mode == ML_MATLAB)
	{
		// MATLAB mode:
		
		// close link before
		if(lnk->eng)
			engClose(lnk->eng);
			
		
		// try to open
		lnk->eng = engOpen(NULL);
		
		return(lnk->eng == NULL);
	}
	else
	{
		// OCTAVE mode:
		
		return(1);
	}
}

// --- close Matlab ---
int mlink_close(TMLink *lnk,int close)
{
	if(lnk->mode == ML_MATLAB)
	{
		// MATLAB mode:
		
		if(lnk->eng)
		{
			// evaluate "exit" command?
			if(close)
			{
				engEvalString(lnk->eng,"exit");
			}
			
			// close engine
			engClose(lnk->eng);
			lnk->eng = NULL;
		}
		
		return(0);
	}
	else
	{
		// OCTAVE mode:
		
		return(1);
	}		
}


// --- exec command Matlab ---
int mlink_cmd(TMLink *lnk,char *cmd,char *ret,int retmax)
{
	if(lnk->mode == ML_MATLAB)
	{
		// MATLAB mode:
		
		// register answer buffer?
		if(ret)
			engOutputBuffer(lnk->eng,ret,retmax-1);
		else
			engOutputBuffer(lnk->eng,NULL,0);
		
		// clear errors
		engEvalString(lnk->eng,"lasterror('reset');");

		// eval command
		engEvalString(lnk->eng,cmd);
		
		// unregister buffer
		engOutputBuffer(lnk->eng,NULL,0);
		
		// detect error
		if(ret)
		{
			// clear errors
			engEvalString(lnk->eng, "errmsg = lasterror(); errmsg = errmsg.message;");
			
			// check error presence
			char *errmsg = NULL;
			int ret = mlink_get_var_str(lnk, "errmsg",&errmsg);
			if(!ret && strlen(errmsg))
			{
				// yes
				return(1);
			}
			if(!ret)
				free((void*)errmsg);
			
		}
		
		return(0);
	}
	else
	{
		// OCTAVE mode:
		
		return(1);
	}		
}


// --- Get string variable from engine ---
int mlink_get_var_str(TMLink *lnk,char *var_name,char **data)
{
	
	// no data yet
	*data = NULL;
	
	if(lnk->mode == ML_MATLAB)
	{
		// MATLAB mode:
		
		
		// try to obtain variable
		mxArray *arr = engGetVariable(lnk->eng,var_name);
		if(!arr)
		{
			// not exist!
			return(1);
		}
		
		if(!mxIsChar(arr))
		{
			// not char array!
			mxDestroyArray(arr);
			return(1);
		}
		
		// get var dimensions
		size_t cols = mxGetN(arr);
		size_t rows = mxGetM(arr);
		
		// allocate string buffer
		*data = (char*)malloc((rows*cols + 1)*sizeof(char));
		**data = '\0';
		
		// try to read string data
		mxGetString(arr,*data,cols*rows + 1);
		
		// get rid of the array		
		mxDestroyArray(arr);
		
		
		return(0);
	}
	else
	{
		// OCTAVE mode:
		
		return(1);
	}		
}



// --- Get real vector variable from engine ---
int mlink_get_var_dbl_vec(TMLink *lnk,char *var_name,double **data,int *size)
{
	
	// no data yet
	*data = NULL;
	
	if(lnk->mode == ML_MATLAB)
	{
		// MATLAB mode:
		
		
		// try to obtain variable
		mxArray *arr = engGetVariable(lnk->eng,var_name);
		if(!arr)
		{
			// not exist!
			return(1);
		}
		
		if(!mxIsNumeric(arr) || mxIsComplex(arr))
		{
			// not numeric array!
			mxDestroyArray(arr);
			return(1);
		}
		
		// get dimensions
		size_t dimn = mxGetNumberOfDimensions(arr);
		mwSize *sz = mxGetDimensions(arr);
		
		// total elements
		size_t num = mxGetNumberOfElements(arr);
		
		if(dimn>2 || (dimn == 2 && sz[0]>1 && sz[1]>1))
		{
			// not 1D
			mxDestroyArray(arr);
			return(1);
		}
		
		// array data pointer
		void *dptr = mxGetData(arr);
		
		// allocate string buffer
		*data = (double*)malloc(num*sizeof(double));
		
		if(mxIsDouble(arr))
		{
			// --- double array
			
			// copy array
			memcpy((void*)*data,dptr,num*sizeof(double));
			
			// total elements
			*size = (int)num;
			
		}
		else if(mxIsInt32(arr))
		{
			// --- int32 array - cast to double and no bitching...
			
			int *iptr = (int*)dptr;
			for(int k = 0;k < num;k++)
				(*data)[k] = (double)*iptr++;
						
			// total elements
			*size = (int)num;						
			
		}
		else
		{
			// --- not supported
			
			free((void*)*data);
			*data = NULL;
			*size = 0;
			
			mxDestroyArray(arr);
			return(1);
			
		}
		
		// get rid of the array		
		mxDestroyArray(arr);
		
		
		return(0);
	}
	else
	{
		// OCTAVE mode:
		
		return(1);
	}		
}


// --- Get int32 vector variable from engine ---
int mlink_get_var_int_vec(TMLink *lnk,char *var_name,int **data,int *size)
{
	
	// no data yet
	*data = NULL;
	
	if(lnk->mode == ML_MATLAB)
	{
		// MATLAB mode:
		
		
		// try to obtain variable
		mxArray *arr = engGetVariable(lnk->eng,var_name);
		if(!arr)
		{
			// not exist!
			return(1);
		}
		
		if(!mxIsNumeric(arr) || mxIsComplex(arr))
		{
			// not numeric array!
			mxDestroyArray(arr);
			return(1);
		}
		
		// get dimensions
		size_t dimn = mxGetNumberOfDimensions(arr);
		mwSize *sz = mxGetDimensions(arr);
		
		// total elements
		size_t num = mxGetNumberOfElements(arr);
		
		if(dimn>2 || (dimn == 2 && sz[0]>1 && sz[1]>1))
		{
			// not 1D
			mxDestroyArray(arr);
			return(1);
		}
		
		// array data pointer
		void *dptr = mxGetData(arr);
		
		// allocate string buffer
		*data = (int*)malloc(num*sizeof(int));
		
		if(mxIsDouble(arr))
		{
			// --- double array - cast to int32
			
			double *ptr = (double*)dptr;
			for(int k = 0;k < num;k++)
				(*data)[k] = (int)*ptr++;
			
			// total elements
			*size = (int)num;
			
		}
		else if(mxIsInt32(arr))
		{
			// --- int32 array
			
			// copy array
			memcpy((void*)*data,dptr,num*sizeof(int));
			
			// total elements
			*size = (int)num;						
			
		}
		else
		{
			// --- not supported
			
			free((void*)*data);
			*data = NULL;
			*size = 0;
			
			mxDestroyArray(arr);
			return(1);
			
		}
		
		// get rid of the array		
		mxDestroyArray(arr);
		
		
		return(0);
	}
	else
	{
		// OCTAVE mode:
		
		return(1);
	}		
}
