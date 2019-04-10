//==============================================================================
//
// Title:		utils.c
// Purpose:		A short description of the implementation.
//
// Created on:	3.3.2014 at 4:04:55 by .
// Copyright:	. All Rights Reserved.
//
//==============================================================================

//==============================================================================
// Include files

#include <utility.h>
#include <ansi_c.h>
#include "utils.h"

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


// parser of CSV string, usage:
//   int cc = csv_parse(csv,';','\n',&rows,&cols,NULL); /* first pass parse to get dimensions */
//	 char **cells = (char**)malloc(cc*sizeof(char*)); /* allocate cell pointers array */
//   csv_parse(csv,';','\n',&rows,&cols,cells); /* second pass parse to fill in cell pointers */
int csv_parse(char *csv,char col_sep,char row_sep,int *rows,int *cols,char **cells)
{
	
	char *ptr = csv;
	
	if(!cells)
	{
		*cols = 1;
		*rows = 1;
	}
	else
	{
		char *nulchr = &csv[strlen(csv)];
		for(int k = 0;k < *cols**rows;k++)
			cells[k] = nulchr;
	}
	
	int col = 1;
	int row = 1;
	int new = 1;
	
	while(*ptr)
	{
		if(new && cells)
			cells[col-1 + *cols*(row-1)] = ptr;
		new = 0;
		
		if(*ptr == col_sep)
		{
			col++;
			if(col > *cols)
				*cols = col;
			new = 1;
		}
		else if(*ptr == row_sep)
		{
			col = 1;
			row++;
			if(row > *rows)
				*rows = row;
			new = 1;
		}
		if(new && cells)
			*ptr = '\0';
		
		ptr++;
		
	}
	
	return((*rows) * (*cols));
	
}


// ensure destination folder exists
// can create wghole chaing of folders if they not exist yet
//  *path - file path
//  is_folder - non-zero if path is folder, zero if path is file
int create_fld_chain(char *path,int is_folder)
{
	int exist;
	char *p;
	
	// temp copy of folder path
	char fld[MAX_PATHNAME_LEN];
	strcpy(fld,path);
	
	if(!is_folder)
	{
		// extract folder path
		p = strrchr(fld, '\\');
    	if(p) *p = '\0';	
	}
	
	int prevBOLE = SetBreakOnLibraryErrors(0);
	exist = MakeDir(fld);
	SetBreakOnLibraryErrors(prevBOLE);
	if(exist == 0 || exist == -9)
		return(0); // we are done
	
	// folder not exist, but we have to check if parent folder exist first
	
	// temp copy of folder path
	char pfld[MAX_PATHNAME_LEN];
	strcpy(pfld,fld);
	
	// extract parent folder path
	p = strrchr(pfld, '\\');
    if(p)
	{
		// some folder path exist
		*p = '\0';
		
		// recoursive
		create_fld_chain(pfld,1);
	}
	
	// make folder
	prevBOLE = SetBreakOnLibraryErrors(0);
	exist = MakeDir(fld);
	SetBreakOnLibraryErrors(prevBOLE);
	
	
	return(exist != 0 && exist != -9);
	
}




// merge folder and file
//   dest <= folder + file
char *merge_path(char *dest, char *folder, char *file)
{
	if(dest != folder)
		strcpy(dest,folder);
	
	if(dest[strlen(dest)-1] != '\\')
		strcat(dest,"\\");
	
	strcat(dest, file);
	
	return(dest);
}


// write INFO file text column
int info_write_text_column(FILE *fw, char *name, char str[][INFOMAXSTR], int rows)
{
	fprintf(fw, "#startmatrix:: %s\n", name);
	
	for(int k = 0;k < rows;k++)
		fprintf(fw, "\t%s\n",str[k]);
	
	fprintf(fw, "#endmatrix:: %s\n", name);
	
	return(0);
}

// write INFO file integer columns
int info_write_int_column(FILE *fw, char *name, int *value, int rows)
{
	fprintf(fw, "#startmatrix:: %s\n", name);
	
	for(int k = 0;k < rows;k++)
		fprintf(fw, "\t%d\n",value[k]);
	
	fprintf(fw, "#endmatrix:: %s\n", name);
	
	return(0);
}

// write INFO file real64 columns
int info_write_dbl_column(FILE *fw, char *name, double *value, int rows)
{
	fprintf(fw, "#startmatrix:: %s\n", name);
	
	for(int k = 0;k < rows;k++)
		fprintf(fw, "\t%.15g\n",value[k]);
	
	fprintf(fw, "#endmatrix:: %s\n", name);
	
	return(0);
}


// write INFO file real64 matrix
int info_write_dbl_matrix(FILE *fw, char *name, double *value, int rows, int cols, int cols_total)
{
	fprintf(fw, "#startmatrix:: %s\n", name);
	
	for(int r = 0;r < rows;r++)
	{
		fprintf(fw, "\t");
		for(int c = 0;c < cols;c++)
			fprintf(fw, "%.15g%s", value[c + r*cols_total], (c == cols-1)?"\n":";");
	}
	
	fprintf(fw, "#endmatrix:: %s\n", name);
	
	return(0);
}



// write INFO file row of real64
int info_write_dbl_row(FILE *fw, char *name, double *val, int cols)
{
	fprintf(fw, "#startmatrix:: %s\n", name);
	
	if(cols)
		fprintf(fw, "\t");
	for(int k = 0;k < cols;k++)
		fprintf(fw, "%g%s",val[k],(k==cols-1)?"":"; ");
	fprintf(fw, "\n");
	
	fprintf(fw, "#endmatrix:: %s\n", name);
	
	return(0);
}

// write INFO file integer
int info_write_int(FILE *fw, char *name, int value)
{
	fprintf(fw, "%s:: %d\n", name, value);
	
	return(0);
}

// write INFO file real64
int info_write_dbl(FILE *fw, char *name, double value)
{
	fprintf(fw, "%s:: %.15g\n", name, value);
	
	return(0);
}

// write INFO file string
int info_write_string(FILE *fw, char *name, char *value)
{
	fprintf(fw, "%s:: %s\n", name, value);
	
	return(0);
}

// read INFO integere key
int info_get_int(char *info, char *key, int *value)
{
	char keystr[INFOMAXSTR];
	strcpy(keystr,key);
	strcat(keystr,"::");
	char *pstr = strstr(info, keystr);
	if(!pstr)
		return(1);
	pstr += strlen(keystr);
	sscanf(pstr,"%d",value);
	return(0);
}
