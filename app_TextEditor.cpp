/*
 * editor.cpp: This file implements the methods
 * and functions in editorBuffer.h
 */
#include "app_TextEditor.h"
#include <taskManagement.h>
#include "mem.h"
#include <iostream>
#include <iomanip>
#include <string>
using namespace std;

void toUpperCaseInPlace(string& str) {
    int nChars = str.length();
    for (int i = 0; i < nChars; i++) {
        str[i] = toupper(str[i]);
    }
}

string toUpperCase(const string& str) {
    string str2 = str;
    toUpperCaseInPlace(str2);
    return str2;
}

EditorBuffer::EditorBuffer(){
   start = cursor = new Cell;
   start->link = NULL;
}

EditorBuffer::~EditorBuffer(){
    Cell *cp = start;
    while(cp != NULL){
        Cell *next = cp->link;
        delete cp;
        cp = next;
    }
}

void EditorBuffer::moveCursorForward(){
    if (cursor->link !=NULL){
        cursor = cursor->link;
    }
}

void EditorBuffer::moveCursorBackward(){
    Cell *cp = start;
    if (cursor != start){
        while(cp->link != cursor){
            cp = cp->link;
        }
        cursor = cp;
    }
}

void EditorBuffer::moveCursorToStart(){
    cursor = start;
}

void EditorBuffer::moveCursorToEnd(){
    while (cursor->link != NULL){
        cursor = cursor->link;
    }
}

void EditorBuffer::insertCharacter(char ch){
    Cell *cp = new Cell;
    cp->ch = ch;
    cp->link = cursor->link;
    cursor->link = cp;
    cursor = cp;
}

void EditorBuffer::deleteCharter(){
    if (cursor->link != NULL){
        Cell *oldCell = cursor->link;
        cursor->link = cursor->link->link;
        delete oldCell;
    }
}

void EditorBuffer::showContents(){
    for (Cell *cp = start->link; cp!=NULL; cp = cp->link){
        cout << ' '<< cp->ch;
    }
    cout << endl;
    for (Cell *cp = start; cp != cursor; cp = cp->link){
        cout << "  ";
    }
    cout << '^'<< endl;
}

bool EditorBuffer::HangOnEditor(int taskid, vector <string> user_name, string username, string & jsonmem,
                                PCB_type (&mem)[100],
                                vector<PCB_type> & runningQueue,
                                vector<PCB_type> & blockQueue,
                                vector<PCB_type> & readyQueue){
    int textsize = 0;
    bool flag = true;
    for (Cell *cp = start->link; cp!=NULL; cp = cp->link){
        textsize++;
        string key = "text" + to_string(textsize);
        string x;
        x = cp->ch;
        flag = task_data_write(taskid, key, x, user_name, username, jsonmem);
        if (!flag) break;
    }

    flag = task_data_write(taskid, "textsize", to_string(textsize), user_name, username, jsonmem);

    if (flag) {
        cout << "Hangon succeeded!" << endl;

        block(username, taskid, mem,
              runningQueue, blockQueue, readyQueue);

        return true;
    } else{

        kill(username, taskid, jsonmem,
             mem, runningQueue, blockQueue, readyQueue);

        cout << "Hangon failed! Lack of memory space" << endl;
        return false;
    }
}

void editorHelp(){
    cout << endl;
    cout << "+--------------------------------------------------------------+"<<endl;
    cout << "|                   Commands for text editor                   |"<< endl;
    cout << "+--------------------------------------------------------------+"<<endl;

    cout << "|"<< right << setw(7)<<" "<< left<<
            setw(55)<< " (Following commands are case-insensitive) "<<"|"<<endl;

    cout << "|"<< right << setw(7)<<" "<< left<< setw(55)<< " "<<"|"<<endl;

    cout << "|"<< right << setw(7)<<"Itext:"<< left<<
            setw(55)<< " Inserts the text following 'I' into the buffer "<<"|"<<endl;

    cout << "|"<< right<< setw(7)<<"J:"<< left<<
            setw(55)<< " Jumps the current point (the cursor) to the beginning"<<"|"<<endl;

    cout << "|"<< right<< setw(7)<<"E:"<< left<<
            setw(55)<< " Moves the cursor to the end of the buffer"<<"|"<<endl;

    cout << "|"<< right<< setw(7)<<"F:"<< left<<
            setw(55)<< " Moves the cursor forward one character"<<"|"<<endl;

    cout << "|"<< right<< setw(7)<<"B:"<< left<<
            setw(55)<< " Moves the cursor backward one character"<<"|"<<endl;

    cout << "|"<< right<< setw(7)<<"D:"<< left<<
            setw(55)<< " Deletes the character after the cursor"<<"|"<<endl;

    cout << "|"<< right<< setw(7)<<"H:"<< left<<
            setw(55)<< " Prints a help message listing the commands"<<"|"<<endl;

    cout << "|"<< right<< setw(7)<<"Q:"<< left<<
            setw(55)<< " Quits from the editor"<<"|"<<endl;
    
    cout << "|"<< right<< setw(7)<<"L:"<< left<<
            setw(55)<< " Hang On the editor task and jump to next task"<<"|"<<endl;

    cout << "+--------------------------------------------------------------+"<<endl;
    cout << endl;

}

void editText(EditorBuffer &editor, vector <string> user_name, string username, string & jsonmem, int taskid,
              PCB_type (&mem)[100],
              vector<PCB_type> & runningQueue,
              vector<PCB_type> & blockQueue,
              vector<PCB_type> & readyQueue){
    string command;
    editorHelp();

    editor.showContents();

    while (true){
        cout << "Please enter your command for text editing: ";
        getline(cin, command);
        if (command.length()==1){
            command = toUpperCase(command);
            if (command == "J") {
                editor.moveCursorToStart();
                editor.showContents();

            }
            if (command == "E"){
                editor.moveCursorToEnd();
                editor.showContents();
            }
            if (command == "F"){
                editor.moveCursorForward();
                editor.showContents();
            }
            if (command == "B"){
                editor.moveCursorBackward();
                editor.showContents();
            }
            if (command == "D"){
                editor.deleteCharter();
                editor.showContents();
            }
            if (command == "H"){
                editorHelp();
            }
            if (command == "Q"){
                cout << "Texteditor killed" <<endl;

                kill(username, taskid, jsonmem,
                     mem, runningQueue, blockQueue, readyQueue);

                return;
            }
            if (command == "L"){
                editor.HangOnEditor(taskid, user_name, username, jsonmem, mem,
                                    runningQueue, blockQueue, readyQueue);
                return;
            }
        }

        else if (command[0]=='I'){
            command = command.c_str();
            for (int i = 1; i < int(command.length()); i++){
                editor.insertCharacter(command[i]);
            }
            editor.showContents();
        }

        else cout << "Invalid Command (Enter H for help)"<<endl;
    }

}


void TextEditor(int taskid, bool wakeup, string & jsonmem,
                string username, vector<string> user_name, PCB_type (&mem)[100],
                vector<PCB_type> & runningQueue,
                vector<PCB_type> & blockQueue,
                vector<PCB_type> & readyQueue){

    EditorBuffer editor;

    if (wakeup){
        int textsize = stoi(task_data_read(taskid, "textsize", jsonmem));
        for (int i = 1; i <= textsize; i++){
            string key = "text" + to_string(i);
            string data = task_data_read(taskid, key, jsonmem);
            data = data.c_str();
            editor.insertCharacter(data[0]);
        }
    }

    editText(editor, user_name, username, jsonmem,
             taskid, mem, runningQueue, blockQueue, readyQueue);
}
