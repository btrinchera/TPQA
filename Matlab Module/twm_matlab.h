//==============================================================================
//
// Title:		twm_matlab.h
// Purpose:		A short description of the interface.
//
// Created on:	2.3.2014 at 20:14:31 by .
// Copyright:	. All Rights Reserved.
//
//==============================================================================

#ifndef __twm_matlab_H__
#define __twm_matlab_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

#include "cvidef.h"
#include "mlink.h"
#include "utils.h"

//==============================================================================
// Constants
		
#define TWMMAXTR 6      /* maximum transducers */
#define TWMMAXCHN 6     /* maximum digitizer channels */
#define TWMMAXREC 100   /* maximum records count */
#define TWMMAXSTR INFOMAXSTR  /* maximum string length */
#define TWMMAXPARAM 64  /* maximum count of QWTB parameters */
#define TWMSSNINFO "session.info" /* name of the session info file */ 
#define TWMQWTBINFO "qwtb.info" /* name of the qwtb control structure */
		
// sample data types
#define TWMMATFMT_DBL 0 /* double format */
#define TWMMATFMT_SGL 10 /* single real format */
#define TWMMATFMT_I32 20 /* int32 format */
#define TWMMATFMT_I16 30 /* int16 format */

//==============================================================================
// Types
		
typedef struct{
	int max_dim;
	int max_array;
	int group_mode;
	int unc_mode;
	int phi_mode;
	int phi_ref_chn;
}TResCfg;


typedef struct{
	char alg_id[TWMMAXSTR]; /* QWTB algorithm id string */
	int all_rec; /* do all records at once */
	double loc; /* level of confidence (0.95) */
	char unc_mode[TWMMAXSTR]; /* uncertainty mode string QWTB style */
	int mcm_cyc; /* monte carlo iterations */
	
	int par_count; /* parameters count */
	char par_names[TWMMAXPARAM][TWMMAXSTR]; /* parameter names */
	char par_data[TWMMAXPARAM][TWMMAXSTR]; /* parameters data rows for each par.: "value1;value2;..." */
}TTWMqwtbCfg;

typedef struct{
	
	char dig_corr[TWMMAXSTR]; /* digitizer correction path */

	int tr_count; /* transduers count */
	char tr_corr[TWMMAXTR][TWMMAXSTR]; /* transducer correction paths [transducer][char path] */
	int tr_map[TWMMAXTR][2]; /* transducer to digitizer mapping [][0] = high-side, [][1] = low_side or 0 to not use */
	int tr_phase[TWMMAXTR]; /* transducer phase assignement {1, 2, 3, ...} */
	
	double fs; /* sampling rate [Hz] */
	double aperture; /* aperture [s] */
	int N; /* samples count */
	
	int rec_count; /* records count */
	
	int chn_count; /* digitizer channels count */
	char chn_idns[TWMMAXCHN][TWMMAXSTR]; /* digitizer channel identifiers */
	double chn_rng[TWMMAXCHN]; /* digitizer channel nominal ranges [V] */
	
	double chn_gains[TWMMAXREC][TWMMAXCHN]; /* gain scaling factor of sample data */
	double chn_offs[TWMMAXREC][TWMMAXCHN]; /* offset of sample data */
	double time_stamps[TWMMAXREC][TWMMAXCHN]; /* relative time-stamps for each channel */
	void *chn_data[TWMMAXREC][TWMMAXCHN]; /* list of poitners to each channel's sample data array */
	char chn_data_paths[TWMMAXREC][TWMMAXSTR]; /* paths to record files (applies if chn_data[] == NULL!) */
	int chn_data_type; /* sample data type tag (see constants def) */
	
	//TTWMqwtbCfg qwtb; /* qwtb processing setup */
						 
}TTWMssnInf;



//==============================================================================
// External variables

//==============================================================================
// Global functions

int twm_init(TMLink *lnk,char **errstr,char *path);
int twm_get_result_info(TMLink *lnk,char **errstr,char *path,char *alg_id,
						int *res_exist,char **res_files, char **alg_list, char **chn_list);
int twm_get_result_data(TMLink *lnk,char **errstr,char *path,int res_id,char *alg_id,TResCfg *cfg,
					    char **csv);
int twm_exec_algorithm(TMLink *lnk,char **errstr,char *path, char *calc_unc, int is_last_avg, int avg_id, int group_id);
int twm_get_alg_list(TMLink *lnk,char **errstr,char **alg_ids,char **alg_names);
int twm_get_alg_info(TMLink *lnk,char *alg_id,char **errstr,char **par_tab,char **par_list,
					 int *has_ui,int *is_diff,int *is_multi,int *unc_guf,int *unc_mcm);

int twm_write_mat(char *path,char *name,int fmt,int chn_count,int smpl_count,void **data);
int twm_write_session(char *meas_path, TTWMssnInf *inf);
int twm_write_proc_info(char *meas_path, TTWMqwtbCfg *qwtb);
int twm_read_session(char *meas_path, TTWMssnInf *inf);

int twm_gen_session_rec_names(TTWMssnInf *info, char *filePath, int rec_id);



#ifdef __cplusplus
    }
#endif

#endif  /* ndef __twm_matlab_H__ */
