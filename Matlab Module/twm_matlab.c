//==============================================================================
//
// Title:		twm_matlab.c
// Purpose:		A short description of the implementation.
//
// Created on:	2.3.2014 at 20:14:31 by .
// Copyright:	. All Rights Reserved.
//
//==============================================================================

//==============================================================================
// Include files

#include <ansi_c.h>
#include <windows.h>
#include <utility.h>
#include "twm_matlab.h"
#include "mlink.h"


//==============================================================================
// Constants

#define MAXERR 16384

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


//------------------------------------------------------------------------------
// Initialize TWM link - set TWM functions path
//
//  lnk - Matlab link session
//  path - TWM functions root folder
//  errstr - allocates and returns error string, if no error, returns NULL
//
// NOTE: Do not forget to free the allocated buffers!
//
int twm_init(TMLink *lnk,char **errstr,char *path)
{
	char cmd[MAX_PATH+100];
	int err;
	
	// allocate error string
	char *errbuf = (char*)malloc(MAXERR*sizeof(char));
	if(errstr)
		*errstr = errbuf;
	
	// add TWM root path
	sprintf(cmd,"addpath('%s');",path);
	err = mlink_cmd(lnk,cmd,errbuf,MAXERR);
	if(err)
		return(1);
	
	// add TWM info path
	sprintf(cmd,"addpath('%s\\info');",path);
	err = mlink_cmd(lnk,cmd,errbuf,MAXERR);
	if(err)
		return(1);
	
	// add QWTB path
	sprintf(cmd,"addpath('%s\\qwtb');",path);
	err = mlink_cmd(lnk,cmd,errbuf,MAXERR);
	if(err)
		return(1);
	
	// disable warnings
	sprintf(cmd,"warning('off');");
	err = mlink_cmd(lnk,cmd,errbuf,MAXERR);
	if(err)
		return(1);
	
	// no errors, loose error buffer
	free((void*)errbuf);
	if(errstr)
		*errstr = NULL;
	
	return(0);
}

						
//------------------------------------------------------------------------------
// TWM: load result(s) info(s)
//
//  lnk - Matlab link session
//  errstr - allocates and returns error string, if no error, returns NULL
//  path - measurement root folder
//  alg_id - string id of QWTB algorithms ("": last calculated)
//  res_exist - returns non-zero if selection is valid
//  res_files - allocates and returns pointer to list of result files,
//              csv string separated by tabs
//  alg_list - allocates and returns pointer to list of calc. algorithms
//             csv string separated by tabs
//  chn_list - allocates and returns pointer to list of possible ref. channels
//             csv string separated by tabs
//
// NOTE: Do not forget to free the allocated buffers!
//
int twm_get_result_info(TMLink *lnk,char **errstr,char *path,char *alg_id,
						int *res_exist,char **res_files, char **alg_list, char **chn_list)
{
	char cmd[MAX_PATH+200];
		
	// build command
	sprintf(cmd,"[res_files, res_exist, alg_list, chn_list] = qwtb_get_results_info('%s','%s');",path,alg_id);
	
	
	// allocate error string
	char *errbuf = (char*)malloc(MAXERR*sizeof(char));
	if(errstr)
		*errstr = errbuf;
	
	// exec command
	int err = mlink_cmd(lnk,cmd,errbuf,MAXERR);
	if(err)
		return(1);
	
	
	// read result exist flag
	if(res_exist)
	{
		*res_exist = 0;
		
		int *buf = NULL;
		int size;
		mlink_get_var_int_vec(lnk,"res_exist",&buf,&size);
		if(buf && size)
		{
			*res_exist = buf[0];
			free((void*)buf);
		}
	}
	
	// read result files list
	if(res_files)
	{
		*res_files = NULL;
		mlink_get_var_str(lnk,"res_files",res_files);
	}
	
	// read calculated algorithms files list
	if(alg_list)
	{
		*alg_list = NULL;
		mlink_get_var_str(lnk,"alg_list",alg_list);
	}
	
	// read channels list
	if(chn_list)
	{
		*chn_list = NULL;
		mlink_get_var_str(lnk,"chn_list",chn_list);
	}
	
	// no errors, loose error buffer
	free((void*)errbuf);
	if(errstr)
		*errstr = NULL;
	
	return(0);
	
}



//------------------------------------------------------------------------------
// TWM: load result data
//
//  lnk - Matlab link session
//  path - measurement root folder
//  res_id - ID of the result file to select (-1: last, 0: average, >0: IDs of files)
//  alg_id - string id of QWTB algorithms ("": last calculated)
//  cfg - display setup structure
//    cfg.max_dim - max shown dim (0: scalar, 1: vectors, 2: matrices)
//    cfg.max_array - max vector size to be shown
//    cfg.group_mode - grouping mode (0: quantities, 1: channels)
//    cfg.unc_mode - uncertainty display mode (0: none, 1: val±unc, 2: val;unc)
//    cfg.phi_mode - phase display mode (0: ±pi [rad], 1: 0-2pi [rad], 2: ±180 [deg], 3: 0-360 [deg])
//    cfg.phi_ref_chn - reference channel is (0: none, >0: channel ids)
//  csv - allocates and returns pointer to list 2D CSV table of results,
//        tabs separator
//
// NOTE: Do not forget to free the allocated buffers!
//
int twm_get_result_data(TMLink *lnk,char **errstr,char *path,int res_id,char *alg_id,TResCfg *cfg,
					   char **csv)
{
	char cmd[MAX_PATH+1024];
	
	// build command
	sprintf(cmd,"cfg = struct();\n"
				"cfg.max_dim = %d;\n"
				"cfg.max_array = %d;\n"
				"cfg.group_mode = %d;\n"
				"cfg.unc_mode = %d;\n"
				"cfg.phi_mode = %d;\n"
				"cfg.phi_ref_chn = %d;\n"
				"[csv, desc, var_names, chn_index] = qwtb_get_results('%s', %d, '%s', cfg);",
				cfg->max_dim,cfg->max_array,cfg->group_mode,cfg->unc_mode,cfg->phi_mode,cfg->phi_ref_chn,
				path,res_id,alg_id);
	
	
	// allocate error string
	char *errbuf = (char*)malloc(MAXERR*sizeof(char));
	if(errstr)
		*errstr = errbuf;
	
	// exec command
	int err = mlink_cmd(lnk,cmd,errbuf,MAXERR);
	if(err)
		return(1);
	
	// read result files list
	if(csv)
	{
		*csv = NULL;
		mlink_get_var_str(lnk,"csv",csv);
	}
	
	// no errors, loose error buffer
	free((void*)errbuf);
	if(errstr)
		*errstr = NULL;
	
	return(0);
}



//------------------------------------------------------------------------------
// TWM: call algorithms passing through qwtb.exec.algorithm.m of matlab
//
// TWM: Executes QWTB algorithm based on the setup from meas. session
// inputs:
//  **errstr - error string, autoallocates data if error
//  *path - path to measurement folder
//  *calc_unc - override uncertainty calculation mode (default "")
//  is_last_avg - is last record from repeated group?
//  avg_id - record id 
//  group_id - group id
//
// NOTE: Do not forget to free the allocated buffers!        
//
int twm_exec_algorithm(TMLink *lnk,char **errstr,char *path, char *calc_unc, int is_last_avg, int avg_id, int group_id)
{
	char cmd[MAX_PATH+1024];
	
	// build session full path
	char fullpath[TWMMAXSTR];
	merge_path(fullpath, path, TWMSSNINFO);
		
	// build command
	sprintf(cmd, "qwtb_exec_algorithm('%s', '%s', %d, %d, %d);", fullpath, calc_unc, is_last_avg, avg_id, group_id);
	
	// allocate error string
	char *errbuf = (char*)malloc(MAXERR*sizeof(char));
	if(errstr)
		*errstr = errbuf;
	
	// exec command
	int err = mlink_cmd(lnk,cmd,errbuf,MAXERR);
	if(err)
		return(1);
	
	// no errors, loose error buffer
	free((void*)errbuf);
	if(errstr)
		*errstr = NULL;
	
	
	return(0);
}



//------------------------------------------------------------------------------
// TWM: get list of algorithms
//
// inputs:
//  **errstr - error string, autoallocates data if error
//  **alg_ids - list of algorithms IDs (NULL if not needed)
//  **alg_names - list of algorithm names (NULL if not needed)
//
// NOTE: Do not forget to free the allocated buffers!        
//
int twm_get_alg_list(TMLink *lnk,char **errstr,char **alg_ids,char **alg_names)
{
	char cmd[1024];
		
	// build command
	sprintf(cmd,"[ids, names] = qwtb_load_algorithms('qwtb_list.info');");

	// allocate error string
	char *errbuf = (char*)malloc(MAXERR*sizeof(char));
	if(errstr)
		*errstr = errbuf;
	
	// exec command
	int err = mlink_cmd(lnk,cmd,errbuf,MAXERR);
	if(err)
		return(1);
	
	
	// load algorithm ids:
	if(alg_ids)
	{
		*alg_ids = NULL;
		mlink_get_var_str(lnk,"ids",alg_ids);
	}
	
	// load algorithm names:
	if(alg_names)
	{
		*alg_ids = NULL;
		mlink_get_var_str(lnk,"names",alg_names);
	}
	
	// no errors, loose error buffer
	free((void*)errbuf);
	if(errstr)
		*errstr = NULL;
	
	return(0);		
}

//------------------------------------------------------------------------------
// TWM: get algorithm info
//
// inputs:
//  *alg_id - algorithm ID string
//  **errstr - error string, autoallocates data if error
//  **par_tab - 2D table of parameters to be displayed in Table (auto alloc, NULL if not needed)
//  **par_list - list of names of parameters (auto alloc, NULL if not needed)
//  *has_ui - algorithm has U and I inputs  (auto alloc, NULL if not needed)
//  *is_diff - algorithm suports differential mode (auto alloc, NULL if not needed)
//  *is_multi - algorithm can process more records at once (auto alloc, NULL if not needed)
//  *unc_guf - algorithm can estimate uncertainty (auto alloc, NULL if not needed)
//  *unc_mcm - algorithm can calculate uncertainty by Monte Carlo (auto alloc, NULL if not needed)
//  
//
// NOTE: Do not forget to free the allocated buffers!        
//
int twm_get_alg_info(TMLink *lnk,char *alg_id,char **errstr,char **par_tab,char **par_list,
					 int *has_ui,int *is_diff,int *is_multi,int *unc_guf,int *unc_mcm)
{
	char cmd[1024];
		
	// build command
	//[alginfo,ptab,input_params,is_multi_inp,is_diff,has_ui,unc_guf,unc_mcm] = qwtb_load_algorithm(alg_id)
			
	sprintf(cmd,"[alginfo,ptab,par,is_multi_inp,is_diff,has_ui,unc_guf,unc_mcm] = qwtb_load_algorithm('%s');\n"
			    "flag = int32([has_ui,is_diff,is_multi_inp,unc_guf,unc_mcm]);",alg_id);

	// allocate error string
	char *errbuf = (char*)malloc(MAXERR*sizeof(char));
	if(errstr)
		*errstr = errbuf;
	
	// exec command
	int err = mlink_cmd(lnk,cmd,errbuf,MAXERR);
	if(err)
		return(1);
	
	// get flags
	int *flag;
	int size;
	mlink_get_var_int_vec(lnk,"flag",&flag,&size);
	if(has_ui)
		*has_ui = flag[0];
	if(is_diff)
		*is_diff = flag[1];
	if(is_multi)
		*is_multi = flag[2];
	if(unc_guf)
		*unc_guf = flag[3];
	if(unc_mcm)
		*unc_mcm = flag[4];
	free((void*)flag);
	
	
	// load parameters table:
	if(par_tab)
	{
		*par_tab = NULL;
		mlink_get_var_str(lnk,"ptab",par_tab);
	}
	
	// load parameter names
	if(par_list)
	{
		*par_list = NULL;
		mlink_get_var_str(lnk,"par",par_list);
	}
	
	// no errors, loose error buffer
	free((void*)errbuf);
	if(errstr)
		*errstr = NULL;
	
	return(0);		
}




//------------------------------------------------------------------------------
// TWM: write record session in TWM format  
//
// inputs:
//  *meas_path - root folder of measurement session
//  *inf - session structure
//
int twm_write_session(char *meas_path, TTWMssnInf *inf)
{
	
	
	// build sessio.info path
	char tsi[TWMMAXSTR];
	merge_path(tsi, meas_path, TWMSSNINFO);
	
	// create folder chain
	create_fld_chain(tsi,0);
	
	// create measurement file
	FILE *fw = fopen(tsi, "wt");
	
	// no data/just file paths mode
	// i.e. will not save MAT files, it will just store paths to files
	int no_data = (inf->chn_data[0][0] == NULL);
	
	
	fprintf(fw,"====== COMMON SETUP ======\n\n");
	
	info_write_text_column(fw, "channel descriptors", inf->chn_idns, inf->chn_count);
	info_write_text_column(fw, "auxiliary HW descriptors", NULL, 0);
	info_write_int(fw, "channels count", inf->chn_count);
	if(no_data)
		info_write_string(fw, "sample data format", "tpqa-txt");
	else
		info_write_string(fw, "sample data format", "mat-v4");
	info_write_string(fw, "sample data variable name", "y");
	info_write_int(fw, "groups count", 1);
	info_write_int(fw, "temperature available", 0);
	info_write_int(fw, "temperature log available", 0);
	
	fprintf(fw,"\n====== SETUP(S) FOR AVERAGE GROUPS ======\n\n");
	fprintf(fw,"#startsection:: measurement group 1\n\n");
	
	info_write_int(fw, "repetitions count", inf->rec_count);
	info_write_int(fw, "samples count", inf->N);
	info_write_int(fw, "bit resolution", 24); // to take from setup
	info_write_dbl(fw, "sampling rate [Sa/s]", inf->fs);
	info_write_dbl_row(fw, "voltage ranges [V]", inf->chn_rng, inf->chn_count);
	
	if(inf->aperture > 0)
		info_write_dbl_row(fw, "aperture [s]", &inf->aperture, 1);
	
	
	if(no_data)
	{
		// --- no data mode: just store file paths
		
		// write results matrix
		info_write_text_column(fw, "record sample data files", inf->chn_data_paths, inf->rec_count);
		
	}
	else
	{
		// --- data provided: store MAT files
		char rec_name[TWMMAXREC][TWMMAXSTR];
		for(int k = 0;k < inf->rec_count;k++)
		{
			// build record path
			sprintf(rec_name[k], "RAW\\G0001-A%04d.mat", k+1);
	
			// MAT file full path
			char rec_pth[TWMMAXSTR];
			merge_path(rec_pth, meas_path, rec_name[k]);
	
			// write MAT file with sample data
			twm_write_mat(rec_pth, "y", inf->chn_data_type, inf->chn_count, inf->N, (void**)inf->chn_data);
		}
		
		// write results matrix
		info_write_text_column(fw, "record sample data files", rec_name, inf->rec_count);
	}
	
	int N[TWMMAXREC];
	for(int k = 0;k < inf->rec_count;k++)
		N[k] = inf->N;
	info_write_int_column(fw, "record samples counts", N, inf->rec_count);
	
	double Ts[TWMMAXREC];
	for(int k = 0;k < inf->rec_count;k++)
		Ts[k] = 1.0/inf->fs;
	info_write_dbl_column(fw, "record time increments [s]", Ts, inf->rec_count);
	
	info_write_dbl_matrix(fw, "record sample data gains [V]", (double*)inf->chn_gains, inf->rec_count, inf->chn_count, TWMMAXCHN);
	info_write_dbl_matrix(fw, "record sample data offsets [V]", (double*)inf->chn_offs, inf->rec_count, inf->chn_count, TWMMAXCHN);
	info_write_dbl_matrix(fw, "record relative timestamps [s]", (double*)inf->time_stamps, inf->rec_count, inf->chn_count, TWMMAXCHN);
	
	fprintf(fw,"\n#endsection:: measurement group 1\n\n");
	
	
	
	fprintf(fw, "====== MEASUREMENT SETUP CONFIGURATION ======\n\n");
	fprintf(fw, "#startsection:: measurement setup configuration\n\n");
	
	info_write_string(fw, "digitizer corrections path", inf->dig_corr);
	
	info_write_text_column(fw, "transducer paths", inf->tr_corr, inf->tr_count);
	
	info_write_int_column(fw, "channel phase indexes", inf->tr_phase, inf->tr_count);
	
	char str[TWMMAXTR][TWMMAXSTR];
	for(int i = 0;i < inf->tr_count;i++)
	{
		if(inf->tr_map[i][1])
			sprintf(str[i],"%d;%d",inf->tr_map[i][0],inf->tr_map[i][1]); /* differential channel */
		else
			sprintf(str[i],"%d",inf->tr_map[i][0]); /* single-ended channel */
	}
	info_write_text_column(fw, "transducer to digitizer channels mapping", str, inf->tr_count);
	
	fprintf(fw, "\n#endsection:: measurement setup configuration\n\n");
	
	fclose(fw);
	
	
	
	
	// write processing info
	//twm_write_proc_info(meas_path, &inf->qwtb);
	
	return(0);
}



//------------------------------------------------------------------------------
// TWM: write processing info
// Note: NOT CALLED DIRECTLY - CALLED FROM twm_write_session()
//
// inputs:
//  *meas_path - root folder of measurement session
//  *qwtb - QWTB processing setup structure
//
int twm_write_proc_info(char *meas_path, TTWMqwtbCfg *qwtb)
{
	
	// build sessio.info path
	char qwin[MAX_PATH];
	merge_path(qwin, meas_path, TWMQWTBINFO);
	
	// create folder chain
	create_fld_chain(qwin,0);
	
	// create qwtb.info file
	FILE *fw = fopen(qwin, "wt");
	
	fprintf(fw,"====== QWTB processing setup ======\n\n");

	fprintf(fw,"#startsection:: QWTB processing setup\n\n");
	
	info_write_string(fw, "algorithm id", qwtb->alg_id);
	info_write_int(fw, "calculate whole average at once", qwtb->all_rec);
	info_write_string(fw, "uncertainty mode", qwtb->unc_mode);
	info_write_dbl(fw, "coverage interval [%]", qwtb->loc);
	info_write_int(fw, "monte carlo cycles", qwtb->mcm_cyc);
	
	info_write_text_column(fw, "list of parameter names", qwtb->par_names, qwtb->par_count);
	for(int k = 0;k < qwtb->par_count;k++)
		info_write_text_column(fw, qwtb->par_names[k], &qwtb->par_data[k], 1);
	
	
	fprintf(fw,"\n#endsection:: QWTB processing setup\n");
	
	fclose(fw);
	
	return(0);
}




//------------------------------------------------------------------------------
// TWM: write sample data to MAT file
//
// inputs:
//  *path - MAT file path
//  *name - variable name to be stored
//  fmt - data element format code (TWMMATFMT_???: SGL, DBL, I32, I16)
//  chn_count - number of channels to be stored
//  smpl_count - number of samples to be stored
//  **data - 2D array of samples data (array of pointers to arrays belonging each channel)
//
int twm_write_mat(char *path,char *name,int fmt,int chn_count,int smpl_count,void **data)
{
	
	// create folder chain
	create_fld_chain(path,0);

	// create binary file for writting
	FILE *fw = fopen(path,"wb");
	
	// store data type flag
	fwrite((void*)&fmt,4,1,fw);
	
	// write channels count (rows)
	fwrite((void*)&chn_count,4,1,fw);
	
	// write samples count (columns)
	fwrite((void*)&smpl_count,4,1,fw);
	
	// imaginary flag (always 0)
	int imag = 0;
	fwrite((void*)&imag,4,1,fw);
	
	// write variable name length (including '\0' terminator)
	int len = (int)strlen(name) + 1;
	fwrite((void*)&len,4,1,fw);
	
	// write the variable name + '\0' terminator
	fwrite((void*)name,len,1,fw);
	
	// now we can write data, per rows, so the sample data are written horizontally!
	switch(fmt)
	{
		case TWMMATFMT_DBL:
			
			double **dbl = (double**)data;
			
			for(int s = 0;s < smpl_count;s++)
				for(int c = 0;c < chn_count;c++)
					fwrite((void*)&dbl[c][s],sizeof(double),1,fw);
			
			break;
		
		case TWMMATFMT_SGL:
			
			float **sgl = (float**)data;
			
			for(int s = 0;s < smpl_count;s++)
				for(int c = 0;c < chn_count;c++)
					fwrite((void*)&sgl[c][s],sizeof(float),1,fw);
			
			break;
		
		case TWMMATFMT_I32:
			
			int **i32 = (int**)data;
			
			for(int s = 0;s < smpl_count;s++)
				for(int c = 0;c < chn_count;c++)
					fwrite((void*)&i32[c][s],4,1,fw);
			
			break;
		
		case TWMMATFMT_I16:
			
			short **i16 = (short**)data;
			
			for(int s = 0;s < smpl_count;s++)
				for(int c = 0;c < chn_count;c++)
					fwrite((void*)&i16[c][s],2,1,fw);
			
			break;
		
		default:
			return(1);

	}
	
	// close file
	fclose(fw);
	
	return(0);
}


//------------------------------------------------------------------------------
// Read SOME of the session.info elements
//
int twm_read_session(char *meas_path, TTWMssnInf *inf)
{
	// build session full path
	char path[TWMMAXSTR];
	merge_path(path, meas_path, TWMSSNINFO);
	
	FILE *fr = fopen(path, "rt");
	if(!fr)
	{
		// loading failed
		return(1);
	}
	
	// get file len
	fseek(fr, 0L, SEEK_END);
	long flen = ftell(fr);
	fseek(fr, 0L, SEEK_SET);
	

	// copy if to buffer
	char *info = (char*)malloc(flen+1);
	memset((void*)info,0,flen+1);
	fread((void*)info, 1, flen, fr);
	
	// close session
	fclose(fr);
	
	info_get_int(info, "samples count", &inf->N);
	info_get_int(info, "channels count", &inf->chn_count);
	info_get_int(info, "repetitions count", &inf->rec_count);
	
	// loose file buffer
	free((void*)info);
	
	return(0);
}







//==============================================================================
// top level functions
//==============================================================================

//------------------------------------------------------------------------------
// Helper function to fill in the TWM session for multiple records.
// User will fill in 'info' session with one record parameters
// and the function will assign all records the same. It will also
// generate names of current and previous records, so when the 
// session is saved, it contains valid references to all previous records.
//
// Parameters:
//  rec_id - current record ID (zero based)
//  *info - TWM session pointer with assigned elements:
//           rec_count, chn_count, chn_gains[0][], chn_offs[0][]
//  *filePath - pointer to buffer with absolute path to record file name
//              it will be expanded by record index and stored back
//              so keep some reserve in size
int twm_gen_session_rec_names(TTWMssnInf *info, char *filePath, int rec_id)
{
	if(info->rec_count > TWMMAXREC)
		return(1);

	// remove '.ext' from full file path
	char pathbase[MAX_PATH];
	strcpy(pathbase, filePath);
	char *pstr = strrchr(pathbase, '.');
	if(pstr)
		*pstr = '\0';
	
	// extract measurement folder and create relative record path
	char meas_fld[MAX_PATH];
	char rel_path[MAX_PATH];
	strcpy(meas_fld, pathbase);
	pstr = strrchr(meas_fld, '\\');
	if(pstr)
		*pstr++ = '\0';
	else
		return(1); // this should not happen...
	strncpy(rel_path, pstr, MAX_PATH); // record relative path (basically just file name base)
	
	
	// build list of all record to this moment {1, 2, ... (rec_id-1)}
	//  note: this is needed so the TWM session which will be rewritten for each record contains all past records!
	for(int k = 0;k <= rec_id;k++)
	{
		// build record name 'name_kkkk.txt'
		strcpy(info->chn_data_paths[k], rel_path);
		char seq_str[16];
		sprintf(seq_str, "_%04d.txt", k+1);
		strcat(info->chn_data_paths[k], seq_str);
	}
	
	// copy last record full path (this record) to global filePath
	sprintf(filePath, "%s\\%s", meas_fld, info->chn_data_paths[rec_id]);
	
	
	// duplicate gains to all records
	for(int r = 1;r <= rec_id;r++)
	{
		// for each record:
		
		for(int k = 0;k < info->chn_count;k++)
		{
			// for each channel:
			
			info->chn_gains[r][k] = info->chn_gains[0][k];
			info->chn_offs[r][k] = info->chn_offs[0][k];
		}
	}
	
	return(0);

}
