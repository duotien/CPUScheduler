/*=======================================================
Program: CPU Scheduling Algorithm
Author: Tien Duong
=========================================================*/
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <string>
#include <sstream>
#include <cmath>

using namespace std;

const int MAX = 1000;
//======================================================//
//		ENUM
//======================================================//
enum AlgorithmType
{
	A_UNIDENTIFIED,
	A_FCFS,
	A_SJF,
	A_SRTF,
	A_RR,
	A_ALL
};
//======================================================//
//		All Structures here
//======================================================//
struct Process
{
	string name;
	int burst, in, ioin, iodur;
};
struct Queue
{
	int front, rear;
	Process list[MAX];
};
//======================================================//
//		Operator Overloads
//======================================================//
bool operator==(Process p1, Process p2)
{
	return (p1.name == p2.name &&
		p1.burst == p2.burst &&
		p1.in == p2.in &&
		p1.ioin == p2.ioin &&
		p1.iodur == p2.iodur);
}
bool operator!=(Process p1, Process p2)
{
	return !(p1 == p2);
}
ostream& operator<<(ostream& out, Process rhs)
{
	out << "{"
		<< rhs.name << ", "
		<< rhs.burst << ", "
		<< rhs.in << ", "
		<< rhs.ioin << ", "
		<< rhs.iodur << "}";
	return out;
}
//======================================================//
//		Prototypes
//======================================================//
//Queue
void init(Queue& q);
void push(Queue& q, Process p);
void pop(Queue& q);
void pop_all(Queue& q);
bool isFull(Queue q);
bool isEmpty(Queue q);
Process front(Queue q);
int getLength(Queue q);
//Queue helpers
void memsetQueue(Queue& q, Process value);
void memsetQueue(Queue& q, int front, int rear, Process value);
void assignQueueToArray(Process p[], int& size, Queue q);
void assignArrayToQueue(Queue & q, Process p[], int size);
void sortQueue(Queue & q, bool firstelement = false);
void printQueue(Queue q, bool printfront = true);
bool compareBurstTime(Process p1, Process p2);
bool compareArrivalTime(Process p1, Process p2);
//Input
void input();
void input(string filename);
AlgorithmType stringToEnum(string name);
int getStringtoInt(string snum);
int GCD(int a, int b);
int GCDofProcess(Process p);
int findGCD(Process p[], int n, int quantum, int check_time);
//Debug-Output
void output(bool debug_mode = false);
void output_debug();
void printSchedulerStep(Queue ready, Queue io, int t);
void printHeaders(string algorithm_name = "");
void printRRSchedulerStep(Queue ready, Queue io, int t, int RRcount);
void printRRHeaders();
//String
string getPrintQueueString(Queue q, bool printfront = true);
string getResultString(Queue ready, Queue io, int t);
//Scheduling algorithms
void FCFS(int check_time = -1, bool printstep = false);
void SJF(int check_time = -1, bool printstep = false);
void SRTF(int check_time = -1, bool printstep = false);
void RR(int check_time = -1, bool printstep = false);
//algorithms helpers
void executeProcess(Queue& ready, Queue& io, int step);
bool checkProcessCompletion(Queue& ready, Queue& io, bool ready_queue_only = true);
bool checkProcessArrival(Queue& ready, int& index, int t);
bool checkProcessIOTime(Queue& ready, Queue& io);
bool checkProcessIOCompletion(Queue& ready, Queue& io);
bool processIsCompleted(Queue q);
bool processNeedIO(Queue ready);
bool processDoneIO(Queue io);
void SJFSorter(Queue& q, bool ready_queue_was_empty);
void SRTFSorter(Queue& q);
//======================================================//
//		All Global Variables here
//======================================================//
Process			p[MAX];
int				n, quantum, check_time;
int				step, step_RR;
int				running_time;
string			result = "";
Queue			process;
AlgorithmType	algorithm_type;
//======================================================//
//		All Constants here
//======================================================//
const Process NULLDATA = { "-",-1,-1,-1,-1 };
const int DECIMAL_PLACES_LIMIT = 2;
const int DECIMAL_VALUE = pow(10, DECIMAL_PLACES_LIMIT);
const int TAB = 15;
const int QUEUE_TAB = TAB + 20;
//======================================================//
int main()
{
	//remove 'input.txt' to manually input from keyboard
	//use: output(true); to print step by step;
	input("input.txt");
	output();
	return 0;
}
//======================================================//
/*-------------------------------------------------------
			Definition
---------------------------------------------------------*/
//Queue
void init(Queue& q)
{
	q.front = 0;
	q.rear = -1;
	memsetQueue(q, NULLDATA);
}
void push(Queue& q, Process p)
{
	if (!isFull(q))
	{
		if (q.rear == MAX - 1)	q.rear = 0;
		else					q.rear++;
		q.list[q.rear] = p;
	}
}
void pop(Queue& q)
{
	if (!isEmpty(q))
	{
		q.list[q.front] = NULLDATA;
		if (q.front == MAX - 1) q.front = 0;
		else					q.front++;
	}
}
void pop_all(Queue & q)
{
	while (!isEmpty(q))
	{
		pop(q);
	}
}
bool isFull(Queue q)
{
	if (q.front == 0 && q.rear == MAX - 1 && q.list[q.front] != NULLDATA) return true;
	if (q.front == q.rear + 1 && q.list[q.front] != NULLDATA) return true;
	return false;
}
bool isEmpty(Queue q)
{
	return q.list[q.front] == NULLDATA;
}
Process front(Queue q)
{
	return q.list[q.front];
}
int getLength(Queue q)
{
	if (q.front <= q.rear) return (q.rear - q.front + 1);
	else return (q.rear + 1) + (MAX - q.front);
}
//Queue Helpers
void memsetQueue(Queue& q, Process value)
{
	for (int i = 0; i < MAX; ++i)
	{
		q.list[i] = value;
	}
}
void memsetQueue(Queue& q, int front, int rear, Process value)
{
	if (front < rear)
	{
		for (int i = front; i <= rear; ++i) q.list[i] = value;
	}
	else
	{
		for (int i = front; i < MAX; ++i) q.list[i] = value;
		for (int i = 0; i <= rear; ++i) q.list[i] = value;
	}
}
void assignQueueToArray(Process p[], int& size, Queue q)
{
	int i = 0;
	while (!isEmpty(q) && i < size)
	{
		p[i] = front(q);
		pop(q);
		++i;
	}
}
void assignArrayToQueue(Queue& q, Process p[], int size)
{
	int i = 0;
	pop_all(q);
	while (!isFull(q) && i < size)
	{
		push(q, p[i]);
		++i;
	}
}
void sortQueue(Queue& q, bool firstelement)
{
	int size = getLength(q);
	Process* tmp = new Process[size];
	assignQueueToArray(tmp, size, q);
	if (firstelement) sort(tmp, tmp + size, compareBurstTime);
	else sort(tmp + 1, tmp + size, compareBurstTime);
	assignArrayToQueue(q, tmp, size);
	delete[] tmp;
}
void printQueue(Queue q, bool printfront)
{
	if (!printfront)
	{
		pop(q);
	}
	if (isEmpty(q)) cout << "Queue is Empty";
	while (!isEmpty(q))
	{
		cout << front(q);
		pop(q);
	}
}
bool compareBurstTime(Process p1, Process p2)
{
	return p1.burst < p2.burst;
}
bool compareArrivalTime(Process p1, Process p2)
{
	return p1.in < p2.in;
}
//input
void input()
{
	string stype, scheck_time, sburst, sin, sioin, siodur;
	cin >> stype >> n;
	algorithm_type = stringToEnum(stype);
	for (int i = 0; i < n; ++i)
	{
		cin >> p[i].name >> sburst >> sin >> sioin >> siodur;
		p[i].burst = getStringtoInt(sburst);
		p[i].in = getStringtoInt(sin);
		p[i].ioin = getStringtoInt(sioin);
		p[i].iodur = getStringtoInt(siodur);
	}
	sort(p, p + n, compareArrivalTime);
	if (algorithm_type == A_RR || algorithm_type == A_ALL)
	{
		string squantum;
		cin >> squantum;
		quantum = getStringtoInt(squantum);
	}
	else quantum = 0;
	cin >> scheck_time;
	check_time = getStringtoInt(scheck_time);
	step = findGCD(p, n, 0, check_time);
	step_RR = findGCD(p, n, quantum, check_time);
}
void input(string filename)
{
	ifstream f;
	f.open(filename.c_str());
	if (f.is_open())
	{
		string stype, scheck_time, sburst, sin, sioin, siodur;
		f >> stype >> n;
		algorithm_type = stringToEnum(stype);
		for (int i = 0; i < n; ++i)
		{
			f >> p[i].name >> sburst >> sin >> sioin >> siodur;
			p[i].burst = getStringtoInt(sburst);
			p[i].in = getStringtoInt(sin);
			p[i].ioin = getStringtoInt(sioin);
			p[i].iodur = getStringtoInt(siodur);
		}
		sort(p, p + n, compareArrivalTime);
		if (algorithm_type == A_RR || algorithm_type == A_ALL)
		{
			string squantum;
			f >> squantum;
			quantum = getStringtoInt(squantum);
		}
		else quantum = 0;
		f >> scheck_time;
		check_time = getStringtoInt(scheck_time);
		step = findGCD(p, n, 0, check_time);
		step_RR = findGCD(p, n, quantum, check_time);
		f.close();
	}
	else cout << "Can't open the file." << endl;
}
AlgorithmType stringToEnum(string name)
{
	for (int i = 0; i < name.length(); ++i)
	{
		name[i] = tolower(name[i]);
	}
	if (name == "fcfs") return A_FCFS;
	if (name == "sjf") return A_SJF;
	if (name == "srtf") return A_SRTF;
	if (name == "rr") return A_RR;
	if (name == "all") return A_ALL;
	return A_UNIDENTIFIED;
}
int getStringtoInt(string snum)
{
	int whole = 0;
	int decimalplace = 0;
	bool isdecimal = false;
	for (int i = 0; i < snum.length() && decimalplace < DECIMAL_PLACES_LIMIT; ++i)
	{
		if (snum[i] == '.')
		{
			isdecimal = true;
			continue;
		}
		if (isdecimal) ++decimalplace;
		whole = (whole * 10) + (snum[i] - '0');
	}
	while (decimalplace < DECIMAL_PLACES_LIMIT)
	{
		whole *= 10;
		++decimalplace;
	}
	return whole;
}
int GCD(int a, int b)
{
	int r;
	while (b != 0) {
		r = a % b;
		a = b;
		b = r;
	}
	return a;
}
int GCDofProcess(Process p)
{
	int result;
	result = GCD(p.burst, p.in);
	result = GCD(p.iodur, result);
	result = GCD(p.ioin, result);
	return result;
}
int findGCD(Process p[], int n, int quantum, int check_time)
{
	int result = GCD(quantum, check_time);
	for (int i = 0; i < n; ++i)
	{
		result = GCD(GCDofProcess(p[i]), result);
	}
	return result;
}
//Debug - Output
void output(bool debug_mode)
{
	cout << setprecision(DECIMAL_PLACES_LIMIT) << fixed;
	if (debug_mode) output_debug();
	switch (algorithm_type)
	{
	case A_FCFS:
		result = "First Come, First Served:\n";
		FCFS(check_time, debug_mode);
		break;
	case A_SJF:
		result = "Shortest Job First:\n";
		SJF(check_time, debug_mode);
		break;
	case A_SRTF:
		result = "Shortest Remaining Time First:\n";
		SRTF(check_time, debug_mode);
		break;
	case A_RR:
		result = "Round Robin:\n";
		RR(check_time, debug_mode);
		break;
	case A_ALL:
		result += "First Come, First Served:\n";
		FCFS(check_time, debug_mode);
		result += "Shortest Job First:\n";
		SJF(check_time, debug_mode);
		result += "Shortest Remaining Time First:\n";
		SRTF(check_time, debug_mode);
		result += "Round Robin:\n";
		RR(check_time, debug_mode);
		break;
	default:
		result = "Unidentified Algorithm type.\ntype: FCFS, SJF, SRTF, ALL.\n";
		break;
	}
	if (running_time < check_time) result += "total run-time exceeded.\n";
	cout << result;
}
void output_debug()
{
	cout << "==================================" << endl
		<< "debug" << endl
		<< "==================================" << endl
		<< "NULLDATA = "<< NULLDATA << endl
		<< "DECIMAL_LIMIT = " << DECIMAL_PLACES_LIMIT << endl
		<< "DECIMAL_VALUE = " << DECIMAL_VALUE << endl
		<< "TAB = " << TAB << endl
		<< "algorithm type = " << algorithm_type << endl
		<< "n = " << n
		<< "\tquantum = " << quantum
		<< "\tcheck_time = " << check_time << endl;
	for (int i = 0; i < n; ++i)
	{
		cout << i + 1 << ". " << p[i] << endl;
	}
	cout << "STEP = " << step << endl;
	cout << "STEP_RR = " << step_RR << endl;
	cout << "==================================" << endl;
}
void printSchedulerStep(Queue ready, Queue io, int t)
{
	cout << left << setw(TAB) << (float)t / DECIMAL_VALUE;

	cout << setw(TAB);
	if (!isEmpty(ready)) cout << front(ready).name;
	else cout << "-";

	cout << setw(TAB);
	if (!isEmpty(io)) cout << front(io).name;
	else cout << "-";

	cout << setw(QUEUE_TAB) << getPrintQueueString(io, false);
	cout << setw(QUEUE_TAB) << getPrintQueueString(ready, false);
	cout << endl;
}
void printHeaders(string algorithm_name)
{
	cout << algorithm_name << endl;
	cout << left << setw(TAB)
		<< "t" << setw(TAB)
		<< "CPU" << setw(TAB)
		<< "IO" << setw(QUEUE_TAB)
		<< "io-q" << setw(QUEUE_TAB)
		<< "r-q" << endl;
}
void printRRSchedulerStep(Queue ready, Queue io, int t, int RRcount)
{
	cout << left << setw(TAB)
		 << (float)t / DECIMAL_VALUE << setw(TAB)
		 << (float)RRcount / DECIMAL_VALUE;
	
	cout << setw(TAB);
	if (!isEmpty(ready)) cout << front(ready).name;
	else cout << "-";

	cout << setw(TAB);
	if (!isEmpty(io)) cout << front(io).name;
	else cout << "-";

	cout << setw(QUEUE_TAB) << getPrintQueueString(io, false);
	cout << setw(QUEUE_TAB) << getPrintQueueString(ready, false);
	cout << endl;
}
void printRRHeaders()
{
	cout << "Round Robin" << endl;
	cout << left << setw(TAB)
		<< "t" << setw(TAB)
		<< "q" << setw(TAB)
		<< "CPU" << setw(TAB)
		<< "IO" << setw(QUEUE_TAB)
		<< "io-q" << setw(QUEUE_TAB)
		<< "r-q" << endl;
}
//String
string getPrintQueueString(Queue q, bool printfront)
{
	if (!printfront)
	{
		pop(q);
	}
	if (isEmpty(q)) return "-";
	string result = "";
	while (!isEmpty(q))
	{
		result += front(q).name + ",";
		pop(q);
	}result.erase(result.length()-1);
	return result;
}
string getResultString(Queue ready, Queue io, int t)
{
	stringstream result;
	result << left << setw(TAB) << "t = " << (float)t / DECIMAL_VALUE << endl;
	result << setw(TAB) << "CPU (running):" << setw(TAB);
	if (!isEmpty(ready)) result << front(ready).name;
	else result << "-";
	result << endl;
	result << setw(TAB) << "IO (waiting):" << setw(TAB);
	if (!isEmpty(io)) result << front(io).name;
	else result << "-";
	result << endl;
	result << setw(TAB) << "IO-queue:" << setw(TAB) << getPrintQueueString(io, false) << endl;
	result << setw(TAB) << "Ready-queue:" << setw(TAB) << getPrintQueueString(ready, false) << endl << endl;
	return result.str();
}
//Scheduling Algorithms
void FCFS(int check_time, bool printstep)
{
	Queue ready, io;
	init(ready);
	init(io);
	int index = 0,
		t = p[0].in;
	if (printstep) printHeaders("First Come First Served");
	do
	{
		checkProcessCompletion(ready, io);
		checkProcessArrival(ready, index, t);
		checkProcessIOTime(ready, io);
		checkProcessIOCompletion(ready, io);
		//print step & get Result//
		if (t == check_time) result += getResultString(ready, io, t);
		if (printstep) printSchedulerStep(ready, io, t);
		else if (t == check_time) break;
		//====================//
		executeProcess(ready, io, step);
		t += step;
	} while (!isEmpty(ready) || !isEmpty(io));
	if (printstep) cout << "==================================" << endl;
	running_time = t;
}
void SJF(int check_time, bool printstep)
{
	Queue ready, io;
	init(ready);
	init(io);
	int index = 0,
		t = p[0].in;
	bool detect_change,
		 ready_queue_was_empty;
	if (printstep) printHeaders("Shortest Job First");
	do
	{
		checkProcessCompletion(ready, io);
		ready_queue_was_empty = isEmpty(ready);
		detect_change = checkProcessArrival(ready, index, t);
		if (detect_change) SJFSorter(ready, ready_queue_was_empty);
		checkProcessIOTime(ready, io);
		ready_queue_was_empty = isEmpty(ready);
		detect_change = checkProcessIOCompletion(ready, io);
		if (detect_change) SJFSorter(ready, ready_queue_was_empty);
		//print step & get Result//
		if (t == check_time) result += getResultString(ready, io, t);
		if (printstep) printSchedulerStep(ready, io, t);
		else if (t == check_time) break;
		//====================//
		executeProcess(ready, io, step);
		t += step;
	} while (!isEmpty(ready) || !isEmpty(io));
	if(printstep) cout << "==================================" << endl;
	running_time = t;
}
void SRTF(int check_time, bool printstep)
{
	Queue ready, io;
	init(ready);
	init(io);
	int index = 0,
		t = p[0].in;
	bool detect_changes;
	if (printstep) printHeaders("Shortest Remaining Time First");
	do
	{
		checkProcessCompletion(ready, io);
		detect_changes = checkProcessArrival(ready, index, t);
		if (detect_changes) SRTFSorter(ready);
		checkProcessIOTime(ready, io);
		detect_changes = checkProcessIOCompletion(ready, io);
		if (detect_changes) SRTFSorter(ready);
		//print step & get Result//
		if (t == check_time) result += getResultString(ready, io, t);
		if (printstep) printSchedulerStep(ready, io, t);
		else if (t == check_time) break;
		//====================//
		executeProcess(ready, io, step);
		t += step;
	} while (!isEmpty(ready) || !isEmpty(io));
	if (printstep) cout << "==================================" << endl;
	running_time = t;
}
void RR(int check_time, bool printstep)
{
	Queue ready, io;
	init(ready);
	init(io);
	int index = 0,
		t = p[0].in,
		RRcount = 0;
	bool detect_change;
	if (printstep) printRRHeaders();
	do
	{
		detect_change = checkProcessCompletion(ready, io);
		if (detect_change || isEmpty(ready)) RRcount = 0;
		checkProcessArrival(ready, index, t);
		if (RRcount != quantum)
		{
			detect_change = checkProcessIOTime(ready, io);
			if (detect_change) RRcount = 0;
			checkProcessIOCompletion(ready, io);
		}
		else
		{
			checkProcessIOCompletion(ready, io);
			push(ready, front(ready));
			pop(ready);
			checkProcessIOTime(ready, io);
			RRcount = 0;
		}
		//print step & get Result//
		if (t == check_time) result += getResultString(ready, io, t);
		if (printstep) printRRSchedulerStep(ready, io, t, RRcount);
		else if (t == check_time) break;
		//====================//
		executeProcess(ready, io, step_RR);
		t += step_RR;
		RRcount += step_RR;
	} while (!isEmpty(ready) || !isEmpty(io));
	if (printstep) cout << "==================================" << endl;
	running_time = t;
}
//Algorithm Helpers
void executeProcess(Queue& ready, Queue& io, int step)
{
	if (!isEmpty(ready))
	{
		ready.list[ready.front].burst -= step;
		ready.list[ready.front].ioin -= step;
	}
	if (!isEmpty(io))
	{
		io.list[io.front].iodur -= step;
		if (io.list[io.front].iodur == 0) io.list[io.front].ioin = -1;
	}
}
bool checkProcessCompletion(Queue& ready, Queue& io, bool ready_queue_only)
{
	bool detect_change = false;
	if (processIsCompleted(ready))
	{
		pop(ready);
		detect_change = true;
	}
	if (processIsCompleted(io))
	{
		pop(io);
		if(!ready_queue_only) detect_change = true;
	}
	return detect_change;
}
bool checkProcessArrival(Queue& ready, int& index, int t)
{
	//push arrived process into ready-q
	bool detect_change = false;
	while (t == p[index].in && index < n)
	{
		push(ready, p[index]);
		index++;
		detect_change = true;
	}
	return detect_change;
}
bool checkProcessIOTime(Queue& ready, Queue& io)
{
	//push process into io-q
	bool detect_change = false;
	while (processNeedIO(ready))
	{
		push(io, front(ready));
		pop(ready);
		detect_change = true;
	}
	return detect_change;
}
bool checkProcessIOCompletion(Queue& ready, Queue& io)
{
	bool detect_change = false;
	while (processDoneIO(io))
	{
		push(ready, front(io));
		pop(io);
		detect_change = true;
	}
	return detect_change;
}
bool processIsCompleted(Queue q)
{
	if (!isEmpty(q)) return (front(q).burst == 0 && front(q).iodur == 0);
	return false;
}
bool processNeedIO(Queue ready)
{
	if (!isEmpty(ready))
	{
		if (ready.list[ready.front].iodur == 0) ready.list[ready.front].ioin = -1;
		return (front(ready).ioin == 0);
	}
	return false;
}
bool processDoneIO(Queue io)
{
	if (!isEmpty(io)) return (front(io).iodur == 0);
	return false;
}
void SJFSorter(Queue& q, bool ready_queue_was_empty)
{
	if (ready_queue_was_empty) sortQueue(q, true);
	else					   sortQueue(q);
}
void SRTFSorter(Queue& q)
{
	sortQueue(q, true);
}