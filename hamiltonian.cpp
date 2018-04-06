#include <iostream>
#include <vector>
#include <array>
#include <deque>
#include <iterator>
#include <algorithm>
#include <cstdlib>
#include <ctime>

using namespace std;


struct Entry {
    int weight;
    std::pair<int, int> coords;
};

struct Node {
    int val;
    Node* next1 = nullptr;
    Node* next2 = nullptr;
};

vector<Entry> owal;

const int chessNo = 11;

void printOwal(const vector<Entry>& owal) {
    for (auto&& e : owal) {
        cout << "w:" << e.weight << " at (" << e.coords.first << "," << e.coords.second << ")" << endl;
    }
}

void printCom(const vector<int>& a) {
    for (auto&& e: a) {
        cout << e << " ";
    }
    cout << endl;
}

void printCount(const array<int, chessNo>& a) {
    cout << "Count: ";
    for (auto&& e : a)
        cout << e << " ";
    cout << endl;
}

array<Node, chessNo> genCycle(const array<int, chessNo>& count, const vector<int>& com) {
    array<int, chessNo> t_count = count;

    array<Node, chessNo> nodes;
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

array<int, chessNo> genPath(const array<Node, chessNo>& nodes) {
    array<int, chessNo> path;
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

bool checkCycle(const array<int, chessNo>& cycle) {
    bool valid_first = true;
    bool valid_second = true;

    // check if continuous cycle
    if (find(cycle.begin()+1, cycle.end(), chessNo) != cycle.end()) {
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

void printNodes(const array<Node, chessNo> nodes) {
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

void printPath(const array<int, chessNo> p) {
    cout << "Cycle: ";
    for (auto&& e : p) {
        cout << e << " ";
    }
    cout << endl;
}

int main() {
    std::srand(std::time(nullptr));
    /** Setup OWAL **/
    for (int i = 1, temp = 0; i <= chessNo; i++) {
        for (int j = i + 1; j <= chessNo; j++) {
            cout << "(" << i << "," << j << ")";
            cin >> temp; 
            // temp = 1+std::rand()%50;
            owal.push_back({temp, {i, j}});
        }
    }
    sort(owal.begin(), owal.end(), [](Entry& a, Entry& b) {
        return a.weight < b.weight;
    });
    printOwal(owal);

    vector<int> com = {};

    for (int i = 0; i < chessNo; i++)
        com.push_back(1);
    for (int i = 0; i < owal.size() - chessNo; i++)
        com.push_back(0);

    next_permutation(com.begin(), com.end());
    vector<int> original_com = com;

    array<int, chessNo> count{};

    /** for each k-combination **/
    int cnt = 0;
    do {
        count.fill(0);
        prev_permutation(com.begin(), com.end());
        // printCom(com);
        if (++cnt % 1000000 == 0) cout << cnt << endl;
        for (int i = 0; i < owal.size(); i++) {
            if (com[i]) {
                count[owal[i].coords.first - 1]++;
                count[owal[i].coords.second - 1]++;
            }
        }
//        printCount(count);
        for (int i = 0; i < chessNo; i++) {
            if (count[i] != 2) break;
            else if (i == chessNo - 1) {
                array<Node, chessNo> nodes = genCycle(count, com);
//                printCom(com);
//                printCount(count);
                array<int, chessNo> path = genPath(nodes);
//                printPath(path);
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
                    return 0;
                }

            }
        }
    } while (com != original_com);

}