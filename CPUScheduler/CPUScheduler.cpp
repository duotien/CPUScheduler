#include <iostream>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <string>
#include <sstream>
#include <cmath>

using namespace std;
const int MAX = 100;
//======================================================//
//		All Structures here
//======================================================//
struct Process
{
	char name;
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
	out << rhs.name << " "
		<< rhs.burst << " "
		<< rhs.in << " "
		<< rhs.ioin << " "
		<< rhs.iodur;
	return out;
}
bool arriveFirst(Process p1, Process p2)
{
	return p1.in < p2.in;
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
//file
//
//
//Input
void input();
void input(string filename);
int getStringtoInt(string snum);
int GCD(int a, int b);
int GCDofProcess(Process p);
int findGCD(Process p[], int n, int quantum, int check_time);
//Debug-Output
void output_debug();
void printSchedulerStep(Queue ready, Queue io, int t);
void printHeaders(string algorithm_name = "");
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
bool checkProcessCompletion(Queue& ready, Queue& io);
bool checkProcessArrival(Queue& ready, int& index, int t);
bool checkProcessIOTime(Queue& ready, Queue& io);
bool checkProcessIOCompletion(Queue& ready, Queue& io);
bool processIsCompleted(Queue q);
bool processNeedIO(Queue ready);
bool processDoneIO(Queue io);
void SJFSorter(Queue& q);
void SRTFSorter(Queue& q);
//=============================================================================================================================This need works
void SuppressIO();

//======================================================//
//		All Global Variables here
//======================================================//
Process			p[MAX];
int				n, quantum, check_time;
int				step, step_RR;
string			result;
Queue			process;
//======================================================//
//		All Constants here
//======================================================//
const Process NULLDATA = { '-',-1,-1,-1,-1 };

const int decimal_places = 3;
const int decimal_value = pow(10, decimal_places);
const int tab = 15;

//======================================================//

int main()
{
	input("input.txt");
	cout << setprecision(decimal_places) << fixed;
	output_debug();
	//FCFS(check_time);
	//FCFS();
	//SJF(check_time, true);
	//SRTF(check_time, true);
	RR(check_time, true);

	cout << result;
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
	while (!isEmpty(q)) pop(q);
}
bool isFull(Queue q)
{
	if (q.front == 0 && q.rear == MAX - 1) return true;
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
	string squantum, scheck_time, sburst, sin, sioin, siodur;
	cin >> n >> squantum >> scheck_time;
	quantum = getStringtoInt(squantum);
	check_time = getStringtoInt(scheck_time);
	for (int i = 0; i < n; ++i)
	{
		cin >> p[i].name >> sburst >> sin >> sioin >> siodur;
		p[i].burst = getStringtoInt(sburst);
		p[i].in = getStringtoInt(sin);
		p[i].ioin = getStringtoInt(sioin);
		p[i].iodur = getStringtoInt(siodur);
	}
	step = findGCD(p, n, 0, check_time);
	step_RR = findGCD(p, n, quantum, check_time);
	sort(p, p + n, arriveFirst);
}
void input(string filename)
{
	ifstream f;
	f.open(filename.c_str());
	if (f.is_open())
	{
		string squantum, scheck_time, sburst, sin, sioin, siodur;
		f >> n >> squantum >> scheck_time;
		quantum = getStringtoInt(squantum);
		check_time = getStringtoInt(scheck_time);
		for (int i = 0; i < n; ++i)
		{
			f >> p[i].name >> sburst >> sin >> sioin >> siodur;
			p[i].burst = getStringtoInt(sburst);
			p[i].in = getStringtoInt(sin);
			p[i].ioin = getStringtoInt(sioin);
			p[i].iodur = getStringtoInt(siodur);
		}
		step = findGCD(p, n, 0, check_time);
		step_RR = findGCD(p, n, quantum, check_time);
		sort(p, p + n, arriveFirst);
		f.close();
	}
	else cout << "Can't open the file." << endl;
}
int getStringtoInt(string snum)
{
	int whole = 0;
	int decimalplace = 0;
	bool isdecimal = false;
	for (int i = 0; i < snum.length() && decimalplace < decimal_places; ++i)
	{
		if (snum[i] == '.')
		{
			isdecimal = true;
			continue;
		}
		if (isdecimal) ++decimalplace;
		whole = (whole * 10) + (snum[i] - '0');
	}
	while (decimalplace < decimal_places)
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
void output_debug()
{
	cout << "==================================" << endl
		<< "debug" << endl
		<< "==================================" << endl
		<< "NULLDATA = {" << NULLDATA << "}" << endl
		<< "DECIMAL_LIMIT = " << decimal_places << endl
		<< "DECIMAL_VALUE = " << decimal_value << endl
		<< "TAB = " << tab << endl
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
	cout << left << setw(tab) << (float)t / decimal_value;

	cout << setw(tab);
	if (!isEmpty(ready)) cout << front(ready).name;
	else cout << "-";

	cout << setw(tab);
	if (!isEmpty(io)) cout << front(io).name;
	else cout << "-";

	cout << setw(tab) << getPrintQueueString(ready, false);
	cout << setw(tab) << getPrintQueueString(io, false);
	cout << endl;
}
void printHeaders(string algorithm_name)
{
	cout << algorithm_name << endl;
	cout << left << setw(tab)
		<< "t" << setw(tab)
		<< "CPU" << setw(tab)
		<< "IO" << setw(tab)
		<< "r-q" << setw(tab)
		<< "io-q" << endl;
}
//String
string getPrintQueueString(Queue q, bool printfront)
{
	string result = "";
	if (!printfront)
	{
		pop(q);
	}
	if (isEmpty(q)) result = "-";
	while (!isEmpty(q))
	{
		result += front(q).name;
		pop(q);
	}
	return result;
}
string getResultString(Queue ready, Queue io, int t)
{
	stringstream result;
	result << left << setw(tab) << "t = " << (float)t / decimal_value << endl;
	result << setw(tab) << "CPU (running):" << setw(tab);
	if (!isEmpty(ready)) result << front(ready).name;
	else result << "-";
	result << endl;
	result << setw(tab) << "IO (waiting):" << setw(tab);
	if (!isEmpty(io)) result << front(io).name;
	else result << "-";
	result << endl;
	result << setw(tab) << "Ready-queue:" << setw(tab) << getPrintQueueString(ready, false) << endl;
	result << setw(tab) << "IO-queue:" << setw(tab) << getPrintQueueString(io, false) << endl;
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
		//print out all step //
		//and get RESULT here//
		if (t == check_time) result = getResultString(ready, io, t);
		if (printstep) printSchedulerStep(ready, io, t);
		else if (t == check_time) break;
		//====================//
		executeProcess(ready, io, step);
		t += step;
	} while (!isEmpty(ready) || !isEmpty(io));
	if (printstep) cout << "==================================" << endl;
}
void SJF(int check_time, bool printstep)
{
	Queue ready, io;
	init(ready);
	init(io);
	int index = 0,
		t = p[0].in;
	bool detect_changes;
	if (printstep) printHeaders("Shortest Job First");
	do
	{
		checkProcessCompletion(ready, io);
		detect_changes = checkProcessArrival(ready, index, t);
		if (detect_changes) SJFSorter(ready);
		checkProcessIOTime(ready, io);
		detect_changes = checkProcessIOCompletion(ready, io);
		if (detect_changes) SJFSorter(ready);
		//print out all step //
		//and get RESULT here//
		if (t == check_time) result = getResultString(ready, io, t);
		if (printstep) printSchedulerStep(ready, io, t);
		else if (t == check_time) break;
		//====================//
		executeProcess(ready, io, step);
		t += step;
	} while (!isEmpty(ready) || !isEmpty(io));
	if(printstep) cout << "==================================" << endl;
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
		//print out all step //
		//and get RESULT here//
		if (t == check_time) result = getResultString(ready, io, t);
		if (printstep) printSchedulerStep(ready, io, t);
		else if (t == check_time) break;
		//====================//
		executeProcess(ready, io, step);
		t += step;
	} while (!isEmpty(ready) || !isEmpty(io));
	if (printstep) cout << "==================================" << endl;
}
//=============================================================================================================================This need works
void RR(int check_time, bool printstep)
{
	Queue ready, io;
	init(ready);
	init(io);
	int index = 0,
		t = p[0].in,
		RRcount = 0;
	bool detect_change;
	if (printstep) printHeaders("Round Robin");
	do
	{
		detect_change = checkProcessCompletion(ready, io);
		if (detect_change) RRcount = 0;
		checkProcessArrival(ready, index, t);
		//process IO or Complete before the clock hit Quantum
		if (RRcount == quantum)
		{
			//end current process's session if RRcount = quantum
			//if p1 done IO and p2 ended turn, push p1 in first
			//if p need IO => push back into ready-q and wait for next session
			checkProcessIOCompletion(ready, io);
			push(ready, front(ready));
			pop(ready);
			RRcount = 0;
		}
		else
		{
			checkProcessIOTime(ready, io);
			checkProcessIOCompletion(ready, io);
		}
		//print out all step //
		//and get RESULT here//
		if (t == check_time) result = getResultString(ready, io, t);
		if (printstep) printSchedulerStep(ready, io, t);
		else if (t == check_time) break;
		//====================//
		executeProcess(ready, io, step_RR);
		t += step_RR;
		RRcount += step_RR;
	} while (!isEmpty(ready) || !isEmpty(io));
	if (printstep) cout << "==================================" << endl;
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
bool checkProcessCompletion(Queue& ready, Queue& io)
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
		detect_change = true;
	}
	return detect_change;
}
bool checkProcessArrival(Queue& ready, int& index, int t)
{
	// push arrived process into ready-q
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
	// push process into io-q
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
	if (!isEmpty(ready)) return (front(ready).ioin == 0);
	return false;
}
bool processDoneIO(Queue io)
{
	if (!isEmpty(io)) return (front(io).iodur == 0);
	return false;
}
void SJFSorter(Queue& q)
{
	if (isEmpty(q)) sortQueue(q, true);
	else			sortQueue(q);
}
void SRTFSorter(Queue& q)
{
	sortQueue(q, true);
}