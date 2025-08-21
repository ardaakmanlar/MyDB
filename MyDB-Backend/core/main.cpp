#include "Table.h"
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <string>
#include <vector>

using namespace std;

static void printBanner() {
    cout << R"(

                   __  __       ____  ____  
                  |  \/  |     |  _ \| __ ) 
                  | |\/| |_____| | | |  _ \ 
                  | |  | |_____| |_| | |_) |
                  |_|  |_|     |____/|____/ 

                          MyDB
)" << "\n";
}

int main() {
    printBanner();

    unordered_map<string, Table*> tables;
    Table* current = nullptr;
    string currentName;

    cout << "Welcome to MyDB CLI. Type 'help' for commands.\n";

    string line;
    while (true) {
        if (current) cout << ">" << currentName << "> ";
        else         cout << "> ";

        if (!getline(cin, line)) break;
        if (line.empty()) continue;

        stringstream ss(line);
        string cmd;
        ss >> cmd;

        if (cmd == "exit") break;

        else if (cmd == "help") {
            cout << "Commands:\n"
                << " create <name>              - create a new table\n"
                << " use <name>                 - switch active table\n"
                << " drop <name>                - delete a table\n"
                << " tables                     - list tables\n"
                << " insert <v1> <v2> ...       - insert row (auto id)\n"
                << " insertid <id> <v1> ...     - insert row with id\n"
                << " update <id> <v1> ...       - update row by id\n"
                << " delete <id>                - delete row by id\n"
                << " print [n]                  - print first n rows (all if omitted)\n"
                << " exit                       - quit\n";
        }

        else if (cmd == "create") {
            string name; ss >> name;
            if (name.empty()) { cout << "Provide a table name.\n"; continue; }
            if (tables.count(name)) { cout << "Table already exists.\n"; continue; }

            vector<string> attrs;
            cout << "Creating table '" << name << "'.\n";
            cout << "Enter attribute names. Type 'done' to finish.\n";
            while (true) {
                cout << "attr> ";
                string aname;
                if (!getline(cin, aname)) aname.clear();
                if (aname == "done") break;
                if (!aname.empty()) {
                    attrs.push_back(aname);
                    cout << "Attribute added: " << aname << "\n";
                }
            }
            tables[name] = new Table(name, attrs);
            cout << "Table '" << name << "' created with "
                << attrs.size() << " attributes.\n";
        }

        else if (cmd == "use") {
            string name; ss >> name;
            auto it = tables.find(name);
            if (it == tables.end()) { cout << "No such table: " << name << "\n"; continue; }
            current = it->second;
            currentName = name;
            cout << "Now using table '" << name << "'.\n";
        }

        else if (cmd == "drop") {
            string name; ss >> name;
            auto it = tables.find(name);
            if (it == tables.end()) { cout << "No such table: " << name << "\n"; continue; }
            Table* victim = it->second;
            if (current == victim) { current = nullptr; currentName.clear(); }
            delete victim;
            tables.erase(it);
            cout << "Table '" << name << "' dropped.\n";
        }

        else if (cmd == "tables") {
            if (tables.empty()) { cout << "(no tables)\n"; continue; }
            cout << "Tables:\n";
            for (auto& kv : tables) cout << " - " << kv.first << "\n";
        }

        else {
            if (!current) { cout << "No active table. Use 'create' or 'use'.\n"; continue; }

            if (cmd == "insert") {
                vector<string> row; string v;
                while (ss >> v) row.push_back(v);
                if (current->addRow(row))
                    cout << "Row inserted.\n";
                else
                    cout << "Insert failed.\n";
            }

            else if (cmd == "insertid") {
                int id; ss >> id;
                if (!ss) { cout << "Provide an id.\n"; continue; }
                vector<string> row; string v;
                while (ss >> v) row.push_back(v);
                if (current->addRow(id, row))
                    cout << "Row inserted with id " << id << ".\n";
                else
                    cout << "Insert failed: id " << id << " already exists.\n";
            }
            else if (cmd == "update") {
                int id; ss >> id;
                if (!ss) { cout << "Provide an id.\n"; continue; }
                vector<string> row; string v;
                while (ss >> v) row.push_back(v);
                if (current->update(id, row))
                    cout << "Row " << id << " updated.\n";
                else
                    cout << "Update failed: no row with id " << id << ".\n";
            }

            else if (cmd == "delete") {
                int id; ss >> id;
                if (!ss) { cout << "Provide an id.\n"; continue; }
                if (current->deleteRow(id))
                    cout << "Row " << id << " deleted.\n";
                else
                    cout << "Delete failed: no row with id " << id << ".\n";
            }

            else if (cmd == "print") {
                int n = -1;
                if (ss >> n) current->printTable(n);
                else current->printTable();
            }

            else {
                cout << "Unknown command: " << cmd << "\n";
            }
        }
    }

    for (auto& kv : tables) delete kv.second;
    return 0;
}
