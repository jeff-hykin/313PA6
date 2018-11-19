/*
    Based on original assignment by: Dr. R. Bettati, PhD
    Department of Computer Science
    Texas A&M University
    Date  : 2013/01/31
 */

#include "BoundedBuffer.h"
#include "Histogram.h"
#include "reqchannel.h"
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


// 
// Main
// 
int main(int argc, char* argv[])
    {
        // 
        // Default arguments
        // 
            int number_of_requests_per_person  = 100;                               // default number_of_requests_per_person
            int number_of_worker_threads       = 1;                                 // default number_of_worker threads
            int capacity_of_the_request_buffer = 3 * number_of_requests_per_person; // default capacity_of_the_request_buffer, you should change this default
        // 
        // Get arguments
        // 
            int opt = 0;
            while((opt = getopt(argc, argv, "n:w:b:")) != -1)
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
                        }
                }

        // 
        // Create/init the data server
        // 
            int pid = fork();
            if(pid == 0)
                {
                    execl("dataserver", (char*)NULL);
                }
        // 
        // thread section
        // 
            else
                {
                    // 
                    // Initial output
                    // 
                        cout << "n == " << number_of_requests_per_person << endl;
                        cout << "w == " << number_of_worker_threads << endl;
                        cout << "b == " << capacity_of_the_request_buffer << endl;

                    // 
                    // Init data structures
                    // 
                        RequestChannel* control_channel = new RequestChannel("control", RequestChannel::CLIENT_SIDE);
                        BoundedBuffer   request_buffer(capacity_of_the_request_buffer);
                        Histogram       histogram_of_tasks;
                        map<string, BoundedBuffer> stat_buffers;
                        stat_buffers["data John Smith"] = BoundedBuffer(capacity_of_the_request_buffer/3);
                        stat_buffers["data Jane Smith"] = BoundedBuffer(capacity_of_the_request_buffer/3);
                        stat_buffers["data Joe Smith" ] = BoundedBuffer(capacity_of_the_request_buffer/3);
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
                                        string each_request = request_buffer.pop();
                                        // save the data request to the worker_channel
                                        worker_channel->cwrite(each_request);
                                        if (each_request == "quit")
                                            {
                                                // close the channel
                                                delete worker_channel;
                                                break;
                                            }
                                        else
                                            {
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
                                while (true)
                                    {
                                        string response_of_worker = stat_buffers[persons_name].pop();
                                        if (response_of_worker == "quit")
                                            {
                                                break;
                                            }
                                        else
                                            {
                                                // put the data in the corrisponding stat buffer 
                                                histogram_of_tasks.update(persons_name, response_of_worker);
                                            }
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
                                    RequestChannel* worker_channel    = new RequestChannel(new_channel_name, RequestChannel::CLIENT_SIDE);
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
                                return 0;
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
                                    request_buffer.push("quit");
                                }
                            for (auto each : worker_tasks)
                                {
                                    each.WaitForCompletion();
                                }
                        // stats
                            stat_buffers["data John Smith"].push("quit");
                            stat_buffers["data Jane Smith"].push("quit");
                            stat_buffers["data Joe Smith" ].push("quit");
                            john_stats_task.WaitForCompletion();
                            jane_stats_task.WaitForCompletion();
                            joe_stats_task.WaitForCompletion();
                        // update
                            update_task.WaitForCompletion();
                        // timer
                            auto end_time = CurrentTimeInMicroSeconds();
                        
                    // 
                    // Print results
                    // 
                        histogram_of_tasks.print();
                        cout << "Duration: " << end_time - start_time << " microseconds\n";
                }
    }
