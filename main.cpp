#include <iostream>
//#include <list>
//#include <cstring>
//#include <math.h>
//#include "struct.h"
//#include "process.h"
#include <sys/time.h>
#include "node.h"
//#include "global.h"

using namespace std;


enum {ARRIVAL=0,DEPARTURE,REQUEST,MIGRATED}; // EVENT TYPES

//global variables
float current_time;
int Q_policy;//fifo or priority
int num_nodes;
int max_pid;
int max_nid;

float msg_trnsfr_delay;
float prcs_trnsfr_delay;

float mean_interarvl_time;
float mean_exec_time;

unsigned int long seed_arvl;
unsigned int long seed_exec;
unsigned int long seed_ddln;

node* nodeptr=NULL;
float lbound=1000;
float ubound=5000.0;
//========================
void initializeSystem();
int* getEarlyEvent();
void do_arrival(int);
void do_departure(int);
void do_request(int);
void do_transfer(int);
float get_max(float, float );
void summarize();

//int generateSeed();
//=======================
int main(int argc, char* argv[])
{
	int* early_event=0;
	int earlyNID,event;
	initializeSystem();
	
	//return 0;
	//boot the system. the next event to happen is an arrival event.
	
	early_event=getEarlyEvent();	
	//free memory	
	earlyNID=early_event[0];
	event=early_event[1];
		
	while( current_time< ubound)	
	{
		//if(current_time>lbound)
		//	cout<<"time: "<<current_time<<"    node: "<<early_event[0]<<"    event: "<<early_event[1];
		switch (event)
		{
			case ARRIVAL:
				do_arrival(earlyNID);// 
				break;
			case DEPARTURE:				
				do_departure(earlyNID);
				break;
			case REQUEST:
				do_request(earlyNID);
				// process the front request in all the nodes and find the best node.Then update this node's mig_list.
				// the same request is at the front position of the rqst queue of all the nodes (since fifo is used)
				break;				 
			case MIGRATED:
				do_transfer(earlyNID);
				break;
			default:
				cout<<"no such event"<<endl;
				exit(1);// free memory
		}
		
		early_event=getEarlyEvent();	
		//free memory	
		earlyNID=early_event[0];
		event=early_event[1];
	}
	
	summarize();
	delete[] nodeptr;
	return 0;
}

//==================================================================
void do_arrival(int earlyNID)
{
	//update generated processes
	if(current_time>lbound)
		nodeptr[earlyNID].update_generated();
	
	// this is the only place random number generators are used.
	//update next arrival time of the node
	nodeptr[earlyNID].update_next_arvl_time();	
	//create a process and initialize its values
	process tmp;
	float exec_time = nodeptr[earlyNID].get_process_exec_time();
	cout<<"  exec: "<<exec_time<<endl;
	int ddl_offset = nodeptr[earlyNID].get_deadline_offset();
	tmp.initialize(exec_time,ddl_offset);
	
	if (nodeptr[earlyNID].get_node_status()==IDLE)
	{
		nodeptr[earlyNID].update_node_status(BUSY);
		
		if(current_time>lbound)
			nodeptr[earlyNID].update_self_grntd();		
		
		nodeptr[earlyNID].push_grntd_list(tmp);
		//int nextdept=current_time+tmp.get_execution_time();
		nodeptr[earlyNID].update_next_dept_time();//nextdept);
	}
	else //check for local guarantee
	{
		// conditional call if fifo
		float srv_start_time;
		if (Q_policy==FIFO)
		{
			srv_start_time=nodeptr[earlyNID].get_total_execution_time();
		}
		else //if priority
		{
			srv_start_time=nodeptr[earlyNID].get_total_execution_time(tmp);
		}
		
		if(srv_start_time +tmp.get_execution_time() <= tmp.get_deadline()) // can Be Guaranteed locally		
		{
			if(current_time>lbound)		
				nodeptr[earlyNID].update_self_grntd();
				
			nodeptr[earlyNID].push_grntd_list(tmp);
		}
		
		else //cannot be guaranteed locally
		{
			// send requests to other nodes		
			message rqst;
			rqst.prcs=tmp;
			rqst.n_id=earlyNID;
			rqst.q_time=current_time + msg_trnsfr_delay;
			for(int i=0;i<num_nodes;i++)
			{
				if(i!=earlyNID)
				{
					nodeptr[i].push_rqst_list(rqst);
				}
			}
		}
	}
}	
//===================================================================
void do_transfer(int earlyNID)
{
	bidNode bn=nodeptr[earlyNID].pop_mig_list();
	process tmp=bn.prcs;
	float q_time=bn.q_time;	
	tmp.set_arvl_time();// treated as an arrival to that node

	if (nodeptr[earlyNID].get_node_status()==IDLE)
	{
		if(current_time>lbound)	
			nodeptr[earlyNID].update_mig_grntd();
			
		nodeptr[earlyNID].update_node_status(BUSY);
		nodeptr[earlyNID].push_grntd_list(tmp);
		nodeptr[earlyNID].update_next_dept_time();//nextdept);
	}
	else //check for local guarantee
	{
		//float srv_start_time=nodeptr[earlyNID].get_total_execution_time();
		// conditional call if fifo
		float srv_start_time;
		if (Q_policy==FIFO)
		{
			srv_start_time=nodeptr[earlyNID].get_total_execution_time();
		}
		else //if priority
		{
			srv_start_time=nodeptr[earlyNID].get_total_execution_time(tmp);
		}
		
		if(srv_start_time +tmp.get_execution_time() <= tmp.get_deadline()) // can Be Guaranteed locally		
		{
			if(current_time>lbound)
				nodeptr[earlyNID].update_mig_grntd();
				
			nodeptr[earlyNID].push_grntd_list(tmp);
		}
		
		else //cannot be guaranteed locally
		{
			// send requests to other nodes		
			message rqst;
			rqst.prcs=tmp;
			rqst.n_id=earlyNID;
			rqst.q_time=current_time + msg_trnsfr_delay;
			for(int i=0;i<num_nodes;i++)
			{
				if(i!=earlyNID)
				{
					nodeptr[i].push_rqst_list(rqst);
				}
			}
		}
	}

}
//==================================================================
void do_departure(int earlyNID)
{
	
	//if(current_time> lbound)
	//	nodeptr[earlyNID].update_completion();//completion
		
	process tmp=nodeptr[earlyNID].pop_grntd_list();
	float exec_time=tmp.get_execution_time();
	float ddln=tmp.get_deadline();
	
	if(current_time - exec_time >= lbound)
	{
		nodeptr[earlyNID].update_busy_time(exec_time);// busy time of the node
		nodeptr[earlyNID].update_completion();//completion
		if( current_time<=ddln)
		{
			nodeptr[earlyNID].update_deadlinehit();
		}
	}
	
	nodeptr[earlyNID].update_next_dept_time();
	
	if(nodeptr[earlyNID].grntd_list_isEmpty())
	{
		nodeptr[earlyNID].update_node_status(IDLE);
	}
}
//======================================================================
void do_request(int earlyNID)
{
	message tmp=nodeptr[earlyNID].pop_rqst_list();
	int n_id =tmp.n_id;//n-id of the source of the request
	process prcs =tmp.prcs;
	float q_time=tmp.q_time;
	float exec_time=prcs.get_execution_time();
	float ddln=prcs.get_deadline();

	//calculate guarantee time for each node and take the best and reply to the requester only from that node		
	int best_node=-1;
	float grntd_value=0.0;
	for(int i=0;i<num_nodes;i++)
	{	float ttl_exec_time;
		if (i!=n_id)
		{
			if (i!=earlyNID)
			{	
				nodeptr[i].pop_rqst_list();// all requests at the front position should be popped
			}
			//call the ff function for all nodes that have the same request	
			//float ttl_exec_time=nodeptr[i].get_total_execution_time();
			if (Q_policy==FIFO)
			{
				ttl_exec_time=nodeptr[i].get_total_execution_time();
			}
			else //if priority
			{
				ttl_exec_time=nodeptr[i].get_total_execution_time(prcs);
			}
	
			float ttl_trnsfr_delay=current_time + msg_trnsfr_delay + prcs_trnsfr_delay;
			
			float srvc_start=get_max(ttl_exec_time,ttl_trnsfr_delay);
			
			float surplus=ddln-(srvc_start+exec_time);
			if(surplus>=grntd_value)
			{
				grntd_value=surplus;
				best_node=i;
			}
		}
	}
	
	if(best_node>=0) // dirty hack
	{
		//push to migrated q
		bidNode bn;
		bn.q_time=current_time + msg_trnsfr_delay + prcs_trnsfr_delay;// to send reply + to transfer task
		bn.prcs=prcs;
		//nb.prcs.setarrivaltime() to the time it arrives at the node
		nodeptr[best_node].push_mig_list(bn);
	}		
}

//===================================================================
void initializeSystem()
{
	current_time=0.0;
	max_pid=0;
	max_nid=0;
	msg_trnsfr_delay=prcs_trnsfr_delay=5;
	mean_interarvl_time=5;
	mean_exec_time=50;
	cout<<"enter 0 for fifo queue and 1 for priority q: ";
	cin>>Q_policy;
	cout<<"enter the number of nodes: ";
	cin>>num_nodes;
	cout<<"here we are"<<endl;	
	
	//seed_arvl=generateSeed();//should be nitialized before node object is created... used in constructor
	//seed_exec=generateSeed();
	//seed_ddln=generateSeed();
	//cout<<"seed arvl: "<<seed_arvl<<endl;
	//cout<<"seed exec: "<<seed_exec<<endl;
	//cout<<"seed ddln: "<<seed_ddln<<endl;
	//create nodes
	
	nodeptr= new node[num_nodes];//  or use a node list
	
}

int* getEarlyEvent()
{
	int* early_event=new int[2]; 
	float val,min=INFINITY;
	for(int i=0;i<num_nodes;i++)
	{
		val=nodeptr[i].get_next_arvl_time();
		if(min>val)
		{
			min=val;
			early_event[0]=i;			
			early_event[1]=ARRIVAL;
		}
		val=nodeptr[i].get_next_dept_time();
		if(min>val)
		{
			min=val;
			early_event[0]=i;
			early_event[1]=DEPARTURE;
		}
		val=nodeptr[i].get_next_mig_time(); //  should be called before request
		if(min>val)
		{
			min=val;
			early_event[0]=i;
			early_event[1]=MIGRATED;
		}
		
		val=nodeptr[i].get_next_reqst_time();
		if(min>val)
		{
			min=val;
			early_event[0]=i;
			early_event[1]=REQUEST;
		}
	}
	
	//pls be careful
	current_time=min;
	return early_event;
}

float get_max(float a, float b)
{
	if (a<b)
		return b;
	return a;	
}

/*int generateSeed()
{
	struct timeval tmp;
	gettimeofday(&tmp, NULL); 
  	return tmp.tv_usec;;
}
*/

void summarize()
{
	float sum_generated,sum_self_grntd,sum_mig_grntd,sum_completion,sum_ddlnhit,sum_busy_time;
	sum_generated=sum_self_grntd=sum_mig_grntd=sum_completion=sum_ddlnhit=sum_busy_time=0;
	cout<<"\n SUMMARY:"<<endl;
	cout<<"=======:"<<endl;
	for (int i=0;i<num_nodes;i++)
	{
		
		sum_generated +=nodeptr[i].get_generated();
		sum_self_grntd +=nodeptr[i].get_self_grntd();
		sum_mig_grntd += nodeptr[i].get_mig_grntd();
		sum_completion += nodeptr[i].get_completion();
		sum_ddlnhit +=nodeptr[i].get_completion();
		sum_busy_time += nodeptr[i].get_busy_time();
	}
		
	
	cout<<"   generated :"<<sum_generated<<endl;		
	cout<<"   self guaranteed :"<<sum_self_grntd<<endl;
	cout<<"   mig guarantted :"<<sum_mig_grntd<<endl;
	cout<<"   completions :"<<sum_completion<<endl;
	cout<<"    ddlhit:"<<sum_ddlnhit<<endl;
		
		
	cout<<"   GR: " << (sum_self_grntd + sum_mig_grntd)*(1.0)/sum_generated<<endl;
	cout<<"   MR: " << (sum_mig_grntd)*(1.0)/(sum_self_grntd + sum_mig_grntd)<<endl;
	cout<<"   HR: " << (sum_ddlnhit)*(1.0)/sum_completion<<endl;
	cout<<"   UTL: " << (sum_busy_time)*(1.0)/((num_nodes)*(ubound-lbound))*100<< " %"<<endl;
	
}

