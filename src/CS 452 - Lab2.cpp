#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
//#define Debug

using namespace std;

/*
=====================================================================
GLOBAL VARIABLES
=====================================================================
 */

bool running = true;

struct job {
	int pid;
	int burst;
	int ogBurst;
	int arrival;
	int priority;
	int deadline;
	int IO;
	int age;
	int timeToIO;
	int terminationTime;
	int slack;
};

queue<job> Q1;
queue<job> Q2;
queue<job> Q3;
queue<job> Q4;
queue<job> Q5;
queue<job> QIO;
queue<int> missed;
queue<int> completed;

vector<job> Jobs;
vector<job> insert;
vector<job> collision;
vector<job> finished;
vector<job> RTSArr;

job temp;

int waitTime = 0;
int ioCounter = 0;
int startIO;

/*
=====================================================================
HELPER METHODS
=====================================================================
 */

void swap(int a, int b)
{
	int t = a;
	a = b;
	b = t;
}

int partition(int l, int h, int pos)
{
	if (pos == 2) {
		int pivot = insert[h].burst;
		int i = (l - 1);

		for (int j = l; j <= h - 1; j++) {
			if (insert[j].burst <= pivot) {
				i++;
				swap(insert[i], insert[j]);
			}
		}
		swap(insert[i + 1], insert[h]);
		return (i + 1);
	}
	if (pos == 3) {
		int pivot = Jobs[h].arrival;
		int i = (l - 1);

		for (int j = l; j <= h - 1; j++) {
			if (Jobs[j].arrival <= pivot) {
				i++;
				swap(Jobs[i], Jobs[j]);
			}
		}
		swap(Jobs[i + 1], Jobs[h]);
		return (i + 1);
	}
	if (pos == 4) {
		int pivot = insert[h].priority;
		int i = (l - 1);

		for (int j = l; j <= h - 1; j++) {
			if (insert[j].priority <= pivot) {
				i++;
				swap(insert[i], insert[j]);
			}
		}
		swap(insert[i + 1], insert[h]);
		return (i + 1);
	}
	if (pos == 9) {
		int pivot = RTSArr[h].deadline;
		int i = (l - 1);

		for (int j = l; j <= h - 1; j++) {
			if (RTSArr[j].deadline <= pivot) {
				i++;
				swap(RTSArr[i], RTSArr[j]);
			}
		}
		swap(RTSArr[i + 1], RTSArr[h]);
		return (i + 1);
	}
	if (pos == 5) {
		int pivot = Jobs[h].deadline;
		int i = (l - 1);

		for (int j = l; j <= h - 1; j++) {
			if (Jobs[j].deadline <= pivot) {
				i++;
				swap(Jobs[i], Jobs[j]);
			}
		}
		swap(Jobs[i + 1], Jobs[h]);
		return (i + 1);

	} else {
		return 0;
	}
}

void quickSort(int l, int h, int pos)
{
	if (l < h) {
		int piv = partition(l, h, pos);
		quickSort(l, piv - 1, pos);
		quickSort(piv + 1, h, pos);
	}
}

void invalidInput()
{
	cout << "Invalid input... Exiting Simulation";
	exit(0);
}

//Open file of given name, read through and store
//any lines that do not contain negative numbers into Jobs vector
void readFile()
{
	fstream file;
	string fileName;
	string line;
	cout << "Enter file name: ";
	cin >> fileName;
	file.open(fileName);

	//Does file exist
	if (file.fail()) {
		invalidInput();
	} else {
		int a, b, c, d, e, f;
		//Get a line from the file
		while (getline(file, line)) {
			std::stringstream stream(line);
			while (1) {
				//get variables from line
				stream >> a >> b >> c >> d >> e >> f;
				if (!stream)
					break;
				// If line contains no negative numbers, then assign variables to struct and add to vector
				if (a >= 0 && b >= 0 && c >= 0 && d >= 0 && e >= 0 && f >= 0) {
					if (f > 0 && b > 1) {
						job temp;
						temp.pid = a;
						temp.burst = b;
						temp.ogBurst = b;
						temp.arrival = c;
						temp.priority = d;
						temp.deadline = e;
						temp.IO = f;
						temp.timeToIO = (b - 1);
						temp.slack = e - (b + c);

						Jobs.push_back(temp);
					} else {
						job temp;
						temp.pid = a;
						temp.burst = b;
						temp.ogBurst = b;
						temp.arrival = c;
						temp.priority = d;
						temp.deadline = e;
						temp.IO = f;
						temp.timeToIO = -1;
						temp.slack = e - (b + c);

						Jobs.push_back(temp);
					}
				}
			}
		}
	}
	file.close();
}

int findMaxDL()
{
	int max = Jobs[0].deadline;
	for (int i = 1; i < Jobs.size(); i++) {
		if (Jobs[i].deadline > max) {
			max = Jobs[i].deadline;
		}
	}
	return max;
}

bool compareTwo(job a, job b)
{
	// If total marks are not same then
	// returns true for higher total
	if (a.deadline != b.deadline)
		return a.deadline < b.deadline;

	// If marks in Maths are same then
	// returns true for higher marks
	if (a.burst != b.burst)
		return a.burst < b.burst;

	return a.slack < b.slack;
}


void runRTS()
{
	Q1.empty();

	int timeBefore = 0;
	//sort by arrival
	int choice;
	int tick = 0;

	cout << "\nWould you like to run RTS in a (0) soft environment or (1) hard environment? :";
	cin >> choice;
	int deadline = findMaxDL();

	while (tick < findMaxDL()) {
		if (choice == 1 && tick >= deadline) {
			cout << deadline << "-> Deadline wasn't met...\n";
			exit(0);
		}
		if (tick % 1000 == 0) {
			cout << "tick: " << tick << endl;
		}

		while (Jobs[0].arrival == tick && Jobs.size() > 0) {
			RTSArr.push_back(Jobs[0]);
			Jobs.erase(Jobs.begin() + 0); ///cant remmeber the command for this
		}

		if (RTSArr.size() > 1) {
			sort(RTSArr.begin(), RTSArr.end(), compareTwo);
		}

		if (RTSArr.size() > 0) {
			timeBefore = 0;
			for (int i = 0; i < RTSArr.size(); i++) {
				timeBefore += RTSArr[i].burst;
/*				if (RTSArr[i].slack < 0 || RTSArr[i].slack < i || RTSArr[i].slack < timeBefore) {
#ifdef Debug
					cout << "\t" << RTSArr[i].pid << " -> Will not be able to finish in time and was removed at: " << tick << endl;
#endif
					missed.push(RTSArr[i].pid);
					if (deadline > RTSArr[i].deadline) {
						deadline = RTSArr[i].deadline;
					}
					RTSArr.erase(RTSArr.begin() + i);
				}
*/
				if (!(RTSArr[0].burst == 0 && RTSArr.size() > 0) && i != 1) { //optimize slack
					RTSArr[i].slack -= 1;
				}
			}
			if (RTSArr[0].burst == 0 && RTSArr.size() > 0) {
#ifdef Debug
				cout << "\t" << RTSArr[0].pid << " -> Finished at:" << tick << endl;
#endif
				RTSArr[0].terminationTime = tick;
				finished.push_back(RTSArr[0]);
				RTSArr.erase(RTSArr.begin());
				tick++;
			} else {
#ifdef Debug
				cout << RTSArr[0].pid << " -> Was ran at: " << tick << endl;
#endif
				RTSArr[0].burst -= 1;
				tick++;
			}
		}
		else {
			tick++;
		}
	}
#ifdef Debug
	cout << "total completed: " << finished.size() << endl;
	cout << "total missed: " << missed.size() << endl;
#endif
}

/*
=====================================================================
MAIN
=====================================================================
 */

int main()
{
	int runTime;
	string stopSim;
	char inputType;
	char resume;
	string schedulerType;
	int schedulerOption;
	int numQueues;
	int timeQuantum;
	int TEMP;
	int maxAge;
	cout << "Please enter a '1' if you would like to run MFQS or a '2' for RTS: ";
	cin >> schedulerOption;
	if (schedulerOption == 1) {
		cout << "Please enter number of queues (<5): ";
		cin >> numQueues;
		if (numQueues > 5) {
			invalidInput();
		}
		cout << "Please enter Time Quantum for the first Queue: ";
		cin >> timeQuantum;

		cout << "Please enter max Age that a process can be to exit the FCFS Queue: ";
		cin >> maxAge;

		cout << "Please enter an 'f' if you'd like to read from a file or 'i' if you want to insert manually: ";
		cin >> inputType;
		if (inputType == 'f')
			readFile();
		else if (inputType == 'i') {
			bool enterInfo = true;
			while (enterInfo) {
				cout << "Would you like to enter a new process? (y/n): ";
				cin >> resume;
				if (resume == 'y') {
					cout << "Enter P_ID: ";
					cin >> temp.pid;
					cout << "Enter burst time for P_ID " << temp.pid << ": ";
					cin >> temp.burst;
					cout << "Enter arrival time for P_ID " << temp.pid << ": ";
					cin >> temp.arrival;
					cout << "Enter priority for P_ID " << temp.pid << ": ";
					cin >> temp.priority;
					cout << "Enter deadline for P_ID " << temp.pid << ": ";
					cin >> temp.deadline;
					cout << "Enter I/O for P_ID " << temp.pid << ": ";
					cin >> temp.IO;
					temp.timeToIO = 0;
					Jobs.push_back(temp);
				} else
					(enterInfo = false);
			}
		} else
			(invalidInput());

		quickSort(0, Jobs.size() - 1, 3);

		int runTime = 0;
		int processesInQueue = 0;
		int completeTime = 0;
		bool canFinish;
		bool running1 = false, running2 = false, running3 = false, running4 = false;
		int count = 0;
		job temp1;
		job temp2;
		int timeCheck = 0;

#ifdef Debug
		cout << "===== Sorted by Arrival! =====\n";
		for (int i = 0; i < Jobs.size(); i++) {
			cout << Jobs[i].pid << "\t " << Jobs[i].arrival << "\t" << Jobs[i].priority << "\n";
		}
#endif

		switch (numQueues) {
		case 5:
			cout << "5 Queues.. Running Simulation\n"
			<< endl;
			while (count < Jobs.size() || (processesInQueue > 0) || (running1 == true || running2 == true || running3 == true || running4 == true)) {
				if (runTime % 100000 == 0) {
					cout << runTime << endl;
				}
				//Inserting by arrival time and if tied, insert by lower priority first
				while (Jobs[count].arrival == runTime) {
					insert.push_back(Jobs[count]);
					count++;
				}

				//MAY HAVE TO MOVE ABOVE INSERT TO HANDLE COLLISIONS
				if (temp.timeToIO > 0 && (running1 == true || running2 == true || running3 == true || running4 == true)) {
					temp.timeToIO -= 1;
					if (temp.timeToIO == 0) {
						temp.burst -= timeCheck;
						QIO.push(temp);
#ifdef Debug
						cout << "Pushed: " << temp.pid << " to IO at time: " << runTime << endl;
#endif
						timeCheck = 0;
						running1 = false;
						running2 = false;
						running3 = false;
						running4 = false;
					}
				}

				//Queue IO
				if (QIO.size() > 0) {
					for (int i = 0; i < QIO.size(); i++) {
						temp2 = QIO.front();
						if (temp2.IO == 0) {
							insert.push_back(temp2);
#ifdef Debug
							cout << temp2.pid << " -> Finished IO at: " << runTime << endl;
#endif
						} else {
							temp2.IO -= 1;
							QIO.push(temp2);
						}
						QIO.pop();
					}
				}

				if (insert.size() > 1) {
					quickSort(0, insert.size() - 1, 4);
				}

				while (insert.size() > 0) {
#ifdef Debug
					cout << insert[0].pid << " -> Inserted at: " << runTime << " with priority: " << insert[0].priority << endl;
#endif
					Q1.push(insert[0]);
					insert.erase(insert.begin() + 0);
				}

				//Queue 5 (FCFS)
				if (Q5.size() > 0) {
					for (int i = 0; i < Q5.size(); i++) {
						temp1 = Q5.front();
						if (temp1.age >= maxAge) {
							temp1.age = 0;
							insert.push_back(temp1);
						} else {
							temp1.age += 1;
							Q5.push(temp1);
						}
						Q5.pop();
					}
				}

				//Queue 4
				if (running1 == false && running2 == false && running3 == false) {
					if (Q4.size() > 0 && running4 == false) { // THERE IS NO PROCESS BEING "DECREMENTED"
						temp = Q4.front();
#ifdef Debug
						cout << temp.pid << " -> Started Running at: " << runTime << endl;
#endif
						running4 = true;
						timeCheck = 0;
						if (temp.burst <= timeQuantum * 8) { // IF PROCESS CAN FINISH IN TQ
							completeTime = runTime + temp.burst; // The time in which the process "finishes"
							canFinish = true; // The Process can finish --> Checks lower
						} else {
							completeTime = runTime + timeQuantum * 8; // The time in which the process finishes time quantum
							canFinish = false; // The Process cannot finish --> Checks later
						}
						Q4.pop();
					}

					if (runTime == completeTime && running4 == true) { // If the current time is a finishing time for a process
						running4 = false;
#ifdef Debug
						cout << temp.pid << " -> Stopped running at time: " << runTime << endl;
#endif
						if (canFinish == true) { // If the burst <= timeQuantum
#ifdef Debug
							cout << temp.pid << " -> Finished at: " << runTime << endl;
#endif
							completeTime = 0; // Can add another process since there's no time to wait for something to finish
							timeCheck = 0;
							temp.terminationTime = runTime;
							finished.push_back(temp);
						} else { // Burst was >= timeQuantum
							temp.burst -= timeQuantum * 8; // Take away timeQuantum from burst
							Q5.push(temp); // Demote
							completeTime = 0;
							timeCheck = 0;
						}
					}
				}

				//Queue 3
				if (running1 == false && running2 == false && running4 == false) {
					if (Q3.size() > 0 && running3 == false) { // THERE IS NO PROCESS BEING "DECREMENTED"
						temp = Q3.front();
#ifdef Debug
						cout << temp.pid << " -> Started Running at: " << runTime << endl;
#endif
						running3 = true;
						timeCheck = 0;
						if (temp.burst <= timeQuantum * 4) { // IF PROCESS CAN FINISH IN TQ
							completeTime = runTime + temp.burst; // The time in which the process "finishes"
							canFinish = true; // The Process can finish --> Checks lower
						} else {
							completeTime = runTime + timeQuantum * 4; // The time in which the process finishes time quantum
							canFinish = false; // The Process cannot finish --> Checks later
						}
						Q3.pop();
					}

					if (runTime == completeTime && running3 == true) { // If the current time is a finishing time for a process
						running3 = false;
#ifdef Debug
						cout << temp.pid << " -> Stopped running at time: " << runTime << endl;
#endif
						if (canFinish == true) { // If the burst <= timeQuantum
#ifdef Debug
							cout << temp.pid << " -> Finished at: " << runTime << endl;
#endif
							completeTime = 0; // Can add another process since there's no time to wait for something to finish
							timeCheck = 0;
							temp.terminationTime = runTime;
							finished.push_back(temp);
						} else { // Burst was >= timeQuantum
							temp.burst -= timeQuantum * 4; // Take away timeQuantum from burst
							Q4.push(temp); // Demote
							completeTime = 0;
							timeCheck = 0;
						}
					}
				}

				//Queue 2
				if (running1 == false && running3 == false && running4 == false) {
					if (Q2.size() > 0 && running2 == false) { // THERE IS NO PROCESS BEING "DECREMENTED"
						temp = Q2.front();
#ifdef Debug
						cout << temp.pid << " -> Started Running at: " << runTime << endl;
#endif
						running2 = true;
						timeCheck = 0;
						if (temp.burst <= timeQuantum * 2) { // IF PROCESS CAN FINISH IN TQ
							completeTime = runTime + temp.burst; // The time in which the process "finishes"
							canFinish = true; // The Process can finish --> Checks lower
						} else {
							completeTime = runTime + timeQuantum * 2; // The time in which the process finishes time quantum
							canFinish = false; // The Process cannot finish --> Checks later
						}
						Q2.pop();
					}

					if (runTime == completeTime && running2 == true) { // If the current time is a finishing time for a process
						running2 = false;
#ifdef Debug
						cout << temp.pid << " -> Stopped running at time: " << runTime << endl;
#endif
						if (canFinish == true) { // If the burst <= timeQuantum
#ifdef Debug
							cout << temp.pid << " -> Finished at: " << runTime << endl;
#endif
							completeTime = 0; // Can add another process since there's no time to wait for something to finish
							timeCheck = 0;
							temp.terminationTime = runTime;
							finished.push_back(temp);

						} else { // Burst was >= timeQuantum
							temp.burst -= timeQuantum * 2; // Take away timeQuantum from burst
							Q3.push(temp); // Demote
							completeTime = 0;
							timeCheck = 0;
						}
					}
				}

				//Queue 1
				if (running2 == false && running3 == false && running4 == false) {
					if (Q1.size() > 0 && running1 == false) { // THERE IS NO PROCESS BEING "DECREMENTED"
						temp = Q1.front();
#ifdef Debug
						cout << temp.pid << " -> Started Running at: " << runTime << endl;
#endif
						running1 = true;
						timeCheck = 0;
						if (temp.burst <= timeQuantum) { // IF PROCESS CAN FINISH IN TQ
							completeTime = runTime + temp.burst; // The time in which the process "finishes"
							canFinish = true; // The Process can finish --> Checks lower
						} else {
							completeTime = runTime + timeQuantum; // The time in which the process finishes time quantum

							canFinish = false; // The Process cannot finish --> Checks later
						}
						Q1.pop();
					}

					if (runTime == completeTime && running1 == true) {
						running1 = false;
#ifdef Debug
						cout << temp.pid << " -> Stopped running at time: " << runTime << endl;
#endif
						if (canFinish == true) { // If the burst <= timeQuantum
#ifdef Debug
							cout << temp.pid << " -> Finished at: " << runTime << endl;
#endif
							completeTime = 0; // Can add another process since there's no time to wait for something to finish
							timeCheck = 0;
							temp.terminationTime = runTime;
							finished.push_back(temp);

						} else { // Burst was >= timeQuantum

							temp.burst -= timeQuantum; // Take away timeQuantum from burst
							Q2.push(temp); // Add to Q2
							completeTime = 0;
							timeCheck = 0;
						}
					}
				}

				timeCheck += 1;
				runTime += 1;
				processesInQueue = Q1.size() + Q2.size() + Q3.size() + Q4.size() + Q5.size() + QIO.size() + insert.size();
			}
			cout << runTime;

			break;
		case 4:
			cout << "4 Queues.. Running Simulation\n"
			<< endl;
			while (count < Jobs.size() || (processesInQueue > 0) || (running1 == true || running2 == true || running3 == true)) {
				//Inserting by arrival time and if tied, insert by lower priority first
				while (Jobs[count].arrival == runTime) {
					insert.push_back(Jobs[count]);
					count++;
				}

				//MAY HAVE TO MOVE ABOVE INSERT TO HANDLE COLLISIONS
				if (temp.timeToIO > 0 && (running1 == true || running2 == true || running3 == true)) {
					temp.timeToIO -= 1;
					if (temp.timeToIO == 0) {
						temp.burst -= timeCheck;
						QIO.push(temp);
#ifdef Debug
						cout << "Pushed: " << temp.pid << " to IO at time: " << runTime << endl;
#endif
						timeCheck = 0;
						running1 = false;
						running2 = false;
						running3 = false;
					}
				}

				//Queue IO
				if (QIO.size() > 0) {
					for (int i = 0; i < QIO.size(); i++) {
						temp2 = QIO.front();
						if (temp2.IO == 0) {
							insert.push_back(temp2);
#ifdef Debug
							cout << temp2.pid << " -> Finished IO at: " << runTime << endl;
#endif
						} else {
							temp2.IO -= 1;
							QIO.push(temp2);
						}
						QIO.pop();
					}
				}

				if (insert.size() > 1) {
					quickSort(0, insert.size() - 1, 4);
				}

				while (insert.size() > 0) {
#ifdef Debug
					cout << insert[0].pid << " -> Inserted at: " << runTime << " with priority: " << insert[0].priority << endl;
#endif
					Q1.push(insert[0]);
					insert.erase(insert.begin() + 0);
				}

				//Queue 5 (FCFS)
				if (Q5.size() > 0) {
					for (int i = 0; i < Q5.size(); i++) {
						temp1 = Q5.front();
						if (temp1.age >= maxAge) {
							temp1.age = 0;
							insert.push_back(temp1);
						} else {
							temp1.age += 1;
							Q5.push(temp1);
						}
						Q5.pop();
					}
				}

				//Queue 3
				if (running1 == false && running2 == false) {
					if (Q3.size() > 0 && running3 == false) { // THERE IS NO PROCESS BEING "DECREMENTED"
						temp = Q3.front();
						running3 = true;
						timeCheck = 0;
						if (temp.burst <= timeQuantum * 4) { // IF PROCESS CAN FINISH IN TQ
							completeTime = runTime + temp.burst; // The time in which the process "finishes"
							canFinish = true; // The Process can finish --> Checks lower
						} else {
							completeTime = runTime + timeQuantum * 4; // The time in which the process finishes time quantum
							canFinish = false; // The Process cannot finish --> Checks later
						}
						Q3.pop();
					}

					if (runTime == completeTime && running3 == true) { // If the current time is a finishing time for a process
						running3 = false;
						if (canFinish == true) { // If the burst <= timeQuantum
#ifdef Debug
							cout << temp.pid << " -> Finished at: " << runTime << endl;
#endif
							completeTime = 0; // Can add another process since there's no time to wait for something to finish
							timeCheck = 0;
							temp.terminationTime = runTime;
							finished.push_back(temp);
						} else { // Burst was >= timeQuantum
							temp.burst -= timeQuantum * 4; // Take away timeQuantum from burst
							Q5.push(temp); // Demote
							completeTime = 0;
							timeCheck = 0;
						}
					}
				}

				//Queue 2
				if (running1 == false && running3 == false) {
					if (Q2.size() > 0 && running2 == false) { // THERE IS NO PROCESS BEING "DECREMENTED"
						temp = Q2.front();
						running2 = true;
						timeCheck = 0;
						if (temp.burst <= timeQuantum * 2) { // IF PROCESS CAN FINISH IN TQ
							completeTime = runTime + temp.burst; // The time in which the process "finishes"
							canFinish = true; // The Process can finish --> Checks lower
						} else {
							completeTime = runTime + timeQuantum * 2; // The time in which the process finishes time quantum
							canFinish = false; // The Process cannot finish --> Checks later
						}
						Q2.pop();
					}

					if (runTime == completeTime && running2 == true) { // If the current time is a finishing time for a process
						running2 = false;
						if (canFinish == true) { // If the burst <= timeQuantum
#ifdef Debug
							cout << temp.pid << " -> Finished at: " << runTime << endl;
#endif
							completeTime = 0; // Can add another process since there's no time to wait for something to finish
							timeCheck = 0;
							temp.terminationTime = runTime;
							finished.push_back(temp);
						} else { // Burst was >= timeQuantum
							temp.burst -= timeQuantum * 2; // Take away timeQuantum from burst
							Q3.push(temp); // Demote
							completeTime = 0;
							timeCheck = 0;
						}
					}
				}

				//Queue 1
				if (running2 == false && running3 == false) {
					if (Q1.size() > 0 && running1 == false) { // THERE IS NO PROCESS BEING "DECREMENTED"
						temp = Q1.front();
						running1 = true;
						timeCheck = 0;
						if (temp.burst <= timeQuantum) { // IF PROCESS CAN FINISH IN TQ
							completeTime = runTime + temp.burst; // The time in which the process "finishes"
							canFinish = true; // The Process can finish --> Checks lower

						} else {
							completeTime = runTime + timeQuantum; // The time in which the process finishes time quantum
							canFinish = false; // The Process cannot finish --> Checks later
						}
						Q1.pop();
					}

					if (runTime == completeTime && running1 == true) {
						running1 = false;
						if (canFinish == true) { // If the burst <= timeQuantum
#ifdef Debug
							cout << temp.pid << " -> Finished at: " << runTime << endl;
#endif
							completeTime = 0; // Can add another process since there's no time to wait for something to finish
							timeCheck = 0;
							temp.terminationTime = runTime;
							finished.push_back(temp);
						} else { // Burst was >= timeQuantum
							temp.burst -= timeQuantum; // Take away timeQuantum from burst
							Q2.push(temp); // Add to Q2
							completeTime = 0;
							timeCheck = 0;
						}
					}
				}

				timeCheck += 1;
				runTime += 1;
				processesInQueue = Q1.size() + Q2.size() + Q3.size() + Q5.size() + QIO.size() + insert.size();
			}
			break;
		case 3:
			cout << "3 Queues.. Running Simulation\n"
			<< endl;
			while (count < Jobs.size() || (processesInQueue > 0) || (running1 == true || running2 == true)) {
				//Inserting by arrival time and if tied, insert by lower priority first
				while (Jobs[count].arrival == runTime) {
					insert.push_back(Jobs[count]);
					count++;
				}

				//MAY HAVE TO MOVE ABOVE INSERT TO HANDLE COLLISIONS
				if (temp.timeToIO > 0 && (running1 == true || running2 == true)) {
					temp.timeToIO -= 1;
					if (temp.timeToIO == 0) {
						temp.burst -= timeCheck;
						QIO.push(temp);
#ifdef Debug
						cout << "Pushed: " << temp.pid << " to IO at time: " << runTime << endl;
#endif
						timeCheck = 0;
						running1 = false;
						running2 = false;
					}
				}

				//Queue IO
				if (QIO.size() > 0) {
					for (int i = 0; i < QIO.size(); i++) {
						temp2 = QIO.front();
						if (temp2.IO == 0) {
							insert.push_back(temp2);
#ifdef Debug
							cout << temp2.pid << " -> Finished IO at: " << runTime << endl;
#endif
						} else {
							temp2.IO -= 1;
							QIO.push(temp2);
						}
						QIO.pop();
					}
				}

				if (insert.size() > 1) {
					quickSort(0, insert.size() - 1, 4);
				}

				while (insert.size() > 0) {
#ifdef Debug
					cout << insert[0].pid << " -> Inserted at: " << runTime << " with priority: " << insert[0].priority << endl;
#endif
					Q1.push(insert[0]);
					insert.erase(insert.begin() + 0);
				}

				//Queue 5 (FCFS)
				if (Q5.size() > 0) {
					for (int i = 0; i < Q5.size(); i++) {
						temp1 = Q5.front();
						if (temp1.age >= maxAge) {
							temp1.age = 0;
							insert.push_back(temp1);
						} else {
							temp1.age += 1;
							Q5.push(temp1);
						}
						Q5.pop();
					}
				}

				//Queue 2
				if (running1 == false) {
					if (Q2.size() > 0 && running2 == false) { // THERE IS NO PROCESS BEING "DECREMENTED"
						temp = Q2.front();
						running2 = true;
						timeCheck = 0;
						if (temp.burst <= timeQuantum * 2) { // IF PROCESS CAN FINISH IN TQ
							completeTime = runTime + temp.burst; // The time in which the process "finishes"
							canFinish = true; // The Process can finish --> Checks lower
						} else {
							completeTime = runTime + timeQuantum * 2; // The time in which the process finishes time quantum
							canFinish = false; // The Process cannot finish --> Checks later
						}
						Q2.pop();
					}

					if (runTime == completeTime && running2 == true) { // If the current time is a finishing time for a process
						running2 = false;
						if (canFinish == true) { // If the burst <= timeQuantum
#ifdef Debug
							cout << temp.pid << " -> Finished at: " << runTime << endl;
#endif
							completeTime = 0; // Can add another process since there's no time to wait for something to finish
							timeCheck = 0;
							temp.terminationTime = runTime;
							finished.push_back(temp);
						} else { // Burst was >= timeQuantum
							temp.burst -= timeQuantum * 2; // Take away timeQuantum from burst
							Q5.push(temp); // Demote
							completeTime = 0;
							timeCheck = 0;
						}
					}
				}

				//Queue 1
				if (running2 == false) {
					if (Q1.size() > 0 && running1 == false) { // THERE IS NO PROCESS BEING "DECREMENTED"
						temp = Q1.front();
						running1 = true;
						timeCheck = 0;
						if (temp.burst <= timeQuantum) { // IF PROCESS CAN FINISH IN TQ
							completeTime = runTime + temp.burst; // The time in which the process "finishes"
							canFinish = true; // The Process can finish --> Checks lower

						} else {
							completeTime = runTime + timeQuantum; // The time in which the process finishes time quantum
							canFinish = false; // The Process cannot finish --> Checks later
						}
						Q1.pop();
					}

					if (runTime == completeTime && running1 == true) {
						running1 = false;
						if (canFinish == true) { // If the burst <= timeQuantum
#ifdef Debug
							cout << temp.pid << " -> Finished at: " << runTime << endl;
#endif
							completeTime = 0; // Can add another process since there's no time to wait for something to finish
							timeCheck = 0;
							temp.terminationTime = runTime;
							finished.push_back(temp);
						} else { // Burst was >= timeQuantum
							temp.burst -= timeQuantum; // Take away timeQuantum from burst
							Q2.push(temp); // Add to Q2
							completeTime = 0;
							timeCheck = 0;
						}
					}
				}

				timeCheck += 1;
				runTime += 1;
				processesInQueue = Q1.size() + Q2.size() + Q5.size() + QIO.size() + insert.size();
			}
			break;
		case 2:
			cout << "2 Queues.. Running Simulation\n"
			<< endl;
			while (count < Jobs.size() || (processesInQueue > 0) || (running1 == true)) {
				//Inserting by arrival time and if tied, insert by lower priority first
				while (Jobs[count].arrival == runTime) {
					insert.push_back(Jobs[count]);
					count++;
				}

				//MAY HAVE TO MOVE ABOVE INSERT TO HANDLE COLLISIONS
				if (temp.timeToIO > 0 && (running1 == true)) {
					temp.timeToIO -= 1;
					if (temp.timeToIO == 0) {
						temp.burst -= timeCheck;
						QIO.push(temp);
#ifdef Debug
						cout << "Pushed: " << temp.pid << " to IO at time: " << runTime << endl;
#endif
						timeCheck = 0;
						running1 = false;
					}
				}

				//Queue IO
				if (QIO.size() > 0) {
					for (int i = 0; i < QIO.size(); i++) {
						temp2 = QIO.front();
						if (temp2.IO == 0) {
							insert.push_back(temp2);
#ifdef Debug
							cout << temp2.pid << " -> Finished IO at: " << runTime << endl;
#endif
						} else {
							temp2.IO -= 1;
							QIO.push(temp2);
						}
						QIO.pop();
					}
				}

				if (insert.size() > 1) {
					quickSort(0, insert.size() - 1, 4);
				}

				while (insert.size() > 0) {
#ifdef Debug
					cout << insert[0].pid << " -> Inserted at: " << runTime << " with priority: " << insert[0].priority << endl;
#endif
					Q1.push(insert[0]);
					insert.erase(insert.begin() + 0);
				}

				//Queue 5 (FCFS)
				if (Q5.size() > 0) {
					for (int i = 0; i < Q5.size(); i++) {
						temp1 = Q5.front();
						if (temp1.age >= maxAge) {
							temp1.age = 0;
							insert.push_back(temp1);
						} else {
							temp1.age += 1;
							Q5.push(temp1);
						}
						Q5.pop();
					}
				}

				//Queue 1
				if (running2 == false) {
					if (Q1.size() > 0 && running1 == false) { // THERE IS NO PROCESS BEING "DECREMENTED"
						temp = Q1.front();
						running1 = true;
						timeCheck = 0;
						if (temp.burst <= timeQuantum) { // IF PROCESS CAN FINISH IN TQ
							completeTime = runTime + temp.burst; // The time in which the process "finishes"
							canFinish = true; // The Process can finish --> Checks lower

						} else {
							completeTime = runTime + timeQuantum; // The time in which the process finishes time quantum
							canFinish = false; // The Process cannot finish --> Checks later
						}
						Q1.pop();
					}

					if (runTime == completeTime && running1 == true) {
						running1 = false;
						if (canFinish == true) { // If the burst <= timeQuantum
#ifdef Debug
							cout << temp.pid << " -> Finished at: " << runTime << endl;
#endif
							completeTime = 0; // Can add another process since there's no time to wait for something to finish
							timeCheck = 0;
							temp.terminationTime = runTime;
							finished.push_back(temp);
						} else { // Burst was >= timeQuantum
							temp.burst -= timeQuantum; // Take away timeQuantum from burst
							Q5.push(temp); // Add to Q2
							completeTime = 0;
							timeCheck = 0;
						}
					}
				}

				timeCheck += 1;
				runTime += 1;
				processesInQueue = Q1.size() + Q5.size() + QIO.size() + insert.size();
			}
			break;
		default:
			cout << "invalid number of queues, Exiting..." << endl;
			exit(0);
			break;
		}

		//RTS ------------------------------------------------------------------------------------------------------------
	} else if (schedulerOption == 2) {

		//promt user and collect data from files

		cout << "Please enter an 'f' if you'd like to read from a file or 'i' if you want to insert manually: ";
		cin >> inputType;
		if (inputType == 'f')
			readFile();
		else if (inputType == 'i') {
			bool enterInfo = true;
			while (enterInfo) {
				cout << "Would you like to enter a new process? (y/n): ";
				cin >> resume;
				if (resume == 'y') {
					cout << "Enter P_ID: ";
					cin >> temp.pid;
					cout << "Enter burst time for P_ID " << temp.pid << ": ";
					cin >> temp.burst;
					temp.ogBurst = temp.burst;
					cout << "Enter arrival time for P_ID " << temp.pid << ": ";
					cin >> temp.arrival;
					cout << "Enter priority for P_ID " << temp.pid << ": ";
					cin >> temp.priority;
					cout << "Enter deadline for P_ID " << temp.pid << ": ";
					cin >> temp.deadline;
					cout << "Enter I/O for P_ID " << temp.pid << ": ";
					cin >> temp.IO;
					temp.timeToIO = 0;
					Jobs.push_back(temp);
				} else
					(enterInfo = false);
			}
		} else
			(invalidInput());

		quickSort(0, Jobs.size() - 1, 3);

		runRTS();
	} else
		(invalidInput()); // END OF SCHEDULING OPTIONS

	//End simulation or run another
	int turnAround = 0;
	int totalBurst = 0;
	cout << "Program over... Here are the results:\n";
	for (int i = 0; i < finished.size(); i++) {
		turnAround += (finished[i].terminationTime - finished[i].arrival);
		totalBurst += finished[i].ogBurst;
	}
	waitTime = turnAround - totalBurst;
	cout << "Total Burst: " << totalBurst << endl;
	cout << "Turnaround: " << turnAround << endl;
	cout << "Average Turnaround: " << (1.0 * turnAround) / finished.size() << endl;
	cout << "Wait Time: " << waitTime << endl;
	cout << "Average Wait: " << (1.0 * waitTime) / finished.size() << endl;
}
