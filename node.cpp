#include <sys/time.h>
#include "node.h"

node::node()
{
	node_id=max_nid++;	
	node_status=IDLE;
	generated=0;
	self_grntd=0;
	mig_grntd=0;
	deadlinehit=0;
	completion=0;
	busy_time=0.0;

	
	seed_arvl=generateSeed();
	seed_exec=generateSeed();
	seed_ddln=generateSeed();
	
	next_arvl_time=current_time+exprand(mean_interarvl_time,seed_arvl);//current_time=0.0
	cout<<"nid: "<<node_id<<" arvl: "<<next_arvl_time<<endl;
	next_dept_time=INFINITY;
}
//================================
int node::generateSeed()
{
	struct timeval tmp;
	gettimeofday(&tmp, NULL); 
  	return tmp.tv_usec;;
}
//===================================
void node::update_generated()
{
	generated++;
}

void node::update_self_grntd()
{
	self_grntd++;
}
void node::update_mig_grntd()
{
	mig_grntd++;	
}
void node::update_completion()
{
	completion++;
}
void node::update_deadlinehit()
{
	deadlinehit++;
}
void node::update_busy_time(float exec_time)
{
	busy_time+=exec_time;
}
void node::update_node_status(int status)
{
	node_status=status;
}
void node::update_next_arvl_time()
{
	next_arvl_time=current_time+exprand(mean_interarvl_time,seed_arvl);
}
void node::update_next_dept_time()//float next_dept) 
{
	//sort according to priority use FIFO and PRIORITY to check priority type in the operator definition
	
	if(!grntd_list.empty())
	{
		if (Q_policy == 1)//PRIORITY)
		{
			grntd_list.sort();
		}
		next_dept_time=current_time + grntd_list.front().get_execution_time();
	}
	else
	{
		next_dept_time=INFINITY;
	}
}
//==================================================

int node::get_generated()
{
	return generated;
}

int node::get_self_grntd()
{
	return self_grntd;
}

int node::get_mig_grntd()
{
	return mig_grntd;
}
int node::get_completion()
{
	return completion;
}
int node::get_deadlinehit()
{
	return deadlinehit;
}
float node::get_busy_time()
{
	return busy_time;
}

int node::get_node_status()
{
	return node_status;
}


float node::get_next_arvl_time()
{
	return next_arvl_time;
}
float node::get_next_dept_time()
{
	return next_dept_time;
}

float node::get_next_mig_time()
{
	//chk if list is not empty otherwise return infinity
	if(!mig_list.empty())
	{
		return mig_list.front().q_time;
	}
	else
	{
		return INFINITY;
	}
}

float node::get_next_reqst_time()
{
	if(!rqst_list.empty())
	{
		return rqst_list.front().q_time;
	}
	else
	{
		return INFINITY;
	}
}
//===========================================

void node::push_grntd_list(process prcs)
{
	grntd_list.push_back(prcs);
}

void node::push_rqst_list(message msg)
{
	rqst_list.push_back(msg);
}
void node::push_mig_list(bidNode bn)
{
	mig_list.push_back(bn);
}
//======================================================
process node::pop_grntd_list()
{
	process tmp=grntd_list.front();
    grntd_list.pop_front();
    return tmp;
}

message node::pop_rqst_list()
{
	message tmp=rqst_list.front();
    rqst_list.pop_front();
    return tmp;
}
bidNode node::pop_mig_list()
{
	//if(!mig_list.empty())// not really needed
	//{
		bidNode tmp=mig_list.front();
    	mig_list.pop_front();
    	return tmp;
    /*}
    else
    {
    	//cout<<"list is empty"<<endl;
    	return -1;
    }*/
}

//============================================
float node::get_total_execution_time()
{
	
	float srv_start_time=get_next_dept_time();
	if(node_status==IDLE)//not really needed
	{
		srv_start_time=current_time;
	}
	list<process>::iterator iter = grntd_list.begin();
	iter++;
	
	for(;iter != grntd_list.end();iter++)
	{
		srv_start_time+=(*iter).get_execution_time();
	}
	return srv_start_time;
}

float node::get_total_execution_time(process p)//this function is called when the server is busy
{
	list<process> proc_list=grntd_list;
	float srv_start_time=get_next_dept_time();//server is busy
	proc_list.pop_front();
	proc_list.push_back(p);
	proc_list.sort();
	int p_id=p.get_p_id();
	
	list<process>::iterator iter;
	
	for (iter=proc_list.begin(); iter != proc_list.end() && p_id != (*iter).get_p_id(); ++iter)
	{
		srv_start_time += (*iter).get_execution_time();
	}
	
	return srv_start_time;
}

int node::grntd_list_isEmpty()
{
	if(grntd_list.empty())
	{
		return 1;
	}
	return 0;
}

//==========================================================================

float node::urand(unsigned long int& seed)//seed argument for arrival excution time and deadline
{

	unsigned long int A = 742938285;
	unsigned long int AHI = (A>>15);
	unsigned long int AL0 = (A&0x7FFF);
	
	unsigned long int xhi,xl0,mid;
	xhi=(seed)>>16;
	xl0=(seed)&0xFFFF;
	mid=AHI*xl0+(AL0<<1)*xhi;
	seed=AHI*xhi+(mid>>16)+AL0*xl0;
	if((seed)&0x80000000)
	{
		(seed)-=0x7FFFFFFF;
	}
	(seed)+=((mid&0xFFFF)<<15);
	if ((seed)&0x80000000)
	{
		(seed)-=0x7FFFFFFF;
	}
	return ((seed)*(1.0/2147483647.0));
}

float node::exprand(float mean,unsigned long int& seed)
{
    float dum;
    do 
    {
		dum = urand(seed); // seed argument
    } while (dum <= 0.0);

   return (-mean)*log(dum); //((-1.0/lambda)*log(dum));
}

//=********************************************************
float node::get_process_exec_time()
{
	return exprand(mean_exec_time,seed_exec);	
}
int node::get_deadline_offset()
{
	int beta=(int)urand(seed_ddln)*299+1;//uniformly distributed between 1 and 300
	return beta;	
}
//=********************************************************

