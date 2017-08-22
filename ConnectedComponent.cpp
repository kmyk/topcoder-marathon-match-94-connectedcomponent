#pragma GCC optimize "O3"
#include <iostream>
#include <cstring>
#include <vector>
#include <algorithm>
#include <array>
#include <set>
#include <map>
#include <stack>
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

constexpr double eps = 1e-6;
default_random_engine gen;

class ConnectedComponent { public: vector<int> permute(vector<int> matrix); };
constexpr int MAX_S = 500;

int s, sq_s;
int16_t p[MAX_S];
char matrix[MAX_S * MAX_S];
int at(int y, int x) { return matrix[p[y] * s + p[x]]; };
bool is_on_field(int y, int x) { return 0 <= y and y < s and 0 <= x and x < s; };

pair<int, int> find_center() {
    for (int delta = 0; delta < s; ++ delta) {
        int l = s / 2 - delta;
        int r = s / 2 + delta + 1;
        repeat_from (y, l, r) {
            int d = (abs(y - s / 2) == delta ? 1 : r - l - 1);
            for (int x = l; x < r; x += d) {
                if (at(y, x) >= 1) {
                    return { y, x };
                }
            }
        }
    }
    assert (false);
}

char g_used[MAX_S * MAX_S] = {};
int16_t g_stack[2 * MAX_S * MAX_S];
int g_stack_size = 0;
int g_cy, g_cx;
int g_ly, g_lx;
int g_ry, g_rx;

double analyze_permutation() {
    tie(g_cy, g_cx) = find_center();
    g_ly = s;
    g_lx = s;
    g_ry = 0;
    g_rx = 0;
    int size = 0;
    int sum = 0;
    auto push = [&](int y, int x) {
        g_used[y * s + x] = true;
        g_stack[g_stack_size ++] = y;
        g_stack[g_stack_size ++] = x;
        size += 1;
        sum += at(y, x);
    };
    push(g_cy, g_cx);
    while (g_stack_size) {
        int x = g_stack[-- g_stack_size];
        int y = g_stack[-- g_stack_size];
        setmin(g_ly, y);
        setmax(g_ry, y + 1);
        setmin(g_lx, x);
        setmax(g_rx, x + 1);
        repeat (i, 4) {
            int ny = y + dy[i];
            int nx = x + dx[i];
            if (is_on_field(ny, nx) and not g_used[ny * s + nx] and at(ny, nx)) {
                push(ny, nx);
            }
        }
    }
    repeat_from (y, g_ly, g_ry) {
        memset((void *)(g_used + y * s + g_lx), 0, (size_t)(g_rx - g_lx));
    }
    return sum * sqrt(size);
}

#ifdef VISUALIZE
void visualize() {
    cerr << "VISUALIZE: " << s;
    repeat (i, s) cerr << ' ' << p[i];
    cerr << endl;
}
#endif

vector<int> ConnectedComponent::permute(vector<int> a_matrix) {
    // prepare
    double clock_begin = rdtsc();
    sq_s = a_matrix.size();
    s = int(sqrt(sq_s));
    repeat (z, sq_s) matrix[z] = a_matrix[z];
    // initialize
    iota(p, p + s, 0);
#ifdef VISUALIZE
visualize();
#endif
#ifdef LOCAL
cerr << "MESSAGE: s = " << s << endl;
#endif
    {
        vector<int> cnt(s);
        repeat (y, s) {
            repeat (x, s) {
                int m = matrix[p[y] * s + p[x]];
                int value = m > 0 ? m + 4 : m == 0 ? -4 : m;
                cnt[p[y]] += value;
                cnt[p[x]] += value;
            }
        }
        vector<int> t(s);
        iota(whole(t), 0);
        sort(whole(t), [&](int i, int j) { return cnt[i] > cnt[j]; });
        int j = 0;
        for (int i = 0; i < s; i += 2) p[j ++] = t[i];
        reverse(p, p + j);
        for (int i = 1; i < s; i += 2) p[j ++] = t[i];
#ifdef VISUALIZE
visualize();
#endif
    }
    double current_score = analyze_permutation();
    vector<int> result(p, p + s);
    double best_score = current_score;
    // simulated annealing
    double temp = INFINITY;
    double time = 0.0;
    for (int iteration = 0; ; ++ iteration) {
        if (iteration % 10 == 0 or time > 0.95) {
            double clock_end = rdtsc();
            time = (clock_end - clock_begin) / 10.0;
            if (time > 0.98) {
#ifdef LOCAL
cerr << "MESSAGE: iteration = " << iteration << endl;
cerr << "MESSAGE: time = " << time << endl;
cerr << "MESSAGE: avg m = " << accumulate(matrix, matrix + sq_s, 0) /(double) sq_s << endl;
int cnt[3] = {};
repeat (z, sq_s) cnt[matrix[z] > 0 ? 0 : matrix[z] == 0 ? 1 : 2] += 1;
cerr << "MESSAGE: positive : zero : negative = "
    << cnt[0] /(double) sq_s << " : "
    << cnt[1] /(double) sq_s << " : "
    << cnt[2] /(double) sq_s << endl;
#endif
                break;
            }
            temp = (1 - time) * s;
        }
        constexpr int neightborhood_type_swap = 3;
        constexpr int neightborhood_type_rotate = 5;
        constexpr int neightborhood_type_reverse = 1;
        int neightborhood_type = uniform_int_distribution<int>(0,
                + neightborhood_type_swap
                + neightborhood_type_rotate
                + neightborhood_type_reverse
                - 1)(gen);
        int x = -1, y = -1;
        while (x == y) {
            x = bernoulli_distribution(0.5)(gen) ?
                uniform_int_distribution<int>(max(0, g_ly - 3), min(s, g_ry + 3) - 1)(gen) :
                uniform_int_distribution<int>(max(0, g_lx - 3), min(s, g_rx + 3) - 1)(gen);
            y = uniform_int_distribution<int>(0, s - 1)(gen);
        }
        if (neightborhood_type < neightborhood_type_swap) {
            swap(p[x], p[y]);
        } else if (neightborhood_type < neightborhood_type_swap + neightborhood_type_rotate) {
            if (x < y) {
                rotate(p + x, p + (x + 1), p + (y + 1));
            } else {
                rotate(p + y, p + x, p + (x + 1));
            }
        } else {
            reverse(p + min(x, y), p + (max(x, y) + 1));
        }
        auto next_score = analyze_permutation();
        double delta = next_score - current_score;
        if (current_score < next_score + 10 or bernoulli_distribution(exp(delta / temp))(gen)) {
            current_score = next_score;
            if (best_score < next_score) {
                best_score = next_score;
                result.assign(p, p + s);
#ifdef VISUALIZE
visualize();
#endif
#ifdef LOCAL
cerr << "MESSAGE: iteration = " << iteration << endl;
cerr << "MESSAGE: time      = " << time << endl;
cerr << "MESSAGE: score     = " << int(best_score) << endl;
#endif
            }
        } else {
            if (neightborhood_type < neightborhood_type_swap) {
                swap(p[x], p[y]);
            } else if (neightborhood_type < neightborhood_type_swap + neightborhood_type_rotate) {
                if (x < y) {
                    rotate(p + x, p + y, p + (y + 1));
                } else {
                    rotate(p + y, p + (y + 1), p + (x + 1));
                }
            } else {
                reverse(p + min(x, y), p + (max(x, y) + 1));
            }
        }
    }
#ifdef VISUALIZE
cerr << "MESSAGE: score = " << best_score << endl;
#endif
    return result;
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
