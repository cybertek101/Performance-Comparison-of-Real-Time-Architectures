//#ifndef NODE_H
//#define NODE_H
#include <math.h>
#include <iostream>
#include <list>
#include "struct.h"


using namespace std;

class node
{
	private:
		int node_id;
		int node_status;
		int generated;
		int self_grntd;
		int mig_grntd;
		int completion;
		int deadlinehit;
		float busy_time;
		//seed for random number generator... should be initialized in constructor using gettimeofday();
		unsigned int long seed_arvl;
		unsigned int long seed_exec;
		unsigned int long seed_ddln;
		
		float next_arvl_time;
		float next_dept_time;
		
		list<process> grntd_list;
		list<message> rqst_list;			
		list<bidNode> mig_list; 		
		
	public:
	
		 node();
		int generateSeed();		
		
		void update_generated();
		void update_self_grntd();
		void update_mig_grntd();		 
		void update_completion();
		void update_deadlinehit();
		void update_busy_time(float);
		void update_node_status(int);
		
		void update_next_arvl_time();
		void update_next_dept_time();
		
		int get_generated();
		int get_self_grntd();
		int get_mig_grntd();
		int get_completion();
		int get_deadlinehit();
		float get_busy_time();
		int get_node_status();
		
		float get_next_arvl_time();
		float get_next_dept_time();		
		float get_next_mig_time();
		float get_next_reqst_time();
		//===========================
 		void push_grntd_list(process);
		void push_rqst_list(message);
		void push_mig_list(bidNode);
		
		process pop_grntd_list();
		message pop_rqst_list();
		bidNode pop_mig_list();		
		
		//====================================
		float get_total_execution_time();
		float get_total_execution_time(process);
		int grntd_list_isEmpty();
		//---------------------------------
		float urand(unsigned long int& );
		float exprand(float,unsigned long int& );
		//------------------------------------------
		float get_process_exec_time();
		int get_deadline_offset();
		//===========================================	
};

//#endif /* NODE_H */

