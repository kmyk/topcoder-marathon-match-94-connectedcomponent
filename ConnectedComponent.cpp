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

struct permutation_info_t {
    double score;
    double evaluated;
    int ly, lx, ry, rx;
};
permutation_info_t analyze_permutation(vector<int> const & p, vector<char> const & matrix, int ly, int lx, int ry, int rx) {
    int s = p.size();
    auto at = [&](int y, int x) { return matrix[p[y] * s + p[x]]; };
    auto is_on_field = [&](int y, int x) { return 0 <= y and y < s and 0 <= x and x < s; };
    vector<char> used(s * s);
    int size = -1, acc = -1;
    bool centered = false;
    int nly = -1, nlx = -1, nry = -1, nrx = -1;
    function<void (int, int)> go = [&](int y, int x) {
        used[y * s + x] = true;
        size += 1;
        acc += at(y, x);
        if (abs(y - s / 2) + abs(x - s / 2) <= 3) centered = true;
        setmin(nly, y);
        setmin(nlx, x);
        setmax(nry, y + 1);
        setmax(nry, x + 1);
        repeat (i, 4) {
            int ny = y + dy[i];
            int nx = x + dx[i];
            if (is_on_field(ny, nx) and not used[ny * s + nx] and at(ny, nx)) {
                go(ny, nx);
            }
        }
    };
    permutation_info_t info = {};
    repeat_from (y, ly, ry) repeat_from (x, lx, rx) {
        if (not used[y * s + x] and at(y, x)) {
            size = acc = 0;
            centered = false;
            nly = y; nlx = x; nry = y + 1; nrx = x + 1;
            go(y, x);
            if (info.score < acc * sqrt(size)) {
                info.score = acc * sqrt(size);
            }
            if (centered and info.evaluated < acc * sqrt(size)) {
                info.evaluated = acc * sqrt(size);
                info.ly = nly;
                info.lx = nlx;
                info.ry = nry;
                info.rx = nrx;
            }
        }
    }
    return info;
}

#ifdef VISUALIZE
void visualize(vector<int> const & p) {
    cerr << "VISUALIZE: " << p.size();
    for (int p_i : p) cerr << ' ' << p_i;
    cerr << endl;
}
#endif

double estimate_base_score(int s, vector<char> const & matrix) {
    vector<int> p(s);
    iota(whole(p), 0);
    deque<double> scores;
    repeat (i, 16) {
        shuffle(whole(p), gen);
        scores.push_back(calculate_score(p, matrix));
    }
    sort(whole(scores));
    scores.pop_front();
    scores.pop_front();
    scores.pop_front();
    scores.pop_back();
    scores.pop_back();
    scores.pop_back();
    return accumulate(whole(scores), 0.0) / 10;
}

vector<int> ConnectedComponent::permute(vector<int> int_matrix) {
    // prepare
    double clock_begin = rdtsc();
    vector<char> matrix(whole(int_matrix));
    int s = int(sqrt(matrix.size()));
    // initialize
    vector<int> p(s);
    iota(whole(p), 0);
#ifdef VISUALIZE
visualize(p);
#endif
#ifdef LOCAL
cerr << "MESSAGE: s = " << s << endl;
#endif
    {
        vector<int> cnt(s);
        repeat (y, s) {
            repeat (x, s) {
                int m = matrix[p[y] * s + p[x]];
                cnt[p[y]] += max<int>(0, m ? m + 4 : 0);
                cnt[p[x]] += max<int>(0, m ? m + 4 : 0);
            }
        }
        vector<int> t(s);
        iota(whole(t), 0);
        sort(whole(t), [&](int i, int j) { return cnt[i] > cnt[j]; });
        p.clear();
        for (int i = 0; i < s; i += 2) p.push_back(t[i]);
        reverse(whole(p));
        for (int i = 1; i < s; i += 2) p.push_back(t[i]);
#ifdef VISUALIZE
visualize(p);
#endif
    }
    auto current = analyze_permutation(p, matrix, 0, 0, s, s);
    vector<int> result = p;
    double best_score = current.score;
    // simulated annealing
    double temp = INFINITY;
    for (int iteration = 0; ; ++ iteration) {
        if (iteration % 10 == 0) {
            double clock_end = rdtsc();
            if (clock_end - clock_begin > 9.5) {
#ifdef LOCAL
cerr << "MESSAGE: iteration = " << iteration << endl;
cerr << "MESSAGE: ratio = " << best_score / estimate_base_score(s, matrix) << endl;
#endif
                break;
            }
            temp = max(0.0, 10 - (clock_end - clock_begin)) / 10 * s;
        }
        int x = bernoulli_distribution(0.5)(gen) ?
            uniform_int_distribution<int>(max(0, current.ly - 1), min(s, current.ry + 1) - 1)(gen) :
            uniform_int_distribution<int>(max(0, current.lx - 1), min(s, current.rx + 1) - 1)(gen);
        int y = uniform_int_distribution<int>(0, s - 1)(gen);
        swap(p[x], p[y]);
        auto next = analyze_permutation(p, matrix,
                max(0, current.ly - 5),
                max(0, current.lx - 5),
                min(s, current.ry + 5),
                min(s, current.rx + 5) );
        double delta = next.evaluated - current.evaluated;
        if (current.evaluated < next.evaluated + eps or bernoulli_distribution(exp(delta / temp))(gen)) {
            current.evaluated = next.evaluated;
            if (best_score < next.score) {
                best_score = next.score;
                current = next;
                result = p;
#ifdef VISUALIZE
visualize(p);
cerr << "MESSAGE: iteration = " << iteration << endl;
cerr << "MESSAGE: evaluated = " << int(current.evaluated) << endl;
cerr << "MESSAGE: score     = " << int(best_score) << endl;
#endif
            }
        } else {
            swap(p[x], p[y]);
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
