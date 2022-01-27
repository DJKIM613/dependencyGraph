#include "threadPool.h"
#include "dependencyGraph.h"

threadPool::threadPool(int _numWorkers) : numWokers(_numWorkers), isTerminated(false ) {
    for(int i = 0 ; i < numWokers ; i++) {
        workers.emplace_back([this](){ workerThread();});
    }
}

threadPool::~threadPool() {
    isTerminated = true;
    cv.notify_all();
    for(auto &worker : workers) {
        worker.join();
    }
}

void threadPool::enqueueNode(functionNode *node) {
    {
        std::unique_lock<std::mutex> lock(m);
        pendingJobs.push(node);
    }
    cv.notify_one();
}

void threadPool::workerThread() {
    while(1) {
        std::unique_lock<std::mutex> lock(m);
        cv.wait(lock, [this](){ return isTerminated || pendingJobs.size(); });

        if(isTerminated && pendingJobs.size() == 0) break;

        functionNode *node = pendingJobs.front();
        pendingJobs.pop();
        lock.unlock();

        node->executeFunction();
    }
}