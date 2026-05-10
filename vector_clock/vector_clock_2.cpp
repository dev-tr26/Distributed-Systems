#include<bits/stdc++.h>
using namespace std;

struct Operation {
    string type;
    string target;
    string message;
};

struct Message {
    string from, to, message;
    vector<int> vclock;
};

struct Process {
    string name;
    vector<Operation> ops;
    int pc = 0;
    vector<int> vclock;
    bool blocked = false;
};

string vcToString(const vector<int>& vc) {
    string s = "[";
    for (int i = 0; i < (int)vc.size(); i++) {
        s += to_string(vc[i]);
        if (i + 1 < (int)vc.size()) s += ",";
    }
    s += "]";
    return s;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    vector<Process> processes;
    unordered_map<string, int> pid;

    string line;
    while (getline(cin, line)) {
        if (line.find("begin process") != string::npos) {
            Process p;
            string tmp;
            stringstream ss(line);
            ss >> tmp >> tmp >> p.name;
            pid[p.name] = processes.size();

            while (getline(cin, line)) {
                if (line.find("end process") != string::npos)
                    break;

                stringstream os(line);
                Operation op;
                os >> op.type;

                if (op.type == "send" || op.type == "recv") {
                    os >> op.target >> op.message;
                } else if (op.type == "print") {
                    os >> op.message;
                }
                p.ops.push_back(op);
            }
            processes.push_back(p);
        }
    }

    int n = processes.size();
    for (auto &p : processes)
        p.vclock.assign(n, 0);

    vector<Message> messagePool;
    vector<string> log;

    // -------- EXECUTION --------
    while (true) {
        bool progress = false;

        for (int i = 0; i < n; i++) {
            auto &p = processes[i];
            if (p.pc >= (int)p.ops.size()) continue;

            auto &op = p.ops[p.pc];

            // SEND
            if (op.type == "send") {
                p.vclock[i]++;

                messagePool.push_back({
                    p.name,
                    op.target,
                    op.message,
                    p.vclock
                });

                log.push_back("sent " + p.name + " " + op.message +
                              " " + op.target + " " +
                              vcToString(p.vclock));

                p.pc++;
                p.blocked = false;
                progress = true;
            }

            // PRINT
            else if (op.type == "print") {
                p.vclock[i]++;

                log.push_back("printed " + p.name + " " +
                              op.message + " " +
                              vcToString(p.vclock));

                p.pc++;
                p.blocked = false;
                progress = true;
            }

            // RECEIVE
            else if (op.type == "recv") {
                bool found = false;

                for (auto it = messagePool.begin(); it != messagePool.end(); ++it) {
                    if (it->from == op.target &&
                        it->to == p.name &&
                        it->message == op.message) {

                        for (int k = 0; k < n; k++)
                            p.vclock[k] = max(p.vclock[k], it->vclock[k]);

                        p.vclock[i]++;

                        log.push_back("received " + p.name + " " +
                                      op.message + " " + it->from +
                                      " " + vcToString(p.vclock));

                        messagePool.erase(it);
                        p.pc++;
                        p.blocked = false;
                        progress = true;
                        found = true;
                        break;
                    }
                }
                p.blocked = !found;
            }
        }

        // -------- TERMINATION CHECK --------
        if (!progress) {
            bool anyBlocked = false;

            for (auto &p : processes) {
                if (p.pc < (int)p.ops.size() && p.blocked)
                    anyBlocked = true;
            }

            // Print execution log
            for (auto &e : log)
                cout << e << "\n";

            if (anyBlocked) {
                cout << "system deadlocked\n";
            }

            // Report undelivered messages
            for (auto &m : messagePool) {
                cout << "message " << m.message << " ("
                     << m.from << " -> " << m.to
                     << ") not delivered\n";
            }

            return 0;
        }
    }
}
