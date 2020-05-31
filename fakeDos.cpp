#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstdlib>
#include <unistd.h>
#include <string>
#include <map>
#include <vector>
#include <io.h>
#include <direct.h>
#include <dist/json/json.h>
#include <fakeDos.h>
#include <applications.h>
#include <userManagement.h>
#include <fileManagement.h>
#include <applicationManagement.h>
#include <taskManagement.h>
#include <mem.h>
#include <app_TextEditor.h>
using namespace std;

string current_user = "NULL";
string current_path;

string command;

string fakeDosFolderPath;
string usersFilePath;

string jsonmem;

vector<string> operation_list{ // the list of all available operations
    "exit",
    "create_user",
    "log_in",
    "change_user",
    "ls_u",
    "ls_lu",
    "log_out",
    "delete_user",
    "help",

 // File management commands
    "make_dir",
    "make_file",
    "del_dir",
    "del_file",

    "read",
    "write",

    "copy",
    "move",

    "show_content",
    "change_path",

 // App management commands
    "install",
    "appstore",
    "run",

 // Task management commands
    "list_task",
    "wake_up_task",
    "kill_task"

};

map<string, string> operation_syntax{ // the syntax for all operations
    {"help", "help"},
    {"create_user", "create_user (user name) (password)"},
    {"delete_user", "delete_user (user name)"},
    {"change_user", "change_user"},
    {"log_in", "log_in (user_name) (password)"},
    {"log_out", "log_out"},
    {"ls_u", "ls_u"},
    {"ls_lu", "ls_lu"},
    {"exit", "exit"},
    {"make_dir", "make_dir (name)"},
    {"make_file", "make_file (name)"},
    {"del_dir", "del_dir (name)"},
    {"del_file", "del_file (name)"},
    {"read", "read (name)"},
    {"write", "write (name)"},
    {"copy","copy (name1) (name/path2) "},
    {"move","move (name1) (name/path2) "},
    {"show_content", "show_content"},
    {"change_path", "change_path (name)"},
    {"run", "run (app name).app"},
    {"ls_t", "ls_t"},
    {"ls_m", "ls_m"},
    {"install", "install (app name)"},
    {"appstore", "appstore"},
    {"list_task","list_task"},
    {"wake_up_task","wake_up_task (taskID)"},
    {"kill_task","kill_task (taskID)"}

};

map<string, string> operation_description{ // the function description of all operations
    {"help", "See description and syntax for all operations."},
    {"create_user", "Create a new user."},
    {"delete_user", "Delete a user."},
    {"change_user", "Go back to the select user panel without logging out from the current user."},
    {"log_in", "Log in with the identity of a user."},
    {"log_out", "Log out from the current user."},
    {"ls_u", "Show all users that exist."},
    {"ls_lu", "Show all logged in users."},
    {"exit", "Shutdown FakeDos."},

    {"make_dir", "Create a new directory under current route."},
    {"make_file", "Create a new file under current route. User should specify the file type, e.g: .txt"},
    {"del_dir", "Delete a directory under current route."},
    {"del_file", "Delete a file under current route. User should specify the file type, e.g: .txt"},
    {"read","Read a text file."},
    {"write", "Write in a text file."},
    {"copy","Copy file/dir 1 to dir 2. When copying file, you need to specify its file type."},
    {"move","Move file/dir 1 to dir 2. When moving file, you need to specify its file type."},
    {"show_content", "Show the content of current directory."},
    {"change_path", "Change current path to a specific directory. "
        "You can go to upper-level directory by entering 'u', "
        "and go to root directory by entering 'rt\'."},

    {"run", "Run apps."},

    {"install", "Install the specific app under the current directory."},
    {"appstore", "Go to App Store and see all available apps."},

    {"list_task","Display the information of task in memory."},
    {"wake_up_task","Wake up a task to turn it to ready state from block state."},
    {"kill_task","Kill a task."}

};

vector<string> app_list{
    "guessGame.app",
    "textEditor.app"
};

map<string, string> app_description{
    {"guessGame.app", "A number guessing game."},
    {"textEditor.app", "A textEditor."}
};

vector<string> user_name;

map<string, string> user_password;

map<string, string> user_route;

bool exitable = false;

bool is_logged_in = false;

vector<string> logged_in_users;

vector<PCB_type> runningQueue;
vector<PCB_type> blockQueue;
vector<PCB_type> readyQueue;

struct PCB_type mem[100]; //a list stands for memory

vector<string> command_split(string command) {

    vector<string> command_splited;
    size_t next_space_pos = command.find(' ');
    while (next_space_pos != string::npos) {
        command_splited.push_back(command.substr(0, next_space_pos));
        command.erase(0, next_space_pos + 1);
        next_space_pos = command.find(' ');
    }

    command_splited.push_back(command);

    return command_splited;
}

void read_users() {
    ifstream ifs;
    ifs.open((usersFilePath).c_str());
    string line;
    string username;
    bool is_username = true;
    bool is_password = false;
    bool is_route = false;

    while (getline(ifs, line)) {
        if (is_username) {
            user_name.push_back(line);
            username = line;
            is_username = false;
            is_password = true;

        } else if (is_password){
            user_password[username] = line;
            is_password = false;
            is_route = true;
        } else if (is_route){
            user_route[username] = line;
            is_route = false;
            is_username = true;
        }
    }
    ifs.close();
}

void write_users() {
    ofstream ofs;
    ofs.open((usersFilePath).c_str());
    for (int i = 0; i < int(user_name.size()); i++) {
        string username = user_name[i];
        string password = user_password[username];


        string userRoute = user_route[username];


        for (int j = 0; j < int(username.size()); j++) {
            ofs.put(username[j]);
        }
        ofs.put('\n');

        for (int j = 0; j < int(password.size()); j++) {
            ofs.put(password[j]);
        }
        ofs.put('\n');

        for (int j = 0; j < int(userRoute.size()); j++) {
            ofs.put(userRoute[j]);
        }
        ofs.put('\n');
    }
    ofs.close();
}

void fakeDosPre() {
    char *buffer;
    buffer = getcwd(NULL, 0);
    string route(buffer);

    if ((_access("fakeDos", 0)) == -1) {
        system("mkdir fakeDos");
        system("mkdir fakeDos\\system");
        system("mkdir fakeDos\\users");
        system("mkdir fakeDos\\files");
    }

    fakeDosFolderPath = route + "\\fakeDos";
    usersFilePath = fakeDosFolderPath + "\\system\\users.txt";

    if ((_access(usersFilePath.c_str(), 0) == -1)) {
        system(("type nul>" + usersFilePath).c_str());
    } else {
        read_users();
    }

    init_mem(jsonmem);
}

void help() {
    for (int i = 0; i < int(operation_list.size()); i++) {
        string op = operation_list[i];
        cout << op << ": " << operation_syntax[op] << endl;
        cout << "\t" << operation_description[op] << endl;
    }
}

void exit() {
    cout << "System shutting down..." << endl << "See you next time!" << endl;
}

void run(string appname) {

    string current_route = route_formating(fakeDosFolderPath, user_route[current_user]);
    vector<string> fileNames = getFiles(current_route + "\\*");
    if (!find(fileNames,appname)){
        cout << "Error: App not found." << endl;
        return;
    } else {
        int taskid = create_task(current_user, appname, jsonmem, mem, readyQueue, runningQueue);

        if (appname == "guessGame.app") {
            guessGame(taskid, true, current_user, user_name, jsonmem,
                      mem, runningQueue, blockQueue, readyQueue);
        }

        if (appname == "textEditor.app") {
            TextEditor(taskid, false, jsonmem, current_user, user_name, mem,
                       runningQueue, blockQueue, readyQueue);
        }
    }
}
void fakeDos() { // fakeDos main process
    cout << "fakeDos Prototype" << endl;
    cout << "System booting..." << endl;

    fakeDosPre();

    cout << "Welcome to fakeDos! Type help for more information." << endl;

    while (exitable == false) {

        if (is_logged_in == false) {
            cout << "Please log in with the following command: " << endl;
            cout << "\tlog_in (username) (password)" << endl;

            if (user_name.size() == 0) {
                cout << "No valid user is found. Please create a user with the following command: " << endl;
                cout << "\tcreate_user (user name) (password)" << endl;
            }
        } else {

            current_path = user_route[current_user];
            cout << current_user << "@" << current_path << " $";
        }

        getline(cin, command); // read the command as a whole line

        vector<string> command_splited = command_split(command); // split the command by blank space
        string operation = command_splited[0];

        vector<string>::iterator it = find(operation_list.begin(), operation_list.end(), operation); // see if the command exists

        if (it != operation_list.end()) {

            if (operation == "help") {
                help();
            }

            if (operation == "create_user") {
                create_user(command_splited, user_name, user_password, user_route, fakeDosFolderPath);
                write_users();
            }

            if (operation == "delete_user") {
                delete_user(command_splited,
                            user_name,
                            user_password,
                            user_route,
                            fakeDosFolderPath,
                            logged_in_users,
                            current_user,
                            jsonmem,
                            mem,
                            runningQueue,
                            blockQueue,
                            readyQueue);
                write_users();
            }

            if (operation == "log_in") {
                log_in(command_splited, user_name, user_password, user_route, is_logged_in, current_user, current_path, logged_in_users, jsonmem);
            }

            if (operation == "ls_lu") {
                ls_lu(logged_in_users);
            }

            if (operation == "ls_u") {
                ls_u(user_name);
            }

            if (operation == "exit") {
                write_users();
                exit();
                exitable = true;
                break;
            }

            if(operation == "kill_task"){

                if (command_splited.size() < 2) {
                    cout << "Error: Invalid input syntax." << endl;
                } else {

                    string taskid = command_splited[1];

                    if (taskid.size() <= 10) {
                        if (stoll(taskid) < 2147483646) {
                            kill(current_user, stoi(taskid), jsonmem,
                                 mem, runningQueue, blockQueue, readyQueue);
                        } else {
                            cout << "Error: task id can't be that large." << endl;
                        }
                    } else {
                        cout << "Error: task id can't be that large." << endl;
                    }
                }

            }

            if(operation == "list_task"){
                display(logged_in_users, jsonmem, mem, runningQueue, blockQueue, readyQueue);
            }

            if (is_logged_in) {

                if (operation == "change_user") {
                    change_user(is_logged_in, current_user);
                }

                if (operation == "log_out") {
                    log_out(is_logged_in, current_user, logged_in_users,
                            jsonmem, mem,
                            runningQueue, blockQueue,
                            readyQueue);
                }

                if (operation == "make_dir"){

                    if (command_splited.size()<2){
                        cout << "Error: Invalid input syntax."<<endl;
                    } else{
                        string name = command_splited[1];
                        make_dir(name, fakeDosFolderPath, current_user, user_route);
                    }
                }

                if (operation == "make_file"){
                    if (command_splited.size()<2){
                        cout << "Error: Invalid input syntax."<<endl;
                    } else {
                        string name = command_splited[1];
                        make_file(name,fakeDosFolderPath,current_user,user_route);
                    }
                }

                if (operation == "del_dir"){
                    if (command_splited.size()<2){
                        cout << "Error: Invalid input syntax."<<endl;
                    } else {
                        string name = command_splited[1];
                        del_dir(name,fakeDosFolderPath, current_user,user_route);
                    }

                }

                if (operation == "del_file"){
                    if (command_splited.size()<2){
                        cout << "Error: Invalid input syntax."<<endl;
                    } else {
                        string name = command_splited[1];
                        del_file(name,fakeDosFolderPath,current_user,user_route);
                    }

                }

                if (operation == "read"){
                    if (command_splited.size()<2){
                        cout << "Error: Invalid input syntax."<<endl;
                    } else {
                        string name = command_splited[1];
                        read(name,fakeDosFolderPath,current_user,user_route);
                    }
                }

                if (operation == "write"){
                    if (command_splited.size()<2){
                        cout << "Error: Invalid input syntax."<<endl;
                    } else {
                        string name = command_splited[1];
                        write(name,fakeDosFolderPath,current_user,user_route);
                    }
                }

                if (operation == "copy"){
                    if (command_splited.size()<3){
                        cout << "Error: Invalid input syntax."<<endl;
                    } else {
                        string this_name = command_splited[1];
                        string taregt_name = command_splited[2];
                        copy(this_name,taregt_name,fakeDosFolderPath,current_user,user_route);
                    }
                }

                if (operation == "move"){
                    if (command_splited.size()<3){
                        cout << "Error: Invalid input syntax."<<endl;
                    } else {
                        string this_name = command_splited[1];
                        string taregt_name = command_splited[2];
                        move(this_name,taregt_name,fakeDosFolderPath,current_user,user_route);
                    }
                }

                if (operation == "show_content"){
                    show_content(fakeDosFolderPath,current_user,user_route);
                }

                if (operation == "change_path"){
                    if (command_splited.size()<2){
                        cout << "Error: Invalid input syntax."<<endl;
                    } else {
                        string name = command_splited[1];
                        change_path(name,fakeDosFolderPath,current_user,user_route);
                    }
                }

                if (operation == "run") {
                    if (command_splited.size()<2){
                        cout << "Error: Invalid input syntax."<<endl;
                    } else {
                        string appname = command_splited[1];
                        run(appname);
                    }
                }

                if (operation == "appstore") {
                    show_apps(app_list, app_description);
                }

                if (operation == "install") {
                    if (command_splited.size() < 2) {
                        cout << "Error: Invalid input syntax." << endl;
                    } else {
                        string appname = command_splited[1];
                        install(appname, app_list, fakeDosFolderPath, current_user, user_route);
                    }
                }

                if (operation == "wake_up_task"){
                    if (command_splited.size() < 2) {
                        cout << "Error: Invalid input syntax." << endl;
                    } else {
                        string taskid = command_splited[1];

                        if (taskid.size() <= 10) {
                            if (stoll(taskid) < 2147483646) {

                                if (wake_up(current_user, stoi(taskid), mem, runningQueue, blockQueue, readyQueue)) {

                                    string appname;

                                    for (int i = 0; i<100; i++) {
                                        if (mem[i].id == stoi(taskid)) {
                                            appname = mem[i].app_name;
                                            break;
                                        }
                                    }

                                    if (appname == "guessGame.app") {
                                        guessGame(stoi(taskid), false, current_user, user_name, jsonmem, mem,
                                                  runningQueue, blockQueue, readyQueue);
                                    }

                                    if (appname == "textEditor.app") {
                                        TextEditor(stoi(taskid), true, jsonmem, current_user, user_name, mem,
                                                   runningQueue, blockQueue, readyQueue);
                                    }
                                }

                            } else {
                                cout << "Error: task id can't be that large." << endl;
                            }
                        } else {
                            cout << "Error: task id can't be that large." << endl;
                        }
                    }
                }
            }

        } else {
            cout << "Invalid command: " << operation << endl;
        }

    }

}
