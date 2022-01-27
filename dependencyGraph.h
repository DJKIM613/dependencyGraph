#ifndef UNTITLED_DEPENDENCYGRAPH_H
#define UNTITLED_DEPENDENCYGRAPH_H

#include "threadPool.h"
#include <atomic>
#include <functional>
#include <vector>

class functionNode;
class dependencyGraph;

class functionNode {
public:
    functionNode(dependencyGraph *G, std::function<void()> bodyFunc);

    void executeFunction();
    void addAdjacentNode(functionNode *nodePtr);
    void modifyIndegree(int count);

    int getIndegree() const;
    dependencyGraph *getGraphPtr() const;

private:
    std::mutex mMutex;
    int mInDegree;
    std::function<void()> mBodyFunc;
    std::vector<functionNode *> mAdjacentNodePtrs;
    dependencyGraph *const mG;

    void postProcess();
};

class dependencyGraph {
public:
    functionNode *addNode(std::function<void()> bodyFunc);
    static void make_edge(functionNode *src, functionNode *dst);

    void modifyNumPendingNodes(int count);
    void pushFunctionNode(functionNode *nodePtr);
    void start();
    void waitForAll();

    dependencyGraph(int numWorkers);
    ~dependencyGraph();

private:
    std::mutex mMutex;
    std::condition_variable mCv;

    int numPendingNodes;
    threadPool mThreadPool;
    std::vector<functionNode *> mNodePtrs;
};

#endif//UNTITLED_DEPENDENCYGRAPH_H
