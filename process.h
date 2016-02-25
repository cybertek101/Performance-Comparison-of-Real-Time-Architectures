#include "global.h"
#ifndef PROCESS_H
#define PROCESS_H
/*
//-------------------------------------------global variables
extern float current_time;
extern int Q_policy;//fifo or priority
extern int num_nodes;
extern int max_pid;
//--------------------------------------------
*/

class process
 {
 	private:
 		int p_id;
 		float arvl_time;
 		float execution_time;
 		float deadline;
 	public:
 		void initialize(float,int);
 	 		
 		void set_arvl_time();
 		void set_execution_time(float);
 		void set_deadline(int);
 
  		float get_arvl_time();
 		float get_execution_time();
 		float get_deadline();
 		int get_p_id();
 		
 		//===========================
 		//bool operator==(process); necessary
 		bool operator<(const process&);	
};

#endif /* PROCESS_H */
