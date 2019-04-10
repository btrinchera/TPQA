//==============================================================================
//
// Title:		utils.h
// Purpose:		A short description of the interface.
//
// Created on:	3.3.2014 at 4:04:55 by .
// Copyright:	. All Rights Reserved.
//
//==============================================================================

#ifndef __utils_H__
#define __utils_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

#include "cvidef.h"

//==============================================================================
// Constants
		
#define INFOMAXSTR 1024

//==============================================================================
// Types

//==============================================================================
// External variables

//==============================================================================
// Global functions

int csv_parse(char *csv,char col_sep,char row_sep,int *rows,int *cols,char **cells);
int create_fld_chain(char *path,int is_folder);

char *merge_path(char *dest, char *folder, char *file);

int info_write_text_column(FILE *fw, char *name, char str[][INFOMAXSTR], int rows);
int info_write_int_column(FILE *fw, char *name, int *value, int rows);
int info_write_dbl_column(FILE *fw, char *name, double *value, int rows);
int info_write_dbl_row(FILE *fw, char *name, double *val, int cols);
int info_write_int(FILE *fw, char *name, int value);
int info_write_dbl(FILE *fw, char *name, double value);
int info_write_string(FILE *fw, char *name, char *value);
int info_write_dbl_matrix(FILE *fw, char *name, double *value, int rows, int cols, int cols_total);
int info_get_int(char *info, char *key, int *value);


#ifdef __cplusplus
    }
#endif

#endif  /* ndef __utils_H__ */
