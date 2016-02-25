#ifndef GLOBAL_H
#define GLOBAL_H

//NODE STATUS
#define IDLE 0
#define BUSY 1

// QUEUE TYPES
#define FIFO 0
#define PRIORITY 1

//-------------------------------------------global variables
extern float current_time;
extern int Q_policy;//fifo or priority
extern int num_nodes;
extern int max_pid;
extern int max_nid;
extern float msg_trnsfr_delay;
extern float prcs_trnsfr_delay;

extern float mean_interarvl_time;
extern float mean_exec_time;
//---------------------------------------------
//extern int generateSeed();
/*
extern unsigned int long seed_arvl;
extern unsigned int long seed_exec;
extern unsigned int long seed_ddln;
*/
#endif /* GLOBAL_H */

