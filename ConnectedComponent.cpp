#include <iostream>
#include <vector>
#include <algorithm>
#include <array>
#include <set>
#include <map>
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

double calculate_score(vector<int> const & p, vector<char> const & matrix) {
    int s = p.size();
    auto at = [&](int y, int x) { return matrix[p[y] * s + p[x]]; };
    auto is_on_field = [&](int y, int x) { return 0 <= y and y < s and 0 <= x and x < s; };
    vector<char> used(s * s);
    int size = 0, acc = 0;
    function<void (int, int)> go = [&](int y, int x) {
        used[y * s + x] = true;
        size += 1;
        acc += at(y, x);
        repeat (i, 4) {
            int ny = y + dy[i];
            int nx = x + dx[i];
            if (is_on_field(ny, nx) and not used[ny * s + nx] and at(ny, nx)) {
                go(ny, nx);
            }
        }
    };
    double result = 0;
    repeat (y, s) repeat (x, s) {
        if (not used[y * s + x] and at(y, x)) {
            size = acc = 0;
            go(y, x);
            setmax(result, acc * sqrt(size));
        }
    }
    return result;
}

void visualize(vector<int> const & p) {
    cerr << "VISUALIZE: " << p.size();
    for (int p_i : p) cerr << ' ' << p_i;
    cerr << endl;
}

vector<int> ConnectedComponent::permute(vector<int> int_matrix) {
    double clock_begin = rdtsc();
    vector<char> matrix(whole(int_matrix));
    int s = int(sqrt(matrix.size()));
    vector<int> p(s);
    iota(whole(p), 0);
// visualize(p);
cerr << "MESSAGE: s = " << s << endl;
    double current_score = calculate_score(p, matrix);
    vector<int> result = p;
    double best_score = current_score;
    double temp = INFINITY;
    for (int iteration = 0; ; ++ iteration) {
        if (iteration % 10 == 0) {
            double clock_end = rdtsc();
            if (clock_end - clock_begin > 9.5) {
cerr << "MESSAGE: iteration = " << iteration << endl;
                break;
            }
            temp = (clock_end - clock_begin) * 10;
        }
        int x = -1, y = -1;
        while (x == y) {
            x = uniform_int_distribution<int>(0, s - 1)(gen);
            y = uniform_int_distribution<int>(0, s - 1)(gen);
        }
        swap(p[x], p[y]);
        double next_score = calculate_score(p, matrix);
        double delta = next_score - current_score;
        if (current_score < next_score + eps or bernoulli_distribution(exp(delta / temp))(gen)) {
            current_score = next_score;
            if (best_score < current_score) {
                best_score = current_score;
                result = p;
// cerr << "score updated: " << best_score << endl;
// visualize(p);
            }
        } else {
            swap(p[x], p[y]);
        }
    }
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
