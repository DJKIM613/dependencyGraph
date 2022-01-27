#ifndef UNTITLED_THREADPOOL_H
#define UNTITLED_THREADPOOL_H

#include <functional>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

class functionNode;

class threadPool {
public :
    void enqueueNode(functionNode *node);
    threadPool(int _numWorkers);
    ~threadPool();

private :
    int numWokers;
    bool isTerminated;
    std::vector<std::thread> workers;
    std::queue<functionNode *> pendingJobs;

    std::mutex m;
    std::condition_variable cv;

    void workerThread();
};

#endif //UNTITLED_THREADPOOL_H
