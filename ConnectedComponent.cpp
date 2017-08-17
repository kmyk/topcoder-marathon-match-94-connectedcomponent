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

class ConnectedComponent { public: vector<int> permute(vector<int> matrix); };

int calculate_score(vector<int> const & p, vector<char> const & matrix) {
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
    int result = 0;
    repeat (y, s) repeat (x, s) {
        if (not used[y * s + x] and at(y, x)) {
            size = acc = 0;
            go(y, x);
            setmax(result, int(acc * sqrt(size)));
        }
    }
    return result;
}

namespace disjoint_sets {
    typedef vector<int> type;
    type construct(size_t n) { return type(n, -1); }
    bool is_root(type const & data, int i) { return data[i] < 0; }
    int find_root(type & data, int i) { return is_root(data, i) ? i : (data[i] = find_root(data, data[i])); }
    int set_size(type & data, int i) { return - data[find_root(data, i)]; }
    int unite_sets(type & data, int i, int j) {
        i = find_root(data, i); j = find_root(data, j);
        if (i != j) {
            if (set_size(data, i) < set_size(data, j)) swap(i,j);
            data[i] += data[j];
            data[j] = i;
        }
        return i;
    }
    bool is_same(type & data, int i, int j) { return find_root(data, i) == find_root(data, j); }
    void compress(type & data) {
        repeat (i, data.size()) {
            find_root(data, i);
        }
    }

    namespace without_compression {
        int find_root(type const & data, int i) { while (not is_root(data, i)) i = data[i]; return i; }
        int set_size(type const & data, int i) { return - data[without_compression::find_root(data, i)]; }
        int unite_sets(type & data, int i, int j, vector<tuple<int, int, int> > & history) {
            i = without_compression::find_root(data, i);
            j = without_compression::find_root(data, j);
            if (i != j) {
                if (without_compression::set_size(data, i) < without_compression::set_size(data, j)) swap(i,j);
                history.emplace_back(i, j, data[j]);
                data[i] += data[j];
                data[j] = i;
            }
            return i;
        }
        bool is_same(type const & data, int i, int j) { return without_compression::find_root(data, i) == without_compression::find_root(data, j); }
        void undo_history(type & data, tuple<int, int, int> history) {
            int i, j, data_j; tie(i, j, data_j) = history;
            data[j] = data_j;
            data[i] -= data[j];
        }
        void undo_history(type & data, vector<tuple<int, int, int> > const & history) {
            repeat_reverse (i, history.size()) {
                undo_history(data, history[i]);
            }
        }
    }
}

int unite_sets_with_sum(disjoint_sets::type & ds, int i, int j, vector<int> & sum) {
    using namespace disjoint_sets;
    i = find_root(ds, i);
    j = find_root(ds, j);
    if (i == j) return i;
    int k = unite_sets(ds, i, j);
    sum[k] = sum[i] + sum[j];
    return k;
}
int unite_sets_with_sum_without_compression(disjoint_sets::type & ds, int i, int j, vector<int> & sum, vector<tuple<int, int, int, int> > & history) {
    using namespace disjoint_sets;
    i = without_compression::find_root(ds, i);
    j = without_compression::find_root(ds, j);
    if (i == j) return i;
    if (without_compression::set_size(ds, i) < without_compression::set_size(ds, j)) swap(i,j);
    history.emplace_back(i, j, ds[j], sum[i]);
    ds[i] += ds[j];
    sum[i] += sum[j];
    ds[j] = i;
    return i;
}
void undo_history_with_sum(disjoint_sets::type & ds, vector<int> & sum, tuple<int, int, int, int> history) {
    int i, j, ds_j, sum_i; tie(i, j, ds_j, sum_i) = history;
    ds[j] = ds_j;
    sum[i] = sum_i;
    ds[i] -= ds[j];
}

// square-root-decomposed union-find tree with the sum of matrix's values
pair<disjoint_sets::type, vector<int> > make_decomposed_regions(int s, int bucket_size, int bucket_count, vector<int> const & p, vector<char> const & matrix) {
    disjoint_sets::type ds = disjoint_sets::construct(matrix.size());
    vector<int> sum(matrix.size());
    repeat (y, s) {
        repeat (x, s) {
            sum[y * s + x] = matrix[p[y] * s + p[x]];
        }
    }
    repeat (by, bucket_count) {
        int ly = by * bucket_size;
        int ry = min(s, ly + bucket_size);
        repeat (bx, bucket_count) {
            int lx = bx * bucket_size;
            int rx = min(s, lx + bucket_size);
            repeat_from (y, ly, ry) repeat_from (x, lx, rx) {
                if (matrix[p[y] * s + p[x]]) {
                    if (y + 1 < ry and matrix[p[y + 1] * s + p[x]]) {
                        unite_sets_with_sum(ds, y * s + x, (y + 1) * s + x, sum);
                    }
                    if (x + 1 < rx and matrix[p[y] * s + p[x + 1]]) {
                        unite_sets_with_sum(ds, y * s + x, y * s + (x + 1), sum);
                    }
                }
            }
        }
    }
    disjoint_sets::compress(ds);
    return { ds, sum };
}

vector<multimap<int, int> > list_decomposed_regions_with_score(int s, int bucket_size, int bucket_count, disjoint_sets::type const & ds, vector<int> const & sum) {
    vector<multimap<int, int> > scored_regions(bucket_count * bucket_count);
    repeat (by, bucket_count) {
        int ly = by * bucket_size;
        int ry = min(s, ly + bucket_size);
        repeat (bx, bucket_count) {
            int bz = by * bucket_count + bx;
            int lx = bx * bucket_size;
            int rx = min(s, lx + bucket_size);
            repeat_from (y, ly, ry) repeat_from (x, lx, rx) {
                int z = y * s + x;
                if (disjoint_sets::is_root(ds, z)) {
                    scored_regions[bz].emplace(int(sum[z] * sqrt(disjoint_sets::without_compression::set_size(ds, z))), z);
                }
            }
        }
    }
    return scored_regions;
}

int calculate_score_from_decomposed_regions(int s, int bucket_size, int bucket_count, disjoint_sets::type & ds, vector<int> & sum, vector<multimap<int, int> > const & scored_regions, vector<int> const & p, vector<char> const & matrix) {
    set<int> modified;
    vector<tuple<int, int, int, int> > history;
    auto unite = [&](int y1, int x1, int y2, int x2) {
        if (matrix[p[y1] * s + p[x1]] and matrix[p[y2] * s + p[x2]]) {
            int z1 = y1 * s + x1;
            int z2 = y2 * s + x2;
            z1 = disjoint_sets::without_compression::find_root(ds, z1);
            z2 = disjoint_sets::without_compression::find_root(ds, z2);
            modified.insert(z1);
            modified.insert(z2);
            unite_sets_with_sum_without_compression(ds, z1, z2, sum, history);
        }
    };
    repeat (by, bucket_count) {
        int ly = by * bucket_size;
        int ry = min(s, ly + bucket_size);
        repeat_from (y, ly, ry) {
            repeat (bx, bucket_count - 1) {
                int x = (bx + 1) * bucket_size - 1;
                unite(y, x, y, x + 1);
            }
        }
        if (ry < s) {
            repeat (x, s) {
                unite(ry - 1, x, ry, x);
            }
        }
    }
    int score = -1;
    for (int z : modified) {
        if (disjoint_sets::is_root(ds, z)) {
            setmax(score, int(sum[z] * sqrt(disjoint_sets::set_size(ds, z))));
        }
    }
    while (not history.empty()) {
        undo_history_with_sum(ds, sum, history.back());
        history.pop_back();
    }
    repeat (b, bucket_count * bucket_count) {
        for (auto it = scored_regions[b].rbegin(); it != scored_regions[b].rend(); ++ it) {
            if (it->first <= score) break;
            if (not modified.count(it->second)) {
                setmax(score, it->first);
                break;
            }
        }
    }
    return score;
}

void swap_on_decomposed_regions(int i, int j, vector<int> const & p, int s, int bucket_size, int bucket_count, disjoint_sets::type & ds, vector<int> & sum, vector<multimap<int, int> > & scored_regions, vector<char> const & matrix) {
    // assume p is already swapped: swap(p[i], p[j]);
    int bi = i / bucket_size;
    int bj = j / bucket_size;
    repeat (by, bucket_count) {
        int ly = by * bucket_size;
        int ry = min(s, ly + bucket_size);
        repeat (bx, bucket_count) {
            if (not (by == bi or by == bj or bx == bi or bx == bj)) continue;
            int lx = bx * bucket_size;
            int rx = min(s, lx + bucket_size);
            repeat_from (y, ly, ry) {
                repeat_from (x, lx, rx) {
                    int z = y * s + x;
                    ds[z] = -1;
                    sum[z] = matrix[p[y] * s + p[x]];
                    if (sum[z]) {
                        if (y - 1 >= ly and matrix[p[y - 1] * s + p[x]]) {
                            unite_sets_with_sum(ds, z, (y - 1) * s + x, sum);
                        }
                        if (x - 1 >= lx and matrix[p[y] * s + p[x - 1]]) {
                            unite_sets_with_sum(ds, z, y * s + (x - 1), sum);
                        }
                    }
                }
            }
            int bz = by * bucket_count + bx;
            scored_regions[bz].clear();
            repeat_from (y, ly, ry) {
                repeat_from (x, lx, rx) {
                    int z = y * s + x;
                    if (disjoint_sets::is_root(ds, z)) {
                        scored_regions[bz].emplace(int(sum[z] * sqrt(disjoint_sets::set_size(ds, z))), z);
                    }
                }
            }
        }
    }
}

void visualize(vector<int> const & p) {
    cerr << "VISUALIZE: " << p.size();
    for (int p_i : p) cerr << ' ' << p_i;
    cerr << endl;
}

default_random_engine gen;
vector<int> ConnectedComponent::permute(vector<int> int_matrix) {
    // prepare
    double clock_begin = rdtsc();
    vector<char> matrix(whole(int_matrix));
    int s = (int)sqrt(matrix.size());
cerr << "s = " << s << endl;
    int bucket_size = ceil(sqrt(s));
    int bucket_count = (s + bucket_size - 1) / bucket_size;
cerr << "bucket_size = " << bucket_size << endl;
cerr << "bucket_count = " << bucket_count << endl;
    // make initial p
    vector<int> p(s);
    iota(whole(p), 0);
visualize(p);
    disjoint_sets::type ds; vector<int> sum; tie(ds, sum) = make_decomposed_regions(s, bucket_size, bucket_count, p, matrix);
    vector<multimap<int, int> > scored_regions = list_decomposed_regions_with_score(s, bucket_size, bucket_count, ds, sum);
    // simulated annealing
    int current_score = calculate_score(p, matrix);
assert (current_score == calculate_score_from_decomposed_regions(s, bucket_size, bucket_count, ds, sum, scored_regions, p, matrix));
    vector<int> result = p;
    int best_score = current_score;
    int force_count = 0;
    for (int iteration = 0; ; ++ iteration) {
        double temp = INFINITY;
        if (iteration % 10 == 0) {
            double clock_end = rdtsc();
            if (clock_end - clock_begin > 9.5) {
                break;
            }
            temp = (10 - (clock_end - clock_begin)) / 10 * s * s;
        }
        int x = -1, y = -1;
        while (x == y) {
            x = uniform_int_distribution<int>(0, s - 1)(gen);
            y = uniform_int_distribution<int>(0, s - 1)(gen);
        }
        swap(p[x], p[y]);
        int next_score = calculate_score(p, matrix);
        int delta = next_score - current_score;
        if (current_score <= next_score or bernoulli_distribution(exp(delta / temp))(gen)) {
            if (current_score > next_score) force_count += 1;
            current_score = next_score;
swap_on_decomposed_regions(x, y, p, s, bucket_size, bucket_count, ds, sum, scored_regions, matrix);
int decomposed_score = calculate_score_from_decomposed_regions(s, bucket_size, bucket_count, ds, sum, scored_regions, p, matrix);
assert (current_score == decomposed_score);
            if (best_score < current_score) {
                best_score = current_score;
                result = p;
visualize(p);
cerr << "MESSAGE: iteration = " << iteration << " / force = " << force_count << endl;
cerr << "score = " << current_score << endl;
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
