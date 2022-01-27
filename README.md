# DependencyGraph

## 1. Build

```
mkdir build && cd build
cmake .. 
make all
```

## 2. Structure

![structure](./Untitled%20Diagram.drawio.png)

## 3. Logic

1. dependencyGraph 초기화
    - 대기중인 노드의 수(numPendingNodes)와 쓰레드풀(numPendingNodes) 관리
2. dependencyGraph::addNode()로 그래프의 node 구성
    - numPendingNodes 1 증가
3. dependencyGraph::make_edge()로 그래프의 edge 구성
    - 두 노드가 같은 그래프의 원소인지 확인
    - source node에 destination node로 단반향 간선 추가
    - destination node의 indegree 증가
4. dependencyGraph::start()로 threadPool로 실행 가능한 노드 실행
    - indegree가 0인 노드를 threadPool에 push 및 numPendingNodes 1 감소
    - 실행 후 functionNode::postProcess()로 인접한 노드의 indegree 1 감소
    - 인접한 노드의 indegree가 0일 경우 해당 노드를 threadPool에 대입.
5. dependencyGraph::waitForAll() 호출
    - numPendingNodes = 0 이 될때까지 기다림
    - ~threadPool() 호출되면서 threadPool에 남아있는 node가 없을 때까지 실행
