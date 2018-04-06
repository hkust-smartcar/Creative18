#include <iostream>
#include <vector>
#include <array>
#include <deque>
#include <iterator>
#include <algorithm>

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

const int size = 5;

void print_owal(const vector<Entry>& owal) {
    for (auto&& e : owal) {
        cout << "w:" << e.weight << " at (" << e.coords.first << "," << e.coords.second << ")" << endl;
    }
}

void print_com(const vector<int>& a) {
    for (auto&& e: a) {
        cout << e << " ";
    }
    cout << endl;
}

void print_count(const array<int, size>& a) {
    cout << "Count: ";
    for (auto&& e : a)
        cout << e << " ";
    cout << endl;
}

array<Node, size> genCycle(const array<int, size>& count, const vector<int>& com) {
    array<int, size> t_count = count;

    array<Node, size> nodes;
    for (int i = 1; i <= size; i++) {
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

array<int, size> genPath(const array<Node, size>& nodes) {
    array<int, size> path;
    const Node* n = &nodes[size - 1];
    path[0] = n->val;
    int old1 = n->val;
    n = n->next1;
    path[1] = n->val;
    int old2 = n->val;
    for (int i = 2; i <= size; i++) {
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

bool checkCycle(const array<int, size>& cycle) {
    bool valid_first = true;
    bool valid_second = true;
    // ONE DIRECTION
    const int half = (size - 1) / 2;
    for (int i = 1, n = 0; i < size; i++) {
        if (cycle[(i) % size] <= half)
            n++;
        else n--;
        if (n < 0) {
            valid_first = false;
            break;
        }
    }

    if (!valid_first) {
        for (int i = 1, n = 0; i < size; i++) {
            if (cycle[(size - i) % size] <= half)
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

void print_nodes(const array<Node, size> nodes) {
    cout << "Cycle: ";
    const Node* n = &nodes[size - 1];
    cout << n->val << " ";
    int old1 = n->val;
    n = n->next1;
    cout << n->val << " ";
    int old2 = n->val;
    for (int i = 0; i < size - 2; i++) {
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

void print_path(const array<int, size> p) {
    cout << "Cycle: ";
    for (auto&& e : p) {
        cout << e << " ";
    }
    cout << endl;
}

int main() {

    /** Setup OWAL **/
    for (int i = 1, temp = 0; i <= size; i++) {
        for (int j = i + 1; j <= size; j++) {
            cout << "(" << i << "," << j << ")";
            cin >> temp;
            owal.push_back({temp, {i, j}});
        }
    }
    sort(owal.begin(), owal.end(), [](Entry& a, Entry& b) {
        return a.weight < b.weight;
    });
    print_owal(owal);

    vector<int> com = {};

    for (int i = 0; i < size; i++)
        com.push_back(1);
    for (int i = 0; i < owal.size() - size; i++)
        com.push_back(0);

    next_permutation(com.begin(), com.end());
    vector<int> original_com = com;

    array<int, size> count{};

    /** for each k-combination **/
    do {
        count.fill(0);
        prev_permutation(com.begin(), com.end());
//        print_com(com);
        for (int i = 0; i < owal.size(); i++) {
            if (com[i]) {
                count[owal[i].coords.first - 1]++;
                count[owal[i].coords.second - 1]++;
            }
        }
//        print_count(count);
        for (int i = 0; i < size; i++) {
            if (count[i] != 2) break;
            else if (i == size - 1) {
                array<Node, size> nodes = genCycle(count, com);
//                print_com(com);
//                print_count(count);
                array<int, size> path = genPath(nodes);
//                print_path(path);
                if (checkCycle(path)) {
                    cout << "FOUND." << endl;
                    print_path(path);
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