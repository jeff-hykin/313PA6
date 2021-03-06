/*
    Based on original assignment by: Dr. R. Bettati, PhD
    Department of Computer Science
    Texas A&M University
    Date  : 2013/01/31
 */

#include "BoundedBuffer.h"
#include "Histogram.h"
#include "reqchannel.h"
#include "reqfifo.h"
#include "reqmq.h"
#include <algorithm>
#include <assert.h>
#include <cassert>
#include <cmath>
#include <cstring>
#include <errno.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <list>
#include <numeric>
#include <pthread.h>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <map>
#include "CekoLibrary.h"
using namespace std;

RequestChannel* getChannel(char ipc_option, string name) 
    {
        // fifo
        if (ipc_option == 'f') {
            return new Fifo(name, CLIENT_SIDE);
        // message queue
        } else if (ipc_option == 'q') {
            return new MessageQue(name, CLIENT_SIDE);
        // shared memory
        } else if (ipc_option == 's') {
            cout << "FIXME, not yet implemented" << "\n";
        }
        return nullptr;
    }

// 
// Main
// 
int main(int argc, char* argv[])
    {
        int number_of_people = 3;
        // 
        // Default arguments
        // 
            int number_of_requests_per_person  = 100;                                              // default number_of_requests_per_person
            int number_of_worker_threads       = 5;                                                // default number_of_worker threads
            int capacity_of_the_request_buffer = number_of_people * number_of_requests_per_person; // default capacity_of_the_request_buffer
            char ipc_option                    = 'q';                                              // default inter process communication option
        // 
        // Get arguments
        // 
            int opt = 0;
            while((opt = getopt(argc, argv, "n:w:b:i:")) != -1)
                {
                    switch(opt)
                        {
                            case 'n':
                                number_of_requests_per_person = atoi(optarg);
                                break;
                            case 'w':
                                number_of_worker_threads = atoi(optarg);
                                break;
                            case 'b':
                                capacity_of_the_request_buffer = atoi(optarg);
                                break;
                            case 'i':
                                ipc_option = *optarg;
                                break;
                        }
                }

        // 
        // Create/init the data server
        // 
            int pid = fork();
            if(pid == 0)
                {
                    execl("dataserver", &ipc_option, (char*)NULL);
                }
        // 
        // thread section
        // 
            else
                {
                    // 
                    // Initial output
                    // 
                        cout << "n == " << number_of_requests_per_person << "\n";
                        cout << "w == " << number_of_worker_threads << "\n";
                        cout << "b == " << capacity_of_the_request_buffer << "\n";
                        cout << "i == " << ipc_option << "\n";

                    // 
                    // Init data structures
                    //
                        int total_number_of_requests       = number_of_requests_per_person * number_of_people;
                        int number_of_not_started_requests = total_number_of_requests;
                        RequestChannel* control_channel = getChannel(ipc_option, "control");
                        BoundedBuffer   request_buffer(capacity_of_the_request_buffer);
                        Histogram       histogram_of_tasks;
                        map<string, BoundedBuffer> stat_buffers;
                        stat_buffers["data John Smith"] = BoundedBuffer(capacity_of_the_request_buffer/number_of_people);
                        stat_buffers["data Jane Smith"] = BoundedBuffer(capacity_of_the_request_buffer/number_of_people);
                        stat_buffers["data Joe Smith" ] = BoundedBuffer(capacity_of_the_request_buffer/number_of_people);
                        auto producerFunction = function<int(string)>([&](string persons_name)
                            {
                                for (auto each : range(0, number_of_requests_per_person))
                                    {
                                        request_buffer.push(persons_name);
                                    }
                                return 0;
                            });
                        auto workerFunction = function<int(RequestChannel*)>([&](RequestChannel* worker_channel)
                            {
                                // 
                                // For each avalible request
                                // 
                                while (true)
                                    {
                                        // 
                                        // Check if should stop
                                        // 
                                        Lock(number_of_not_started_requests);
                                        bool should_stop = false;
                                        if (number_of_not_started_requests <= 0)
                                            {
                                                should_stop = true;
                                            }
                                        else
                                            {
                                                --number_of_not_started_requests;
                                            }
                                        Unlock(number_of_not_started_requests);
                                        if (should_stop)
                                            {
                                                // close the channel
                                                delete worker_channel;
                                                break;
                                            }
                                        else
                                            {
                                                string each_request = request_buffer.pop();
                                                // save the data request to the worker_channel
                                                worker_channel->cwrite(each_request);
                                                // retrieve all the data requests from the worker_channel
                                                string response_of_worker = worker_channel->cread();
                                                // put the data in the corrisponding stat buffer 
                                                stat_buffers[each_request].push(response_of_worker);
                                            }
                                    }
                                return 0;
                            });
                        auto statFunction = function<int(string)>([&](string persons_name)
                            {
                                for (auto each : range(0, number_of_requests_per_person))
                                    {
                                        string response_of_worker = stat_buffers[persons_name].pop();
                                        histogram_of_tasks.update(persons_name, response_of_worker);
                                    }
                                return 0;
                            });
                    //
                    // Start threads
                    //
                        // Producers
                            auto john_producer_task = Task(producerFunction, (string)("data John Smith")); john_producer_task.Start();
                            auto jane_producer_task = Task(producerFunction, (string)("data Jane Smith")); jane_producer_task.Start();
                            auto joe_producer_task  = Task(producerFunction, (string)("data Joe Smith" )); joe_producer_task.Start();
                        // Workers
                            vector<Task<int, RequestChannel*>> worker_tasks;
                            for (auto each : range(0, number_of_worker_threads))
                                {
                                    // create channel for worker
                                    control_channel->cwrite("newchannel");
                                    string          new_channel_name  = control_channel->cread();
                                    RequestChannel* worker_channel = getChannel(ipc_option, new_channel_name);
                                    // create worker
                                    auto worker_task = Task(workerFunction, worker_channel);
                                    worker_task.Start();
                                    // Keep track of worker for stopping it later
                                    worker_tasks.push_back(worker_task);
                                }
                        // Stats
                            auto john_stats_task = Task(statFunction, (string)("data John Smith")); john_stats_task.Start();
                            auto jane_stats_task = Task(statFunction, (string)("data Jane Smith")); jane_stats_task.Start();
                            auto joe_stats_task  = Task(statFunction, (string)("data Joe Smith" )); joe_stats_task.Start();
                        // timer
                            auto start_time = CurrentTimeInMicroSeconds();
                    
                    
                    // 
                    // Live update
                    //
                        // create a function that prints the histogram after every some interval
                        auto updateFunction = function<int(int)>([&](int input)
                            {
                                while (true)
                                    {
                                        // OS-independent version of system("clear")
                                        ClearScreen();
                                        histogram_of_tasks.print();
                                        sleep_for(milliseconds(300));
                                        if (john_stats_task.StillRunning()) { continue; }
                                        if (jane_stats_task.StillRunning()) { continue; }
                                        if (joe_stats_task.StillRunning() ) { continue; }
                                        break;
                                    }
                                return 0;
                            });
                        auto update_task = Task(updateFunction, 0); update_task.Start();
                    
                    // 
                    // End Threads
                    //
                        // producers
                            john_producer_task.WaitForCompletion();
                            jane_producer_task.WaitForCompletion();
                            joe_producer_task.WaitForCompletion();
                        // workers
                            for (auto each : worker_tasks)
                                {
                                    each.WaitForCompletion();
                                }
                        // stats
                            john_stats_task.WaitForCompletion();
                            jane_stats_task.WaitForCompletion();
                            joe_stats_task.WaitForCompletion();
                        // update
                            update_task.WaitForCompletion();
                        // timer
                            auto end_time = CurrentTimeInMicroSeconds();
                        // control channel
                            control_channel->cwrite("quit");
                    // 
                    // Print results
                    // 
                        histogram_of_tasks.print();
                        cout << "Duration: " << end_time - start_time << " microseconds\n";
                }
    }
