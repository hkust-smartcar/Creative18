#include <stdio.h>
#include <cuda_runtime.h>
#include <iostream>
#include <vector>
#include <array>
#include <deque>
#include <iterator>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <future>
#include <chrono>
#include <queue>

using std::cout;
using std::endl;
    
struct Entry {
  int weight;
  std::pair<int, int> coords;
};

struct Node {
  int val;
  Node* next1 = nullptr;
  Node* next2 = nullptr;
};

std::vector<Entry> owal;

constexpr int chessNo = 11;

void printOwal(const std::vector<Entry>& owal) {
  for (auto&& e : owal) {
      cout << "w:" << e.weight << " at (" << e.coords.first << "," << e.coords.second << ")" << endl;
  }
}

void printCom(const std::vector<int>& a) {
  for (auto&& e: a) {
      cout << e << " ";
  }
  cout << endl;
}

void printCount(const std::array<int, chessNo>& a) {
  cout << "Count: ";
  for (auto&& e : a)
      cout << e << " ";
  cout << endl;
}

inline std::array<Node, chessNo> genCycle(const std::vector<int>& com) {
  std::array<Node, chessNo> nodes;
  for (int i = 1; i <= chessNo; i++) {
      nodes[i - 1].val = i;
  }

  for (int i = 0; i < owal.size(); i++) {
      if (com[i]) {
          if (nodes[owal[i].coords.first - 1].next1 == nullptr) {
              nodes[owal[i].coords.first - 1].next1 = &nodes[owal[i].coords.second - 1];
          } else {
              nodes[owal[i].coords.first - 1].next2 = &nodes[owal[i].coords.second - 1];
          }
          if (nodes[owal[i].coords.second - 1].next1 == nullptr) {
              nodes[owal[i].coords.second - 1].next1 = &nodes[owal[i].coords.first - 1];
          } else {
              nodes[owal[i].coords.second - 1].next2 = &nodes[owal[i].coords.first - 1];
          }
      }
  }
  return nodes;
}

inline std::array<int, chessNo> genPath(const std::array<Node, chessNo>& nodes) {
  std::array<int, chessNo> path;
  const Node* n = &nodes[chessNo - 1];
  path[0] = n->val;
  int old1 = n->val;
  n = n->next1;
  path[1] = n->val;
  int old2 = n->val;
  for (int i = 2; i <= chessNo; i++) {
      old2 = n->val;
      if (old1 == n->next1->val)
          n = n->next2;
      else
          n = n->next1;
      path[i] = n->val;
      old1 = old2;
  }

  return path;
};

inline bool checkCycle(const std::array<int, chessNo>& cycle) {
  bool valid_first = true;
  bool valid_second = true;

  // check if continuous cycle
  if (std::find(cycle.begin()+1, cycle.end(), chessNo) != cycle.end()) {
      return false;
  }

  
  // ONE DIRECTION
  const int half = (chessNo - 1) / 2;
  for (int i = 1, n = 0; i < chessNo; i++) {
      if (cycle[(i) % chessNo] <= half)
          n++;
      else n--;
      if (n < 0) {
          valid_first = false;
          break;
      }
  }

  if (!valid_first) {
      for (int i = 1, n = 0; i < chessNo; i++) {
          if (cycle[(chessNo - i) % chessNo] <= half)
              n++;
          else n--;
          if (n < 0) {
              valid_second = false;
              break;
          }
      }
  }
  return valid_first || valid_second;
}

void printNodes(const std::array<Node, chessNo> nodes) {
  cout << "Cycle: ";
  const Node* n = &nodes[chessNo - 1];
  cout << n->val << " ";
  int old1 = n->val;
  n = n->next1;
  cout << n->val << " ";
  int old2 = n->val;
  for (int i = 0; i < chessNo - 2; i++) {
      old2 = n->val;
      if (old1 == n->next1->val)
          n = n->next2;
      else
          n = n->next1;
      cout << n->val << " ";
      old1 = old2;
  }
  cout << endl;
}

void printPath(const std::array<int, chessNo> p) {
  cout << "Cycle: ";
  for (auto&& e : p) {
      cout << e << " ";
  }
  cout << endl;
}

__global__ inline bool checkCombination(const std::vector<int>& com) {
  std::array<int, chessNo> count{};
  for (int i = 0; i < owal.size(); i++) {
      if (com[i]) {
          count[owal[i].coords.first - 1]++;
          count[owal[i].coords.second - 1]++;
      }
  }
//   printCount(count);
  for (int i = 0; i < chessNo; i++) {
      if (count[i] != 2) break;
      else if (i == chessNo - 1) {
          std::array<Node, chessNo> nodes = genCycle(com);
          // printCom(com);
          // printCount(count);
          std::array<int, chessNo> path = genPath(nodes);
          // printPath(path);
          if (checkCycle(path)) {
              cout << "FOUND." << endl;
              printPath(path);
              printCom(com);
              int w = 0;
              for (int j = 0; j < owal.size(); j++) {
                  if (com[j]) {
                      w += owal[j].weight;
                  }
              }
              cout << "Total weight: " << w << endl;
              return true;
          }

      }
  }
  return false;
}

#define BLOCK_DIM   128
#define GRID_DIM    32

__global__ void Find(std::vector<int>* coms, int* result)
{
  int bx = blockIdx.x;

  // Thread index
  int tx = threadIdx.x;

  std::vector<int>& com = coms[bx*BLOCK_DIM+tx];
  bool _result = checkCombination(com);
  __shared__ uint32_t found_id;
  found_id = BLOCK_DIM*GRID_DIM+1; 
  __syncthreads();
  if(_result){
      found_id = min(found_id,bx*BLOCK_DIM+tx);
  }
  __syncthreads();
  *result = found_id;
}

int main()
{
	
  std::srand(std::time(nullptr));

    /** Setup OWAL */
  for (int i = 1, temp = 0; i <= chessNo; i++) {
    for (int j = i + 1; j <= chessNo; j++) {
      cout << "(" << i << "," << j << ")";
      // cin >> temp; 
      temp = 1+std::rand()%50;
      owal.push_back({temp, {i, j}});
    }
  }
  sort(owal.begin(), owal.end(), [](const Entry& a, const Entry& b) {
    return a.weight < b.weight;
  });
  printOwal(owal);

  /** Setup k-combination */
  std::vector<int> com = {};

  for (int i = 0; i < chessNo; i++)
    com.push_back(1);
  for (int i = 0; i < owal.size() - chessNo; i++)
    com.push_back(0);

  std::next_permutation(com.begin(), com.end());
  std::vector<int> original_com = com;


  /** for each k-combination, O(n) per cycle */
  int cnt = 0;
  
  /** for CUDA*/
  constexpr int len = GRID_DIM*BLOCK_DIM;   //length of coms to handle in parallel
  std::vector<int>* d_coms = nullptr;  //coms array store in gpu(device)
  std::vector<int> h_coms[len];  //coms array store in host
  int* d_found_id;
  int h_found_id;
  cudaMalloc((void**) &d_coms, sizeof(com) * len);
  cudaMalloc((void**) &d_found_id, sizeof(int));

  dim3 dimBlock(BLOCK_DIM);
	dim3 dimGrid(GRID_DIM);

  int push_len = 0;

  do {
    if(push_len<len){
      prev_permutation(com.begin(), com.end());
      h_coms[push_len]=com;
      push_len++;
    } else {
      cudaMemcpy(d_coms, h_coms, len* sizeof(com), cudaMemcpyHostToDevice);
      Find<<<dimGrid,dimBlock>>>(d_coms,d_found_id);
      cudaMemcpy(&h_found_id, d_found_id, sizeof(int),cudaMemcpyDeviceToHost);
      if(h_found_id!=-1){
        std::vector<int>& temp = h_coms[h_found_id];cout<<endl;
        for(int i:temp)cout<<i<<" ";
        cout<<endl;
        break;
      }
      push_len = 0;
    }
    if (++cnt % 10000000 == 0) cout << cnt << endl;
    // if (checkCombination(com)) break;

  } while (com != original_com);

	return 0;
}

