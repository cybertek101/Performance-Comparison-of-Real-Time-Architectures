#include "process.h"

/*//NODE STATUS
#define IDLE 0
#define BUSY 1

// QUEUE TYPES
#define FIFO 0
#define PRIORITY 1
*/

#define ALPHA 5.0 //used in deadline calculation

void process::initialize(float exec_time,int beta) //should not be in constructor... max_pid can be affected
 {
	p_id=max_pid++;
	set_arvl_time();
 	set_execution_time(exec_time);
 	set_deadline(beta);// should be called after set_execution_time(---)
 }
 
void  process::set_arvl_time()
 {
 	arvl_time=current_time;
 }
 
void process::set_execution_time(float exec_time)
{
	execution_time=exec_time;
}

void process::set_deadline(int beta)
{
	deadline=current_time + ALPHA*execution_time+beta;
}

float process::get_arvl_time()
{
	return arvl_time;
}

float process::get_execution_time()
{
	return execution_time;
}

float process::get_deadline()
{
	return deadline;
}

int process::get_p_id()
{
	return p_id;
}

bool process::operator<(const process& p)
{
	/*if (Q_policy == FIFO) //arrival time
	{
		return (this->arvl_time < p.arvl_time );
	}else{//least laxity first; laxity=maxint-(deadline-execution_time)*/
	return (this->deadline - this->execution_time - current_time <  p.deadline - p.execution_time - current_time);
	//}	
}

