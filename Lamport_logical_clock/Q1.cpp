#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <string>
#include <map>
#include <set>
#include <algorithm>
using namespace std;

struct Message
{
    string sender;
    string receiver;
    string payload;
    bool delivered;
};

struct Operation
{
    string type;    // "send", "recv", "print"
    string target;  // process name for send/recv
    string message; // message content
};

struct Process
{
    string name;
    vector<Operation> operations;
    int operationIndex;
    int lamportClock;
    bool waiting;              // true if waiting on recv
    string waitingForMsg;      // what message waiting for
    string waitingFromProcess; // from which process
};

struct Event
{
    string type; // "sent", "received", "printed"
    string process;
    string message;
    string otherProcess;
    int lamportClock;

    bool operator<(const Event &other) const
    {
        if (lamportClock != other.lamportClock)
            return lamportClock < other.lamportClock;
        return process < other.process;
    }
};

int main()
{
    map<string, Process> processes;
    vector<Message> messagePool;
    vector<Event> eventLog;

    // Read input from file
    string line;
    string currentProcess;

    ifstream inputFile("input.txt");
    if (!inputFile)
    {
        cerr << "Cannot open input.txt\n";
        return 1;
    }

    while (getline(inputFile, line))
    {
        if (line.empty())
            continue;

        istringstream iss(line);
        string first;
        iss >> first;

        if (first == "begin")
        {
            string word1, pname;
            iss >> word1 >> pname;
            currentProcess = pname;
            processes[pname].name = pname;
            processes[pname].operationIndex = 0;
            processes[pname].lamportClock = 0;
            processes[pname].waiting = false;
        }
        else if (first == "end")
        {
            currentProcess = "";
        }
        else if (first == "send")
        {
            string target, msg;
            iss >> target >> msg;
            Operation op;
            op.type = "send";
            op.target = target;
            op.message = msg;
            processes[currentProcess].operations.push_back(op);
        }
        else if (first == "recv")
        {
            string target, msg;
            iss >> target >> msg;
            Operation op;
            op.type = "recv";
            op.target = target;
            op.message = msg;
            processes[currentProcess].operations.push_back(op);
        }
        else if (first == "print")
        {
            string msg;
            iss >> msg;
            Operation op;
            op.type = "print";
            op.message = msg;
            processes[currentProcess].operations.push_back(op);
        }
    }

    // Simulation
    int totalEvents = 0;
    int maxIterations = 10000;
    int iterations = 0;

    while (iterations < maxIterations)
    {
        iterations++;
        bool anyProgress = false;

        // Try each process
        for (auto &p : processes)
        {
            string pname = p.first;
            Process &process = p.second;

            if (process.operationIndex >= process.operations.size())
            {
                continue; // Process finished
            }

            if (process.waiting)
            {
                // Check if message arrived
                bool found = false;
                for (auto &msg : messagePool)
                {
                    if (msg.sender == process.waitingFromProcess &&
                        msg.receiver == pname &&
                        msg.payload == process.waitingForMsg &&
                        !msg.delivered)
                    {
                        found = true;
                        msg.delivered = true;

                        // Update Lamport clock
                        process.lamportClock++;

                        Event evt;
                        evt.type = "received";
                        evt.process = pname;
                        evt.message = process.waitingForMsg;
                        evt.otherProcess = process.waitingFromProcess;
                        evt.lamportClock = process.lamportClock;
                        eventLog.push_back(evt);

                        process.waiting = false;
                        process.operationIndex++;
                        anyProgress = true;
                        totalEvents++;
                        break;
                    }
                }
                continue;
            }

            Operation &op = process.operations[process.operationIndex];

            if (op.type == "send")
            {
                process.lamportClock++;

                Message msg;
                msg.sender = pname;
                msg.receiver = op.target;
                msg.payload = op.message;
                msg.delivered = false;
                messagePool.push_back(msg);

                Event evt;
                evt.type = "sent";
                evt.process = pname;
                evt.message = op.message;
                evt.otherProcess = op.target;
                evt.lamportClock = process.lamportClock;
                eventLog.push_back(evt);

                process.operationIndex++;
                anyProgress = true;
                totalEvents++;
            }
            else if (op.type == "recv")
            {
                // Check if message is available
                bool found = false;
                for (auto &msg : messagePool)
                {
                    if (msg.sender == op.target &&
                        msg.receiver == pname &&
                        msg.payload == op.message &&
                        !msg.delivered)
                    {
                        found = true;
                        msg.delivered = true;

                        process.lamportClock++;

                        Event evt;
                        evt.type = "received";
                        evt.process = pname;
                        evt.message = op.message;
                        evt.otherProcess = op.target;
                        evt.lamportClock = process.lamportClock;
                        eventLog.push_back(evt);

                        process.operationIndex++;
                        anyProgress = true;
                        totalEvents++;
                        break;
                    }
                }

                if (!found)
                {
                    // Block waiting for message
                    process.waiting = true;
                    process.waitingForMsg = op.message;
                    process.waitingFromProcess = op.target;
                }
            }
            else if (op.type == "print")
            {
                process.lamportClock++;

                Event evt;
                evt.type = "printed";
                evt.process = pname;
                evt.message = op.message;
                evt.lamportClock = process.lamportClock;
                eventLog.push_back(evt);

                process.operationIndex++;
                anyProgress = true;
                totalEvents++;
            }
        }

        // Check for deadlock (outside the process loop)
        bool allFinished = true;
        bool allWaiting = true;

        for (auto &p : processes)
        {
            string pname = p.first;
            Process &process = p.second;
            if (process.operationIndex < process.operations.size())
            {
                allFinished = false;
                if (!process.waiting)
                {
                    allWaiting = false;
                }
            }
        }

        if (!allFinished && allWaiting)
        {
            cout << "system deadlocked\n";
            break;
        }

        if (allFinished || !anyProgress)
        {
            break;
        }
    }

    // Sort events by Lamport clock
    sort(eventLog.begin(), eventLog.end());

    // Output events
    for (const auto &evt : eventLog)
    {
        if (evt.type == "sent")
        {
            cout << "sent " << evt.process << " " << evt.message << " "
                 << evt.otherProcess << " (" << evt.lamportClock << ")\n";
        }
        else if (evt.type == "received")
        {
            cout << "received " << evt.process << " " << evt.message << " "
                 << evt.otherProcess << " (" << evt.lamportClock << ")\n";
        }
        else if (evt.type == "printed")
        {
            cout << "printed " << evt.process << " " << evt.message << " ("
                 << evt.lamportClock << ")\n";
        }
    }

    // Report undelivered messages
    for (const auto &msg : messagePool)
    {
        if (!msg.delivered)
        {
            cout << "message " << msg.payload << " (" << msg.sender << " -> "
                 << msg.receiver << ") not delivered\n";
        }
    }

    return 0;
}
