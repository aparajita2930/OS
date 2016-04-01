//============================================================================
// Name        : scheduler_ac5901.cpp
// Author      : Aparajita
// Description : Scheduler
//============================================================================

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <vector>
#include <string.h>
#include <sstream>
#include <algorithm>
#include <stack>
using namespace std;

class Process
{
	private:
		//at - arrival time, tc - computation time, cb - cpu burst, io - IO burst
		//ft - finishing time, tt - turnaround time, it - time in block state, cw - time in cpu waiting (in ready state)


	public:
		int pid, at, tc, cb, io, ft, tt, it, cw;
		int rem_exec_time, rem_cb, rem_io, prev_time, rr_pr_random_cb, random_cb;
		int static_priority, dyn_priority;
		//process characterized by parameters: at, tc, cb, io and to identify each process: pid
		Process(){
			pid = 0;
			at = 0;
			tc = 0;
			cb = 0;
			io = 0;
			rem_exec_time = rem_cb = rem_io = cw = ft = tt= it = rr_pr_random_cb = random_cb = prev_time = dyn_priority = static_priority = 0;
		}
		Process(int p, int arr_time, int time_comp, int cpu_burst, int io_burst)
		{
			pid = p;
			at = arr_time;
			tc = time_comp;
			cb = cpu_burst;
			io = io_burst;
			rem_cb = time_comp;
			rem_exec_time = rem_io = cw = ft = tt= it= rr_pr_random_cb = random_cb = prev_time = dyn_priority = static_priority = 0;
		}
};

class Scheduler
{
	public:
		int scheduler_algo;
		int quantum;
		vector<Process> process_queue;
		vector<Process> priority_queue_0;
		vector<Process> priority_queue_1;
		vector<Process> priority_queue_2;
		vector<Process> priority_queue_3;
		vector<Process> exp_priority_0;
		vector<Process> exp_priority_1;
		vector<Process> exp_priority_2;
		vector<Process> exp_priority_3;
		virtual Process get_next_process() = 0; //get the next process from the run queue
		virtual void add_process(Process) = 0; //add an event in the run queue
		virtual string get_scheduler_name() = 0;
		virtual void add_exp_process(Process) = 0; //maintain expired process queue for priority scheduling
		virtual ~Scheduler() {};
		void create_process_queue(vector<Process> p)
		{
			process_queue = p;
		}
		vector<Process> get_process_queue()
		{
			return process_queue;
		}
		void set_scheduler(int val)
		{
			this->scheduler_algo = val;
		}
		int get_scheduler()
		{
			return this->scheduler_algo;
		}
};

class FCFS: public Scheduler
{
	public:
		FCFS(int val)
		{
			scheduler_algo = val;
		}

		string get_scheduler_name()
		{
			return "FCFS";
		}

		Process get_next_process()
		{
			Process pr;
			if (!process_queue.empty())
			{
				pr = process_queue.front();
				process_queue.erase(process_queue.begin());
			}
			return pr;
		}

		void add_process(Process p)
		{
			process_queue.push_back(p);
		}

		virtual void add_exp_process(Process p)
		{

		}
};

class LCFS: public Scheduler
{
	public:
		LCFS(int val)
		{
			scheduler_algo = val;
		}

		string get_scheduler_name()
		{
			return "LCFS";
		}

		Process get_next_process()
		{
			Process pr;
			if (!process_queue.empty())
			{
				pr = process_queue.back();
				process_queue.pop_back();
			}
			return pr;
		}

		void add_process(Process p)
		{
			process_queue.push_back(p);
		}

		virtual void add_exp_process(Process p)
		{

		}
};

class SJF: public Scheduler
{
	public:
		SJF(int val)
		{
			scheduler_algo = val;
		}

		string get_scheduler_name()
		{
			return "SJF";
		}

		Process get_next_process()
		{
			Process pr;
			if (!process_queue.empty())
			{
				int min = process_queue[0].rem_cb;
				int current_process = 0;
				for (unsigned int i = 1; i < process_queue.size(); i++)
				{
					if (process_queue[i].rem_cb < min)
					{
						min = process_queue[i].rem_cb;
						current_process = i;
					}
				}
				pr = process_queue[current_process];
				process_queue.erase(process_queue.begin() + current_process);
			}
			return pr;
		}

		void add_process(Process p)
		{
			process_queue.push_back(p);
		}

		virtual void add_exp_process(Process p)
		{

		}
};

class RR: public Scheduler
{
	public:
		RR(int val, int q)
		{
			scheduler_algo = val;
			quantum = q;
		}

		string get_scheduler_name()
		{
			return "RR";
		}

		Process get_next_process()
		{
			Process pr;
			if (!process_queue.empty())
			{
				pr = process_queue.front();
				process_queue.erase(process_queue.begin());
			}
			return pr;
		}

		void add_process(Process p)
		{
			process_queue.push_back(p);
		}

		virtual void add_exp_process(Process p)
		{

		}
};

class PRIO: public Scheduler
{
public:
	PRIO(int val, int q)
	{
		scheduler_algo = val;
		quantum = q;
	}

	string get_scheduler_name()
	{
		return "PRIO";
	}

	Process get_next_process()
	{
		Process pr;

		if (priority_queue_0.empty() && priority_queue_1.empty() && priority_queue_2.empty() && priority_queue_3.empty())
		{
			//exchange the active queues with the expired queues
			priority_queue_0.swap(exp_priority_0);
			priority_queue_1.swap(exp_priority_1);
			priority_queue_2.swap(exp_priority_2);
			priority_queue_3.swap(exp_priority_3);
		}
		if (!priority_queue_3.empty())
		{
			pr = priority_queue_3.front();
			priority_queue_3.erase(priority_queue_3.begin());
		}
		else if (!priority_queue_2.empty())
		{
			pr = priority_queue_2.front();
			priority_queue_2.erase(priority_queue_2.begin());
		}
		else if (!priority_queue_1.empty())
		{
			pr = priority_queue_1.front();
			priority_queue_1.erase(priority_queue_1.begin());
		}
		else if (!priority_queue_0.empty())
		{
			pr = priority_queue_0.front();
			priority_queue_0.erase(priority_queue_0.begin());
		}
		return pr;
	}

	void add_process(Process p)
	{
		if (p.dyn_priority == 0)
		{
			priority_queue_0.push_back(p);
		}
		else if (p.dyn_priority == 1)
		{
			priority_queue_1.push_back(p);
		}
		else if (p.dyn_priority == 2)
		{
			priority_queue_2.push_back(p);
		}
		else if (p.dyn_priority == 3)
		{
			priority_queue_3.push_back(p);
		}
	}

	void add_exp_process(Process p)
	{
		if (p.dyn_priority == 0)
		{
			exp_priority_0.push_back(p);
		}
		else if (p.dyn_priority == 1)
		{
			exp_priority_1.push_back(p);
		}
		else if (p.dyn_priority == 2)
		{
			exp_priority_2.push_back(p);
		}
		else if (p.dyn_priority == 3)
		{
			exp_priority_3.push_back(p);
		}
	}
};

class Event
{
	public:
		int pid;
		int create_timestamp;
		int schedule_timestamp;
		int transition_id;
		Event(int p, int ct, int st, int tid)
		{
			pid = p;
			create_timestamp = ct;
			schedule_timestamp = st;
			transition_id = tid;
		}
};

class IOBlock //required to calculate IO time
{
	public:
		int start;
		int finish;
		IOBlock(){
			this->start = 0;
			this->finish = 0;
		}
		IOBlock(int s, int f)
		{
			this->start = s;
			this->finish = f;
		}
};
bool operator<(const IOBlock &iob1, const IOBlock &iob2){
	return iob1.start == iob2.start ? iob1.finish < iob2.finish : iob1.start < iob2.start;
}

bool file_received = true;
bool set_infile = true, set_rfile = true;
int rrnum, pnum;
int count_random;
vector<int> random_number;
int offset = 0;
vector<Process> read_process_queue; //queue to hold the processes read from the file
vector<IOBlock> ioblock;

class DES_interface
{
	public:
		bool verbose;
		int scheduling_algo;
		int round_robin_num;
		int prio_num;
		vector<Event> event_queue;

		DES_interface()
		{
			verbose = false;
			scheduling_algo = 0;
			round_robin_num = 0;
			prio_num = 0;
		}

		void set_verbose(bool val)
		{
			this->verbose = val;
		}

		void set_scheduling_algo(int val)
		{
			this->scheduling_algo = val;
		}

		void set_round_robin_num(int val)
		{
			this->round_robin_num = val;
		}

		void set_prio_num(int val)
		{
			this->prio_num = val;
		}

		Event get_event()
		{
			Event e(0, 0, 0, 1);
			if (!event_queue.empty())
			{
				e = event_queue.front();
				event_queue.erase(event_queue.begin());
			}
			return e;
		}

		void put_event(Event e)
		{
			int pos = event_queue.size();
			for (unsigned int i = 0; i < event_queue.size(); i++)
			{
				if (e.schedule_timestamp < event_queue[i].schedule_timestamp)
				{
					pos = i;
					break;
				}
			}
			event_queue.insert(event_queue.begin() + pos, e);
		}

		Scheduler* select_scheduling_algo()
		{
			vector<Process> pr_queue = read_process_queue;
			switch(scheduling_algo)
			{
				case 0: return new FCFS(0);
						break;
				case 1: return new LCFS(1);
						break;
				case 2: return new SJF(2);
						break;
				case 3: return new RR(3, round_robin_num);
						break;
				case 4: return new PRIO(4, prio_num);
						break;
				default: return NULL;
			}
			return 0;
		}

		bool compare(const IOBlock& iob1,const IOBlock& iob2)
		{
			return iob1.start == iob2.start ? iob1.finish < iob2.finish : iob1.start < iob2.start;
		}

		void simulation()
		{
			Scheduler* scheduling;
			Event current_event(0, 0, 0, 1);
			int event_id = current_event.pid;
			int rem_io_burst = 0;
			int next_run_time = 0;
			int fin;
			int reset_dyn_priority = -1;
			//Summary variables
			int ft_last_event = 0;
			double cpu_util = 0.0;
			double io_util = 0.0;
			double turnaround = 0.0;
			double cpu_wait = 0.0;
			double throughput = 0.0;

			scheduling = select_scheduling_algo();

			for (unsigned int i = 0; i < read_process_queue.size(); i++)
			{
				put_event(Event(read_process_queue[i].pid, read_process_queue[i].at, read_process_queue[i].at, 1));
				read_process_queue[i].static_priority = 1 + (random_number[offset] % 4);
				offset++;
				read_process_queue[i].dyn_priority = read_process_queue[i].static_priority-1;
			}

			int current_sim_time = read_process_queue[0].at;

			while (!event_queue.empty())
			{
				current_event = get_event();
				if (current_sim_time <= current_event.schedule_timestamp)
				{
					current_sim_time = current_event.schedule_timestamp;
				}

				event_id = current_event.pid;

				if (current_event.transition_id == 1) //created to ready
				{
					read_process_queue[event_id].prev_time = current_sim_time;
					scheduling->add_process(read_process_queue[event_id]);
					Event event(event_id, read_process_queue[event_id].at, read_process_queue[event_id].at, 2); //2 - ready to running
					put_event(event);
				}
				else if (current_event.transition_id == 2) //ready to running
				{
					if (current_event.schedule_timestamp < next_run_time)
					{
						current_event.schedule_timestamp = next_run_time;
						put_event(current_event);
						continue;
					}

					Process proc = scheduling->get_next_process();
					event_id = proc.pid;
					current_event.create_timestamp = read_process_queue[event_id].prev_time;
					read_process_queue[event_id].cw = read_process_queue[event_id].cw + current_sim_time - current_event.create_timestamp;


					if (scheduling->get_scheduler_name() == "RR" || scheduling->get_scheduler_name() == "PRIO")
					{
						int quant = scheduling->quantum;
						if (read_process_queue[event_id].rr_pr_random_cb == 0)
						{
							if (offset >= 40000)
							{
								offset = 0;
							}
							read_process_queue[event_id].rr_pr_random_cb = 1 + (random_number[offset] % read_process_queue[event_id].cb);
							offset++;
						}

						if (quant <= read_process_queue[event_id].rr_pr_random_cb)
						{
							if (quant >= read_process_queue[event_id].rem_cb)
							{
								Event event(event_id, current_sim_time, read_process_queue[event_id].rem_cb + current_sim_time, 6); //6 - process complete
								next_run_time = read_process_queue[event_id].rem_cb + current_sim_time;
								read_process_queue[event_id].rem_cb = 0;
								read_process_queue[event_id].rr_pr_random_cb = 0;
								put_event(event);
							}
							else
							{
								if (read_process_queue[event_id].rr_pr_random_cb != quant)
								{
									Event event(event_id, current_sim_time, quant + current_sim_time, 5); //3 - running to block, 5 - running to ready
									next_run_time = quant + current_sim_time;
									read_process_queue[event_id].rem_cb = read_process_queue[event_id].rem_cb - quant;
									read_process_queue[event_id].rr_pr_random_cb = read_process_queue[event_id].rr_pr_random_cb - quant;
									put_event(event);
								}
								else
								{
									Event event(event_id, current_sim_time, read_process_queue[event_id].rr_pr_random_cb + current_sim_time, 3); //3 - running to block, 5 - running to ready
									next_run_time = read_process_queue[event_id].rr_pr_random_cb + current_sim_time;
									read_process_queue[event_id].rem_cb = read_process_queue[event_id].rem_cb - read_process_queue[event_id].rr_pr_random_cb;
									read_process_queue[event_id].rr_pr_random_cb = 0;
									put_event(event);
								}
							}
						}
						else if (quant > read_process_queue[event_id].rr_pr_random_cb)
						{
							if (read_process_queue[event_id].rr_pr_random_cb >= read_process_queue[event_id].rem_cb)
							{
								Event event(event_id, current_sim_time, read_process_queue[event_id].rem_cb + current_sim_time, 6); //6 - process complete
								next_run_time = read_process_queue[event_id].rem_cb + current_sim_time;
								read_process_queue[event_id].rem_cb = 0;
								read_process_queue[event_id].rr_pr_random_cb = 0;
								put_event(event);
							}
							else
							{
								Event event(event_id, current_sim_time, read_process_queue[event_id].rr_pr_random_cb + current_sim_time, 3); //3 - running to block
								next_run_time = read_process_queue[event_id].rr_pr_random_cb + current_sim_time;
								read_process_queue[event_id].rem_cb = read_process_queue[event_id].rem_cb - read_process_queue[event_id].rr_pr_random_cb;
								read_process_queue[event_id].rr_pr_random_cb = 0;
								put_event(event);
							}
						}
					}

					else //for non-RR
					{
						if (offset >= 40000)
						{
							offset = 0;
						}
						int random_cpu_burst = 1 + (random_number[offset] % read_process_queue[event_id].cb);
						offset++;

						read_process_queue[event_id].random_cb = random_cpu_burst;

						if (read_process_queue[event_id].rem_cb <= random_cpu_burst) //complete process
						{
							int rem_cpu_burst = read_process_queue[event_id].rem_cb;
							read_process_queue[event_id].rem_cb = 0;
							Event event(event_id, current_sim_time, rem_cpu_burst + current_sim_time, 6); //6 - process complete
							put_event(event);
							next_run_time = rem_cpu_burst + current_sim_time;
						}
						else //transition to blocked
						{
							read_process_queue[event_id].rem_cb = read_process_queue[event_id].rem_cb - random_cpu_burst;
							Event event(event_id, current_sim_time, random_cpu_burst + current_sim_time, 3); //3 - running to blocked
							put_event(event);
							next_run_time = random_cpu_burst + current_sim_time;
						}
					}
				}

				else if (current_event.transition_id == 3) //running to blocked
				{
					if (offset >= 40000)
					{
						offset = 0;
					}
					rem_io_burst = 1 + (random_number[offset] % read_process_queue[event_id].io);
					offset++;

					Event event(event_id, current_sim_time, rem_io_burst + current_sim_time, 4); //4 - block to ready
					read_process_queue[event_id].it = read_process_queue[event_id].it + rem_io_burst;
					put_event(event);

					IOBlock iob;
					iob.start = current_sim_time;
					iob.finish = rem_io_burst + current_sim_time;
					ioblock.push_back(iob);
				}

				else if (current_event.transition_id == 4) //blocked to ready
				{
					read_process_queue[event_id].dyn_priority = read_process_queue[event_id].static_priority - 1;
					read_process_queue[event_id].prev_time = current_sim_time;
					scheduling->add_process(read_process_queue[event_id]);
					Event event(event_id, current_sim_time, current_sim_time, 2); //2 - ready to running
					put_event(event);
				}

				else if (current_event.transition_id == 5) //running to ready
				{
					read_process_queue[event_id].prev_time = current_sim_time;
					Event event(event_id, current_sim_time, current_sim_time, 2); //2 - ready to running

					if (scheduling->get_scheduler_name() == "PRIO")
					{
						read_process_queue[event_id].dyn_priority--;
						if (read_process_queue[event_id].dyn_priority == reset_dyn_priority)
						{
							read_process_queue[event_id].dyn_priority = read_process_queue[event_id].static_priority - 1;
							scheduling->add_exp_process(read_process_queue[event_id]);
							put_event(event);
						}
						else
						{
							put_event(event);
							scheduling->add_process(read_process_queue[event_id]);
						}
					}
					else
					{
						put_event(event);
						scheduling->add_process(read_process_queue[event_id]);
					}
				}

				else if (current_event.transition_id == 6) //process completion
				{
					read_process_queue[event_id].ft = current_sim_time;
					read_process_queue[event_id].tt = read_process_queue[event_id].ft - read_process_queue[event_id].at;
				}
			}

			sort(ioblock.begin(), ioblock.end());
			for (unsigned int i = 0; i < ioblock.size(); i++)
			{
				if (i == 0 || fin <= ioblock[i].start)
				{
					io_util = io_util + ioblock[i].finish - ioblock[i].start;
					fin = ioblock[i].finish;
				}
				else if (fin < ioblock[i].finish)
				{
					io_util = io_util + ioblock[i].finish - fin;
					fin = ioblock[i].finish;
				}
			}

			cout << scheduling->get_scheduler_name();
			if (scheduling->get_scheduler_name() == "RR" || scheduling->get_scheduler_name() == "PRIO")
			{
				cout << " " << scheduling->quantum;
			}
			cout << endl;
			for (unsigned int i = 0; i < read_process_queue.size(); i++)
			{
				Process proc = read_process_queue[i];
				printf("%04d: %4d %4d %4d %4d %1d | %5d %5d %5d %5d\n", proc.pid, proc.at, proc.tc, proc.cb, proc.io, proc.static_priority, proc.ft, proc.ft - proc.at, proc.it, proc.cw);

				if (proc.ft > ft_last_event)
				{
					ft_last_event = proc.ft;
				}
				cpu_util += proc.tc;
				turnaround += proc.ft - proc.at;
				cpu_wait += proc.cw;
			}

			cpu_util = cpu_util * 100 / ft_last_event;
			io_util = io_util * 100 / ft_last_event;
			turnaround = turnaround / read_process_queue.size();
			cpu_wait = cpu_wait / read_process_queue.size();
			throughput = float(read_process_queue.size() * 100) / ft_last_event;

			printf("SUM: %d %.2lf %.2lf %.2lf %.2lf %.3lf\n", ft_last_event, cpu_util, io_util, turnaround, cpu_wait, throughput);
		}
};


int main(int argc, char **argv)
{
	DES_interface desinterface;

	if (argc <= 2)
	{
		cout << "Invalid number of arguments. Please pass the scheduling type, input file and random_number file." << endl;
		exit(0);
	}
	else
	{
		for (int i = 1; i < argc; i++)
		{
			if (strcmp(argv[i],"-v") == 0)
			{
				desinterface.set_verbose(true);
			}
			else if (argv[i][1] == 's')
			{
				if (argv[i][2] == 'F')
				{
					desinterface.set_scheduling_algo(0);
				}
				else if (argv[i][2] == 'L')
				{
					desinterface.set_scheduling_algo(1);
				}
				else if (argv[i][2] == 'S')
				{
					desinterface.set_scheduling_algo(2);
				}
				else if (argv[i][2] == 'R')
				{
					desinterface.set_scheduling_algo(3);
					string str = argv[i];
					rrnum = atoi(str.substr(3,strlen(argv[i])).c_str());
					desinterface.set_round_robin_num(rrnum);
				}
				else if (argv[i][2] == 'P')
				{
					desinterface.set_scheduling_algo(4);
					string str = argv[i];
					pnum = atoi(str.substr(3,strlen(argv[i])).c_str());
					desinterface.set_prio_num(pnum);
				}
				else
				{
					cout << "Invalid scheduling algorithm." << endl;
					exit(0);
				}
			}

			else
			{
				if (file_received)
				{
					ifstream input_file(argv[i]);
					if (!input_file.is_open())
					{
						cout << "The input file could not be opened." << endl;

					}
					else
					{
						string l;
						int arrtime, timecomp, cburst, ioburst, pid=0;
						while (input_file >> arrtime >> timecomp >> cburst >> ioburst)
						{
							read_process_queue.push_back(Process(pid, arrtime, timecomp, cburst, ioburst));
							pid++;
						}
					}
					file_received = false;
				}

				else
				{
					ifstream random_file(argv[i]);
					if (!random_file.is_open())
					{
						cout << "The random_number file could not be opened." << endl;
					}
					else
					{
						string l;
						int r_index = 0;
						while (getline(random_file, l))
						{
							if(r_index == 0)
							{
								r_index++;
								continue;
							}

							random_number.push_back(atoi(l.c_str()));
							r_index++;
						}

					}
				}
			}
		}

		desinterface.simulation();
	}
	return 0;
}
