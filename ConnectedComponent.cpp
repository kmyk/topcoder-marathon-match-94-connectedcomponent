#pragma GCC optimize "O3"
#include <iostream>
#include <cstring>
#include <vector>
#include <algorithm>
#include <array>
#include <set>
#include <map>
#include <stack>
#include <memory>
#include <queue>
#include <tuple>
#include <unordered_set>
#include <unordered_map>
#include <functional>
#include <cassert>
#define repeat(i, n) for (int i = 0; (i) < int(n); ++(i))
#define repeat_from(i, m, n) for (int i = (m); (i) < int(n); ++(i))
#define repeat_reverse(i, n) for (int i = (n)-1; (i) >= 0; --(i))
#define repeat_from_reverse(i, m, n) for (int i = (n)-1; (i) >= int(m); --(i))
#define whole(x) begin(x), end(x)
using namespace std;
const int dy[] = { -1, 1, 0, 0 };
const int dx[] = { 0, 0, 1, -1 };
bool is_on_field(int y, int x, int h, int w) { return 0 <= y and y < h and 0 <= x and x < w; }
template <class T> inline void setmax(T & a, T const & b) { a = max(a, b); }
template <class T> inline void setmin(T & a, T const & b) { a = min(a, b); }

double rdtsc() { // in seconds
    constexpr double ticks_per_sec = 2500000000;
    uint32_t lo, hi;
    asm volatile ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32 | lo) / ticks_per_sec;
}

constexpr double eps = 1e-4;
default_random_engine gen;

struct disjoint_sets {
    vector<int> data;
    disjoint_sets() = default;
    explicit disjoint_sets(size_t n) : data(n, -1) {}
    bool is_root(int i) { return data[i] < 0; }
    int find_root(int i) { return is_root(i) ? i : (data[i] = find_root(data[i])); }
    int set_size(int i) { return - data[find_root(i)]; }
    int unite_sets(int i, int j) {
        i = find_root(i); j = find_root(j);
        if (i != j) {
            if (set_size(i) < set_size(j)) swap(i,j);
            data[i] += data[j];
            data[j] = i;
        }
        return i;
    }
    bool is_same(int i, int j) { return find_root(i) == find_root(j); }
};

class ConnectedComponent { public: vector<int> permute(vector<int> matrix); };
constexpr int MAX_S = 500;

#ifdef VISUALIZE
void visualize(vector<int> const & p) {
    int s = p.size();
    cerr << "VISUALIZE: " << s;
    repeat (i, s) cerr << ' ' << p[i];
    cerr << endl;
}
#endif

struct component_t {
    int sum;
    int size;
};
struct state_t {
    vector<int16_t> p;
    vector<bool> used;
    vector<int> frontier;
    vector<component_t> component;
    double score;
};

shared_ptr<state_t> compute_next_state(shared_ptr<state_t> const & a, int p_back, int s, vector<char> const & matrix) {
    // prepare
    auto b = make_shared<state_t>();
    b->p = a->p;
    b->p.push_back(p_back);
    int k = b->p.size();
    b->used = a->used;
    b->used[p_back] = true;
    b->frontier.resize(k * 2 - 1, -1);
    b->component = a->component;
    auto matrix_at = [&](int y, int x) { return matrix[b->p[y] * s + b->p[x]]; };
    // make next frontier
    disjoint_sets ds(max(1, 4 * k - 4));
    auto add_component = [&](int i, int y, int x) {
        int m = matrix_at(y, x);
        if (m) {
            b->frontier[i] = b->component.size();
            b->component.push_back((component_t) { m, 1 });
        }
        return bool(m);
    };
    auto unite_components = [&](int i, int j) {
        if (i == -1) return;
        if (j == -1) return;
        i = ds.find_root(i);
        j = ds.find_root(j);
        if (i == j) return;
        int k = ds.unite_sets(i, j);
        if (k == j) swap(i, j);
        assert (k == i);
        b->component[i].sum  += b->component[j].sum;
        b->component[i].size += b->component[j].size;
    };
    repeat (y, k - 1) {
        int i = (2 * k - 2) - y;
        if (add_component(i, y, k - 1)) {
            if (y - 1 >= 0) unite_components(b->frontier[i], b->frontier[i + 1]);
            int j = (2 * k - 4) - y;
            unite_components(b->frontier[i], a->frontier[j]);
        }
    }
    repeat (x, k - 1) {
        if (add_component(x, k - 1, x)) {
            if (x - 1 >= 0) unite_components(b->frontier[x], b->frontier[x - 1]);
            unite_components(b->frontier[x], a->frontier[x]);
        }
    }
    if (add_component(k - 1, k - 1, k - 1)) {
        if (k >= 2) {
            unite_components(b->frontier[k - 1], b->frontier[k - 2]);
            unite_components(b->frontier[k - 1], b->frontier[k]);
        }
    }
    // get score
    b->score = a->score;
    for (auto it : b->component) {
        setmax(b->score, it.sum * sqrt(it.size));
    }
    // normalize frontier
    map<int, int> compress;
    repeat (i, 2 * k - 1) if (b->frontier[i] != -1) {
        if (ds.is_root(b->frontier[i])) {
            compress.emplace(b->frontier[i], compress.size());
        }
    }
    repeat (i, 2 * k - 1) if (b->frontier[i] != -1) {
        b->frontier[i] = compress[ds.find_root(b->frontier[i])];
    }
    vector<component_t> component;
    component.swap(b->component);
    b->component.resize(compress.size());
    for (auto it : compress) {
        b->component[it.second] = component[it.first];
    }
    return b;
}

vector<int> ConnectedComponent::permute(vector<int> a_matrix) {
    // prepare
    double clock_begin = rdtsc();
    int sq_s = a_matrix.size();
    int s = int(sqrt(sq_s));
    vector<char> matrix(whole(a_matrix));
    // solve
    vector<shared_ptr<state_t> > cur, prv;
    {
        auto a = make_shared<state_t>();
        a->used.resize(s);
        a->score = 0;
        cur.push_back(a);
    }
    int beam_width = pow(500, 3) * 2 / pow(s, 3);
    repeat (k, s) {
        cur.swap(prv);
        cur.clear();
        for (auto a : prv) {
            repeat (i, s) if (not a->used[i]) {
                cur.push_back(compute_next_state(a, i, s, matrix));
            }
        }
        int cur_size = min<int>(cur.size(), beam_width);
        partial_sort(cur.begin(), cur.begin() + cur_size, cur.end(), [&](shared_ptr<state_t> const & a, shared_ptr<state_t> const & b) { return a->score > b->score; });
        cur.resize(cur_size);
        cerr << "depth = " << k + 1 << ": score = " << cur.front()->score << "  (t = " << rdtsc() - clock_begin << ")" << endl;
    }
#ifdef VISUALIZE
    visualize(vector<int>(whole(cur.front()->p)));
#endif
    return vector<int>(whole(cur.front()->p));
}

// -------8<------- end of solution submitted to the website -------8<-------

template<class T> void getVector(vector<T>& v) {
    for (int i = 0; i < (int)v.size(); ++i)
        cin >> v[i];
}

int main() {
    ConnectedComponent cc;
    int M;
    cin >> M;
    vector<int> matrix(M);
    getVector(matrix);

    vector<int> ret = cc.permute(matrix);
    cout << ret.size() << endl;
    for (int i = 0; i < (int)ret.size(); ++i)
        cout << ret[i] << endl;
    cout.flush();
}
