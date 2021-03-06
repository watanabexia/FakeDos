#include <iostream>
#include <string>
#include <random>
#include <vector>
#include "mem.h"
using namespace std;

#define TRUE 1
#define FALSE 0

#define NULL 0
#define RUNNING 1
#define BLOCK 2
#define READY 3

struct PCB_type
{
   int id;
   string user_name;
   string app_name;
   int state; /* 0 for not in memory, 1 for running, 2 for hang up */
};

vector<PCB_type> runningQueue;
vector<PCB_type> blockQueue;
vector<PCB_type> readyQueue;

struct PCB_type mem[100]; //a list stands for memory

void task_management()
{
   if (readyQueue.empty())
   {
      cout << "No tasks in the ready Queue." << endl;
      return;
   }

   else if (runningQueue.empty())
   {
      readyQueue[0].state = RUNNING;
      runningQueue.push_back(readyQueue[0]);
      readyQueue.erase(readyQueue.begin());
   }
   for (int i = 0; i < 100; i++)
   {
      if (runningQueue[0].id == mem[i].id)
      {
         mem[i].state = RUNNING;
      }
   }
}

int create_task(string userName, string appName, string jsonmem)
{
   int i, j, random_num;
   default_random_engine random;
   vector<string> usernames;

   random_num = random();
   for (j = 0; j < 100; j++)
      while (mem[j].id == random_num)
      {
         random_num = random();
      }
   for (i = 0; i < 100; i++)
      if (mem[i].state != NULL)
      {
         usernames.push_back(mem[i].user_name);
      }

   if (limit_check(0, usernames, jsonmem))
   {

   for (i = 0; i < 100; i++)
      if (mem[i].state == NULL)
         break;

   mem[i].id = random_num;
   mem[i].state = READY;
   mem[i].app_name = appName;
   mem[i].user_name = userName;
   readyQueue.push_back(mem[i]);

   cout << "The task is created successfully, task id:" << mem[i].id << endl;
   //print the task id
   task_management();

   return mem[i].id;
   }

   else
    {
        cout << "Task creation failed." << endl;
        return false;
    }
}

void display()
{
   int i;
   cout << "Number of tasks in memory: " << runningQueue.size() + readyQueue.size() + blockQueue.size() << endl;
   for (i = 0; i < 100; i++)
   {
      if (mem[i].state == RUNNING)
      {
         cout << "Task id =" << mem[i].id << " is running." << endl;
         cout << "User name:" << mem[i].user_name << endl;
         cout << "App name:" << mem[i].app_name << endl;
      }

      else if (mem[i].state == READY)
      {
         cout << "Task id =" << mem[i].id << " is ready." << endl;
         cout << "User name:" << mem[i].user_name << endl;
         cout << "App name:" << mem[i].app_name << endl;
      }

      else if (mem[i].state == BLOCK)
      {
         cout << "Task id =" << mem[i].id << " is blocked." << endl;
         cout << "User name:" << mem[i].user_name << endl;
         cout << "App name:" << mem[i].app_name << endl;
      }
   }
}

bool block(string userName, int id)
{
   int i;

   if (runningQueue.empty() && readyQueue.empty() && blockQueue.empty())
   {
      cout << "No task in the memory." << endl;
      return false;
   }

   else if (runningQueue.empty() && readyQueue.empty())
   {
      cout << "Every task is blocked." << endl;
      return false;
   }

   bool find = 0; //stands for whether task exists in memory

   for (i = 0; i < 100; i++)
   {
      if (id == mem[i].id)
      {
         find = 1;

         if (mem[i].user_name != userName)
         {
            cout << "You can only block your task." << endl;
            return false;
         }

         if (mem[i].state == NULL) //The task is already killed.
            cout << "The task does not exist." << endl;

         else if (mem[i].state == BLOCK) //The task has been already blocked.
            cout << "The task has been already blocked." << endl;

         else if (mem[i].state == RUNNING)
         {
            mem[i].state = BLOCK; //The task is blocked.
            cout << "The running task is blocked." << endl;
            blockQueue.push_back(mem[i]);
            vector<PCB_type>::iterator iter;
            for (iter = runningQueue.begin(); iter != runningQueue.end(); iter++)
            {
               if (iter->id == mem[i].id)
               {
                  runningQueue.erase(iter);
                  break;
               }
            }
         }

         else if (mem[i].state == READY)
         {
            mem[i].state = BLOCK; //The task is blocked.
            cout << "The ready task is blocked." << endl;
            blockQueue.push_back(mem[i]);
            vector<PCB_type>::iterator iter;
            for (iter = readyQueue.begin(); iter != readyQueue.end(); iter++)
            {
               if (iter->id == mem[i].id){
                  readyQueue.erase(iter);
                  break;
               }
            }
         }
      }
   }

   if (find == 0) //No such tasks in the memory
   {
      cout << "You entered a wrong id!" << endl;
      return false;
   }

   task_management();
   return TRUE;
}

bool wake_up(string userName, int id)
{
   int i;

   if (runningQueue.empty() && readyQueue.empty() && blockQueue.empty())
   {
      cout << "No task in the memory." << endl;
      return false;
   }

   else if (blockQueue.empty())
   {
      cout << "Every task is either running or ready, no task need to wake up." << endl;
      return false;
   }

   bool find = 0; //stands for whether task exists in memory

   for (i = 0; i < 100; i++)
   {
      if (id == mem[i].id)
      {
         find = 1;
         if (mem[i].user_name != userName)
         {
            cout << "You can only wake up your task." << endl;
            return false;

            if (mem[i].state == NULL) //The task is already killed.
               cout << "The task does not exist." << endl;

            else if (mem[i].state != BLOCK) //The task is not blocked.
               cout << "The task is either running or ready. "
                       "No need to wake it up."
                    << endl;

            else //the task is blocked.
            {
               mem[i].state = READY; //The task is waken up.
               cout << "The task is waken up to ready state." << endl;
               readyQueue.push_back(mem[i]);
               vector<PCB_type>::iterator iter;
               for (iter = blockQueue.begin(); iter != blockQueue.end(); iter++)
               {
                  if (iter->id == mem[i].id)
                  {
                     blockQueue.erase(iter);
                     break;
                  }
               }
            }
         }
      }
      if (find == 0) //No such tasks in the memory
      {
         cout << "You entered a wrong id!" << endl;
      }
   }
   task_management();
   return TRUE;
}

bool kill(string userName, int id, string jsonmem)
{
   int i;

   if (runningQueue.empty() && readyQueue.empty() && blockQueue.empty())
   {
      cout << "No task in the memory." << endl;
      return false;
   }

   bool find = 0; //stands for whether task exists in memory

   for (i = 0; i < 100; i++)
   {
      if (id == mem[i].id)
      {
         find = 1;

         if (mem[i].user_name != userName)
         {
            cout << "You can only kill your task." << endl;
            return false;
         }

         if (mem[i].state == NULL)
         { //The task is already killed.
            cout << "The task does not exist." << endl;
            return false;
         }

         else if (mem[i].state == RUNNING)
         {
            mem[i].state = NULL; //The task is killed.
            cout << "The task is killed." << endl;
            vector<PCB_type>::iterator iter;
            for (iter = runningQueue.begin(); iter != runningQueue.end(); iter++)
            {
               if (iter->id == mem[i].id){
                  runningQueue.erase(iter);
                  break;
               }
            }
         }

         else if (mem[i].state == BLOCK)
         {
            mem[i].state = NULL; //The task is killed.
            cout << "The task is killed." << endl;
            vector<PCB_type>::iterator iter;
            for (iter = blockQueue.begin(); iter != blockQueue.end(); iter++)
            {
               if (iter->id == mem[i].id){
                  blockQueue.erase(iter);
                  break;
               }
            }
            task_mem_free(id, mem[i].user_name, jsonmem);
         }

         else if (mem[i].state == READY)
         {
            mem[i].state = NULL; //The task is killed.
            cout << "The task is killed." << endl;
            vector<PCB_type>::iterator iter;
            for (iter = readyQueue.begin(); iter != readyQueue.end(); iter++)
            {
               if (iter->id == mem[i].id)
               {
                  readyQueue.erase(iter);
                  break;
               }
            }
            task_mem_free(id, mem[i].user_name, jsonmem);
         }

         break;
      }
   }
   if (find == 0) //No such tasks in the memory
   {
      cout << "You entered a wrong id!" << endl;
   }
   task_management();
   return TRUE;
}

