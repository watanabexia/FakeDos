

#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <mem.h>
#include "dist/json/json.h"
#include <iostream>
#include <sstream>
using namespace std;

void init_mem(string jsonmem){
    Json::Value root, user, taskmem;

    root["user"] = user;
    root["taskmem"] = taskmem;

    jsonmem = root.toStyledString(); 
}

void user_mem_alloc(string username, string jsonmem){
    Json::Reader reader;
    Json::Value root;
    
    if (reader.parse(jsonmem, root)){
        root["user"][username] = 0x1000;  //  0x1000 represent OS memory usage
    }
    jsonmem = root.toStyledString(); 
}

void task_mem(int taskid, int taskmem, string username, string jsonmem){
    Json::Reader reader;
    Json::Value root;

    string id = to_string(taskid);

    if (reader.parse(jsonmem, root)){
        if (!root.isMember(id)){
            root["taskmem"][id] = taskmem;
        }
        else {
            root["taskmem"][id] = root["taskmem"][id].asInt() + taskmem;
        }
        root["user"][username] = root["user"][username].asInt() + taskmem;
    }
    jsonmem = root.toStyledString(); 
}

template <typename ValueType>
bool task_data_write(int taskid, string key, ValueType value, string jsonmem){
    Json::Reader reader;
    Json::Value root;

    string id = to_string(taskid);
    int datamem = sizeof(value);
    if (limit_check(datamem)) return false;
    if (reader.parse(jsonmem, root)){
        root[id][key] = value;
        task_mem(taskid, datamem);
    }
    jsonmem = root.toStyledString(); 
    return true;
}

template <typename ValueType>
ValueType task_data_read(int taskid, string key, string jsonmem){
    Json::Reader reader;
    Json::Value root;

    string id = to_string(taskid);

    if (reader.parse(jsonmem, root)){
        return root[id][key];
    }
}


void user_mem_free(string username, string jsonmem){
    Json::Reader reader;
    Json::Value root, user;
    
    if (reader.parse(jsonmem, root)){
        root["user"][username] = 0;
    }
    jsonmem = root.toStyledString();
    ///////////////////////////////////////////////
    // need to kill all task memory under this user
    ///////////////////////////////////////////////



    
}

void task_mem_free(int taskid, string username, string jsonmem){
    Json::Reader reader;
    Json::Value root;
    string id = to_string(taskid);

    if (reader.parse(jsonmem, root)){
        root.removeMember(id);
        int mem = root["taskmem"][id].asInt();
        root["taskmem"][id] = 0;
        root["user"][username] = root["user"][username].asInt() - mem;
    }   
    jsonmem = root.toStyledString();
}

bool limit_check(int mem, vector <string> user_name, string jsonmem){
    Json::Reader reader;
    Json::Value root;

    int usage = 0;
    if (reader.parse(jsonmem, root)){
        for (int i = 0; i < root["user"].size(); i++){
            usage += root["user"][user_name[i]].asInt();
        }
    }
    if ((usage + mem) >= 0xFFFF){
        return false;
    }else return true;
}

int get_task_mem(int taskid, string jsonmem){
    Json::Reader reader;
    Json::Value root;
    // cout << jsonmem;

    if (reader.parse(jsonmem, root)){
        return root["taskmem"][taskid].asInt();
    }
}

int get_user_mem(string username, string jsonmem){
    Json::Reader reader;
    Json::Value root;
    // cout << jsonmem;

    if (reader.parse(jsonmem, root)){
        return root["user"][username].asInt();
    }
}