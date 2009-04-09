/********************************************************************\
  Name:         scbertan.c
  Created by:   Pierre
  $Id$

\********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "midas.h"
#include "mcstd.h"
#include "mscb.h"
#include "experim.h"

#define MAX_HVB_CHANNELS    1

/* make frontend functions callable from the C framework */
#ifdef __cplusplus
extern "C" {
#endif

/*-- Globals -------------------------------------------------------*/

/* The frontend name (client name) as seen by other MIDAS clients   */
char *frontend_name = "scBertan";
/* The frontend file name, don't change it */
char *frontend_file_name = __FILE__;

/* frontend_loop is called periodically if this variable is TRUE    */
BOOL frontend_call_loop = TRUE;

/* a frontend status page is displayed with this frequency in ms */
INT display_period = 0000;

/* maximum event size produced by this frontend */
INT max_event_size = 10000;

/* maximum event size for fragmented events (EQ_FRAGMENTED) */
INT max_event_size_frag = 5 * 1024 * 1024;

/* buffer size to hold events */
INT event_buffer_size = 100 * 10000;

typedef struct {
   char mscb_device[NAME_LENGTH];
   char pwd[NAME_LENGTH];
   int  base_address;
} LMSCB_SETTINGS;
LMSCB_SETTINGS lmscb;
int lmscb_fd;

#define LMSCB_SETTINGS_STR "\
MSCB Device = STRING : [32] mscb503\n\
MSCB Pwd = STRING : [32] \n\
Base Address = INT : 1\n\
"

typedef struct {
  HNDLE hhvbS;
  HV_BERTAN_SETTINGS *hvbset;
} HVB_STRUCT;//

INT gdebug = 1;
HNDLE hSet;
HVB_STRUCT *phvbs[MAX_HVB_CHANNELS];
HV_BERTAN_SETTINGS lhvbset[MAX_HVB_CHANNELS];

/*-- Function declarations -----------------------------------------*/
INT frontend_init();
INT frontend_exit();
INT begin_of_run(INT run_number, char *error);
INT end_of_run(INT run_number, char *error);
INT pause_run(INT run_number, char *error);
INT resume_run(INT run_number, char *error);
INT frontend_loop();

INT  read_trigger_event(char *pevent, INT off);
INT  read_scaler_event(char *pevent, INT off);
INT  read_mscb_event(char *pevent, INT off);
INT  localmscb_init(char *eqname);
void register_cnaf_callback(int debug);
void hvb_callback(INT hDB, INT hKey, void *info);
INT  hvbchannel_init(char *eqname, int nchannel);

/*-- Equipment list ------------------------------------------------*/

EQUIPMENT equipment[] = {
   {"HV_BERTAN",             /* equipment name */
    {3, 0,                   /* event ID, trigger mask */
     "SYSTEM",               /* event buffer */
     EQ_PERIODIC,            /* equipment type */
     0,                      /* event source */
     "MIDAS",                /* format */
     TRUE,                   /* enabled */
     RO_ALWAYS | RO_TRANSITIONS |   /* read when running and on transitions */
     RO_ODB,                 /* and update ODB */
     10000,                  /* read every 10 sec */
     0,                      /* stop run after this event limit */
     0,                      /* number of sub events */
     1,                      /* log history */
     "", "", "",},
    read_mscb_event,       /* readout routine */
    },

   {""}
};

#ifdef __cplusplus
}
#endif

/********************************************************************\
              Callback routines for system transitions

  These routines are called whenever a system transition like start/
  stop of a run occnurs. The routines are called on the following
  occations:

  frontend_init:  When the frontend program is started. This routine
                  should initialize the hardware.

  frontend_exit:  When the frontend program is shut down. Can be used
                  to releas any locked resources like memory, commu-
                  nications ports etc.

  begin_of_run:   When a new run is started. Clear scalers, open
                  rungates, etc.

  end_of_run:     Called on a request to stop a run. Can send
                  end-of-run event and close run gates.

  pause_run:      When a run is paused. Should disable trigger events.

  resume_run:     When a run is resumed. Should enable trigger events.
\********************************************************************/

/*-- Frontend Init -------------------------------------------------*/
INT frontend_init()
{
  int i, status;
  char str[32];
  HNDLE hDB;

  cm_get_experiment_database(&hDB, NULL);
  status = hvbchannel_init("HV_BERTAN", 4);
  printf("mscb and hvbchannel_init status:%d\n", status);
  
  /* hardware initialization */
  /* print message and return FE_ERR_HW if frontend should not be started */
  status = localmscb_init("HV_BERTAN");
  if (status != FE_SUCCESS) {
    for (i=0 ; i<MAX_HVB_CHANNELS ; i++) {
      sprintf(str, "Hardware ch.%d not running", i);
      db_set_value(hDB, phvbs[i]->hhvbS, "Warning", str, sizeof(str), 1, TID_STRING);
    }
    cm_msg(MERROR,"scbertan","Access to mscb failed [%d]", status);
    return status;
  }
  return SUCCESS;
}

/*-- Frontend Exit -------------------------------------------------*/
INT frontend_exit()
{
  int i;

  for (i=0 ; i<MAX_HVB_CHANNELS ; i++) {
    if (phvbs[i])
      free(phvbs[i]);
    phvbs[i] = NULL;
  }
  return SUCCESS;
}

/*-- Begin of Run --------------------------------------------------*/
INT begin_of_run(INT run_number, char *error)
{
  return SUCCESS;
}

/*-- End of Run ----------------------------------------------------*/
INT end_of_run(INT run_number, char *error)
{
   return SUCCESS;
}

/*-- Pause Run -----------------------------------------------------*/
INT pause_run(INT run_number, char *error)
{
   return SUCCESS;
}

/*-- Resume Run ----------------------------------------------------*/
INT resume_run(INT run_number, char *error)
{
   return SUCCESS;
}

/*-- Frontend Loop -------------------------------------------------*/
INT frontend_loop()
{
   /* if frontend_call_loop is true, this routine gets called when
      the frontend is idle or once between every event */
  ss_sleep(100);
   return SUCCESS;
}

/*------------------------------------------------------------------*/
/********************************************************************\

  Readout routines for different events

\********************************************************************/
/*-- Trigger event routines ----------------------------------------*/
INT poll_event(INT source, INT count, BOOL test)
/* Polling routine for events. Returns TRUE if event
   is available. If test equals TRUE, don't return. The test
   flag is used to time the polling */
{
   int i;
   DWORD lam;

   for (i = 0; i < count; i++) {
     lam = 1;
      if (lam & LAM_SOURCE_STATION(source))
         if (!test)
            return lam;
   }
   return 0;
}

/*-- Interrupt configuration ---------------------------------------*/
INT interrupt_configure(INT cmd, INT source, POINTER_T adr)
{
   switch (cmd) {
   case CMD_INTERRUPT_ENABLE:
      break;
   case CMD_INTERRUPT_DISABLE:
      break;
   case CMD_INTERRUPT_ATTACH:
      break;
   case CMD_INTERRUPT_DETACH:
      break;
   }
   return SUCCESS;
}

/*-- MSCB event --------------------------------------------------*/
INT read_mscb_event(char *pevent, INT off)
{
  int i, status, size;
  float *pdata;
  HNDLE hDB;
  char str[32];

  cm_get_experiment_database(&hDB, NULL);

  bk_init(pevent);
  bk_create(pevent, "HTPC", TID_FLOAT, &pdata);
  /* Find HVB channel */
  for (i= 0 ; i<MAX_HVB_CHANNELS; i++) {
    if (phvbs[i]) {
      size = sizeof(str);
      status = mscb_read(lmscb_fd, lmscb.base_address+i, 8, str, &size);
      db_set_value(hDB, phvbs[i]->hhvbS, "Warning", str, sizeof(str), 1, TID_STRING);
      if (gdebug) printf("Warning str:%s\n", str);
      size = sizeof(float);
      status = mscb_read(lmscb_fd, lmscb.base_address+i, 2, pdata, &size);
      if (gdebug) printf("Umeasure:%f - %f\n", *pdata, lhvbset[i].umeasure);
      pdata++;
      status = mscb_read(lmscb_fd, lmscb.base_address+i, 3, pdata, &size);
      if (gdebug) printf("Umeasure:%f - %f\n", *pdata, lhvbset[i].umeasure);
      pdata++;
    }
  }
  bk_close(pevent, pdata);
  return bk_size(pevent);
}

/*-- Local MSCB event --------------------------------------------------*/
INT localmscb_init(char *eqname)
{
  int  status, size;
  HNDLE hDB, hDD;
  MSCB_INFO node_info;
  char set_str[80];
  /* Book Setting space */
  
  cm_get_experiment_database(&hDB, NULL);

  /* Map /equipment/Trigger/settings for the sequencer */
  sprintf(set_str, "/Equipment/%s/Settings", eqname);
  status = db_create_key(hDB, 0, set_str, TID_KEY);
  status = db_find_key (hDB, 0, set_str, &hSet);
  if (status != DB_SUCCESS)
    cm_msg(MINFO,"FE","Key %s not found", set_str);

  /* create MSCB settings record */
  status = db_find_key (hDB, 0, set_str, &hSet);
  status = db_create_record(hDB, hSet, "DD", LMSCB_SETTINGS_STR);
  if (status != DB_SUCCESS)
    return FE_ERR_ODB;

  db_find_key(hDB, hSet, "DD", &hDD);
  size = sizeof(lmscb);
  db_get_record(hDB, hDD, &lmscb, &size, 0);

  /* open device on MSCB */
  lmscb_fd = mscb_init(lmscb.mscb_device, NAME_LENGTH, lmscb.pwd, FALSE);
  if (lmscb_fd < 0) {
    cm_msg(MERROR, "mscb_init",
      "Cannot access MSCB submaster at \"%s\". Check power and connection.",
      lmscb.mscb_device);
    return FE_ERR_HW;
  }
  
  /* check if FGD devices are alive */
  mscb_ping(lmscb_fd, lmscb.base_address, 0);
  if (status != FE_SUCCESS) {
    cm_msg(MERROR, "mscb_init",
	   "Cannot ping MSCB address 0. Check power and connection.");
    return FE_ERR_HW;
  }
  
  mscb_info(lmscb_fd, lmscb.base_address, &node_info);
  if (strncmp(node_info.node_name, "TPC_HV",6) != 0) {
    cm_msg(MERROR, "mscb_init",
	   "Found one expected node \"%s\" at address \"%d\".", node_info.node_name, lmscb.base_address);
    return FE_ERR_HW;
  }
  
  return FE_SUCCESS;
}

/*-- HVB channel --------------------------------------------------*/
void hvb_DevSet(INT hDB, INT hKey, void *info)
{
  int i, size, status;
  
  /* Find HVB channel */
  for (i=0; i<MAX_HVB_CHANNELS; i++) {
    if (phvbs[i] == 0)
      continue;
    if (phvbs[i]->hhvbS == hKey) {
      if (gdebug) printf("DevSet on HVB_%i\n", i);
      
      // Read device setting
      size = sizeof(float);
      status = mscb_read(lmscb_fd, lmscb.base_address+i, 1, &(phvbs[i]->hvbset->udemand), &size);
      // Set localstructure
      lhvbset[i].udemand = phvbs[i]->hvbset->udemand;
      if (gdebug) printf("Udemand:%f\n", lhvbset[i].udemand);

      size = sizeof(float);
      status = mscb_read(lmscb_fd, lmscb.base_address+i, 2, &(phvbs[i]->hvbset->umeasure), &size);
      // Set localstructure
      lhvbset[i].umeasure = phvbs[i]->hvbset->umeasure;
      if (gdebug) printf("Umeasure:%f\n", lhvbset[i].umeasure);

      size = sizeof(float);
      status = mscb_read(lmscb_fd, lmscb.base_address+i, 3, &(phvbs[i]->hvbset->imeasure), &size);
      // Set localstructure
      lhvbset[i].imeasure = phvbs[i]->hvbset->imeasure;
      if (gdebug) printf("Imeasure:%f\n", lhvbset[i].imeasure);


      size = sizeof(WORD);
      status = mscb_read(lmscb_fd, lmscb.base_address+i, 4, &(phvbs[i]->hvbset->rampup), &size);
      // Set localstructure
      lhvbset[i].rampup = phvbs[i]->hvbset->rampup;
      if (gdebug) printf("Rampup:%d\n", lhvbset[i].rampup);


      size = sizeof(WORD);
      status = mscb_read(lmscb_fd, lmscb.base_address+i, 5, &(phvbs[i]->hvbset->rampdown), &size);
      // Set localstructure
      lhvbset[i].rampdown = phvbs[i]->hvbset->rampdown;
      if (gdebug) printf("Rampdown:%d\n", lhvbset[i].rampdown);

      size = sizeof(float);
      status = mscb_read(lmscb_fd, lmscb.base_address+i, 6, &(phvbs[i]->hvbset->ulimit), &size);
      // Set localstructure
      lhvbset[i].ulimit = phvbs[i]->hvbset->ulimit;
      if (gdebug) printf("Ulimit:%f\n", lhvbset[i].ulimit);


      size = sizeof(float);
      status = mscb_read(lmscb_fd, lmscb.base_address+i, 7, &(phvbs[i]->hvbset->ilimit), &size);
      // Set localstructure
      lhvbset[i].ilimit = phvbs[i]->hvbset->ilimit;
      if (gdebug) printf("Ilimit:%f\n", lhvbset[i].ilimit);


      size = sizeof(phvbs[i]->hvbset->warning);
      status = mscb_read(lmscb_fd, lmscb.base_address+i, 8, phvbs[i]->hvbset->warning, &size);
      // Set localstructure
      sprintf(lhvbset[i].warning, phvbs[i]->hvbset->warning);
      if (gdebug) printf("Warning:%s\n", lhvbset[i].warning);
    }
  }
}
 
/*-- HVB channel --------------------------------------------------*/
void hvb_callback(INT hDB, INT hKey, void *info)
{
  int i, size, status;
  
  /* Find HVB channel */
  for (i=0; i<MAX_HVB_CHANNELS; i++) {
    if (phvbs[i] == 0) continue;
    if (phvbs[i]->hhvbS == hKey) {
      db_close_record(hDB, hKey);
      printf("callback on HVB_%i\n", i);
      // Determine what changed
      if (phvbs[i]->hvbset->udemand != lhvbset[i].udemand) {
	size = sizeof(float);
	status = mscb_write(lmscb_fd, lmscb.base_address+i, 1, &(phvbs[i]->hvbset->udemand), size);
        lhvbset[i].udemand = phvbs[i]->hvbset->udemand;
	if (gdebug) printf("Set Udemand:%f\n", lhvbset[i].udemand);
      }
      
      if (phvbs[i]->hvbset->rampup != lhvbset[i].rampup) {
        size = sizeof(WORD);
        status = mscb_write(lmscb_fd, lmscb.base_address+i, 4, &(phvbs[i]->hvbset->rampup), size);
        lhvbset[i].rampup = phvbs[i]->hvbset->rampup;
	if (gdebug) printf("Set Rampup:%d\n", lhvbset[i].rampup);
      }

      if (phvbs[i]->hvbset->rampdown != lhvbset[i].rampdown) {
        size = sizeof(WORD);
        status = mscb_write(lmscb_fd, lmscb.base_address+i, 5, &(phvbs[i]->hvbset->rampdown), size);
        lhvbset[i].rampdown = phvbs[i]->hvbset->rampdown;
	if (gdebug) printf("Set Rampdown:%d\n", lhvbset[i].rampdown);
      }

      if (phvbs[i]->hvbset->ulimit != lhvbset[i].ulimit) {
        size = sizeof(float);
        status = mscb_write(lmscb_fd, lmscb.base_address+i, 6, &(phvbs[i]->hvbset->ulimit), size);
        lhvbset[i].ulimit = phvbs[i]->hvbset->ulimit;
	if (gdebug) printf("Set Ulimit:%f\n", lhvbset[i].ulimit);
      }

      if (phvbs[i]->hvbset->ilimit != lhvbset[i].ilimit) {
        size = sizeof(float);
        status = mscb_write(lmscb_fd, lmscb.base_address+i, 7, &(phvbs[i]->hvbset->ilimit), size);
        lhvbset[i].ilimit = phvbs[i]->hvbset->ilimit;
	if (gdebug) printf("Set Ilimit:%f\n", lhvbset[i].ilimit);
      }

      size = sizeof(HV_BERTAN_SETTINGS);
      status = db_open_record(hDB, phvbs[i]->hhvbS, phvbs[i]->hvbset, size, MODE_READ, hvb_callback, NULL);
      if (status != DB_SUCCESS) {
	cm_msg(MERROR,"hvbchannel"," cannot open re-record");
      }
    }
  }
}


/*-- HVB channel --------------------------------------------------*/
INT hvbchannel_init(char *eqname, int nchannel)
{
  int  status, size, i;
  HNDLE hDB, hHVBS;
  char hvb_str[80], info[32];
  HV_BERTAN_SETTINGS_STR(hv_bertan_settings_str); 
  cm_get_experiment_database(&hDB, NULL);

  /* Map /equipment/Trigger/settings */
  sprintf(hvb_str, "/Equipment/%s/Settings", eqname);
  status = db_create_key(hDB, 0, hvb_str, TID_KEY);
  status = db_find_key (hDB, 0, hvb_str, &hSet);
  if (status != DB_SUCCESS)
    cm_msg(MINFO,"FE","Key %s not found", hvb_str);

  for (i=0 ; i< nchannel ; i++) {
    sprintf(hvb_str, "HVB_%1i", i);
    status = db_find_key(hDB, hSet, hvb_str, &hHVBS);
    if (status != DB_SUCCESS) {
      /* create HVB channel settings record */
      status = db_create_record(hDB, hSet, hvb_str, strcomb(hv_bertan_settings_str));
      if (status != DB_SUCCESS)  return FE_ERR_ODB;      // Get handle to HVB setting
      db_find_key(hDB, hSet, hvb_str, &hHVBS);
    }
    size = sizeof(HVB_STRUCT);
    phvbs[i] = (HVB_STRUCT *) malloc(size);
    size = sizeof(HV_BERTAN_SETTINGS);
    phvbs[i]->hvbset = (HV_BERTAN_SETTINGS *) malloc(size);
    phvbs[i]->hhvbS = hHVBS;
    status = db_open_record(hDB, phvbs[i]->hhvbS, phvbs[i]->hvbset, size, MODE_READ, hvb_callback, NULL);
    if (status != DB_SUCCESS) {
      cm_msg(MERROR,"hvbchannel"," cannot open record on %s", hvb_str);
    }
  
    // Get initial record from ODB
    size = sizeof(HV_BERTAN_SETTINGS);
    db_get_record(hDB, phvbs[i]->hhvbS, phvbs[i]->hvbset, &size, 0);
    // Read Device setting
    hvb_DevSet(hDB, phvbs[i]->hhvbS, info);
  }

  return FE_SUCCESS;
}

