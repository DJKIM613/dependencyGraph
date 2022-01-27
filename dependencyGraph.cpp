#include "dependencyGraph.h"
#include <assert.h>

functionNode::functionNode(dependencyGraph *G, std::function<void()> bodyFunc) : mG(G), mBodyFunc(bodyFunc), mInDegree(0) {
}

int functionNode::getIndegree() const {
    return mInDegree;
}

dependencyGraph *functionNode::getGraphPtr() const {
    return mG;
}

void functionNode::modifyIndegree(int count) {
    mInDegree += count;
}

void functionNode::executeFunction() {
    this->mBodyFunc();
    postProcess();
}

void functionNode::addAdjacentNode(functionNode *nodePtr) {
    this->mAdjacentNodePtrs.push_back(nodePtr);
}

/*
 * decrease 1 on adjacent node
 * check adjacent node's indegree is 0
 * if it is true, add adjacent node to threadPool
 * */
void functionNode::postProcess() {
    for (auto nodePtr: this->mAdjacentNodePtrs) {
        // TODO : maybe... spin_lock is faster than mutex
        std::unique_lock<std::mutex> lock(nodePtr->mMutex);
        nodePtr->mInDegree--;

        // TODO : should be delete
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        if (nodePtr->mInDegree == 0) mG->pushFunctionNode(nodePtr);
    }
    mG->modifyNumPendingNodes(-1);
}

functionNode *dependencyGraph::addNode(std::function<void()> bodyFunc) {
    functionNode *nodePtr = new functionNode(this, bodyFunc);
    {
        std::unique_lock<std::mutex> lock(mMutex);
        modifyNumPendingNodes(1);
        mNodePtrs.push_back(nodePtr);
    }
    return nodePtr;
}

void dependencyGraph::make_edge(functionNode *src, functionNode *dst) {
    // TODO : Can I leave this assert?
    assert(src->getGraphPtr() == dst->getGraphPtr());
    src->addAdjacentNode(dst);
    dst->modifyIndegree(1);
}

/*
 * modify numPendingNodes and check the value is 0
 * to prevent two other access thread modify numPendingNodes and they get numPendingNodes 0 both,
 * we should do this process after mutex lock
 */
void dependencyGraph::modifyNumPendingNodes(int count) {
    // TODO : maybe... spin_lock is faster than mutex
    std::unique_lock<std::mutex> lock;
    numPendingNodes += count;
    printf("numPendingNodes : %d\n", numPendingNodes);
    if (numPendingNodes == 0) mCv.notify_one();
}

void dependencyGraph::pushFunctionNode(functionNode *nodePtr) {
    mThreadPool.enqueueNode(nodePtr);
}

void dependencyGraph::start() {
    for (auto nodePtr: mNodePtrs) {
        int indegree = nodePtr->getIndegree();
        if (indegree == 0) mThreadPool.enqueueNode(nodePtr);
    }
}

void dependencyGraph::waitForAll() {
    std::unique_lock<std::mutex> lock(mMutex);
    mCv.wait(lock, [this]() { return numPendingNodes == 0; });
}

dependencyGraph::dependencyGraph(int numWorkers) : numPendingNodes(0), mThreadPool(numWorkers) {
}

dependencyGraph::~dependencyGraph() {
    for (auto nodePtr: mNodePtrs) {
        delete nodePtr;
    }
}
