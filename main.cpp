#include "dependencyGraph.h"
#include "threadPool.h"

void funcA() {
    printf("This is func A\nThe next fucntion is funcB\n");
}

void funcB() {
    printf("This is func B\n");
}

void funcC() {
    printf("This is func C\nI'm running independently\n");
}

int main() {
    dependencyGraph G(3);

    functionNode *nodeA = G.addNode([]() { funcA(); });
    functionNode *nodeC = G.addNode([]() { funcC(); });
    functionNode *nodeB = G.addNode([]() { funcB(); });
    dependencyGraph::make_edge(nodeA, nodeB);

    G.start();
    G.waitForAll();
    return 0;
}