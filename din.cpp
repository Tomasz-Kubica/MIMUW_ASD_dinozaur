#include <iostream>
#include <cassert>
#include <utility>

using namespace std;

struct seq_info_t {
    int size;
    int longest_seq;
    int left_seq;
    int right_seq;
    char left_seq_t;
    char right_seq_t;

    explicit seq_info_t(const char &k) : size(1), longest_seq(1), left_seq(1),
                                         right_seq(1), left_seq_t(k),
                                         right_seq_t(k) {}

    seq_info_t(const seq_info_t &l, const char &k) {
        if (!l.size) {
            *this = seq_info_t(k);
            return;
        }
        *this = l;
        size++;
        if (l.right_seq_t == k) {
            right_seq++;
        } else {
            right_seq = 1;
            right_seq_t = k;
        }
        if (l.left_seq == l.size && l.left_seq_t == k)
            left_seq++;
        longest_seq = max(longest_seq, right_seq);
    }

    seq_info_t(const char &k, const seq_info_t &r) {
        if (!r.size) {
            *this = seq_info_t(k);
            return;
        }
        *this = r;
        size++;
        if (r.left_seq_t == k) {
            left_seq++;
        } else {
            left_seq = 1;
            left_seq_t = k;
        }
        if (r.right_seq == r.size && r.right_seq_t == k)
            right_seq++;
        longest_seq = max(longest_seq, left_seq);
    }

    seq_info_t(const seq_info_t &l, const seq_info_t &r) : size(
            l.size + r.size) {
        if (l.size == 0) {
            *this = r;
            return;
        } else if (r.size == 0) {
            *this = l;
            return;
        }

        longest_seq = max(l.longest_seq, r.longest_seq);
        if (l.right_seq_t == r.left_seq_t)
            longest_seq = max(longest_seq, l.right_seq + r.left_seq);
        left_seq_t = l.left_seq_t;
        right_seq_t = r.right_seq_t;
        left_seq = l.left_seq;
        right_seq = r.right_seq;
        if (l.left_seq == l.size && l.left_seq_t == r.left_seq_t)
            left_seq = l.left_seq + r.left_seq;
        if (r.right_seq == r.size && r.right_seq_t == l.right_seq_t)
            right_seq = r.right_seq + l.right_seq;
    }

    seq_info_t(const seq_info_t &l, const char &k, const seq_info_t &r)
            : size(l.size + r.size + 1)
    {
        if (!l.size && !r.size) {
            *this = seq_info_t(k);
            return;
        } else if (!l.size) {
            *this = seq_info_t(k, r);
            return;
        } else if (!r.size) {
            *this = seq_info_t(l, k);
            return;
        }

        left_seq_t = l.left_seq_t;
        left_seq = l.left_seq;
        if (left_seq == l.size && left_seq_t == k) {
            left_seq++;
            if (k == r.left_seq_t)
                left_seq += r.left_seq;
        }

        right_seq_t = r.right_seq_t;
        right_seq = r.right_seq;
        if (right_seq == r.size && r.right_seq_t == k) {
            right_seq++;
            if (k == l.right_seq_t)
                right_seq += l.right_seq;
        }

        longest_seq = max(l.longest_seq, r.longest_seq);
        if (l.right_seq_t == k && k == r.left_seq_t) {
            longest_seq = max(longest_seq, l.right_seq + r.left_seq  + 1);
        } else if (l.right_seq_t == k) {
            longest_seq = max(longest_seq, l.right_seq + 1);
        } else if (k == r.left_seq_t) {
            longest_seq = max(longest_seq, r.left_seq + 1);
        }
    }

    constexpr seq_info_t() : size(0), longest_seq(0), left_seq(0), right_seq(0),
                             left_seq_t(0), right_seq_t(0) {}
};

seq_info_t reverse_seq(seq_info_t seq) {
    swap(seq.left_seq, seq.right_seq);
    swap(seq.right_seq_t, seq.left_seq_t);
    return seq;
}

const static constexpr seq_info_t empty_seq;

struct Splay_tree {
    char key;
    Splay_tree *father;
    Splay_tree *left_s = nullptr;
    Splay_tree *right_s = nullptr;

    int count;

    seq_info_t seq_info;
    bool reverse = false;

    Splay_tree(const char &k, Splay_tree *f) : key(k), father(f), count(1),
                                               seq_info(k) {}
};

int get_count(Splay_tree *T) {
    if (T == nullptr)
        return 0;
    else
        return T->count;
}

seq_info_t get_seq_info(Splay_tree *T) {
    if (T == nullptr)
        return empty_seq;
    if (T->reverse)
        return reverse_seq(T->seq_info);
    else
        return T->seq_info;
}

void update(Splay_tree *T) {
    //assert(T != nullptr);
    T->count = get_count(T->left_s) + get_count(T->right_s) + 1;
    T->seq_info = seq_info_t(get_seq_info(T->left_s), T->key,
                             get_seq_info(T->right_s));
}

void push_down_reverse(Splay_tree *T) {
    //assert(T != nullptr);
    if (!T->reverse)
        return;
    T->reverse = false;
    if (T->left_s != nullptr)
        T->left_s->reverse = !T->left_s->reverse;
    if (T->right_s != nullptr)
        T->right_s->reverse = !T->right_s->reverse;
    swap(T->left_s, T->right_s);
    update(T);
}

void splay(Splay_tree *T);

void splay(const int &n, Splay_tree **T);

/*pair<Splay_tree *, Splay_tree *>
_add(int n, Splay_tree *k, Splay_tree *T, Splay_tree *F) {
    pair<Splay_tree *, Splay_tree *> res;
    if (T == nullptr) {
        assert(n == 0);
        auto tmp = k;
        k->father = F;
        return {tmp, tmp};
    }
    push_down_reverse(T);
    if (n > get_count(T->left_s)) {
        res = _add(n - get_count(T->left_s) - 1, k, T->right_s, T);
        T->right_s = res.first;
    } else {
        res = _add(n, k, T->left_s, T);
        T->left_s = res.first;
    }
    update(T);
    return {T, res.second};
}

void add(int n, int k, Splay_tree **T) {
    auto res = _add(n, new Splay_tree(k, nullptr), *T, nullptr);
    splay(res.second);
    *T = res.second;
}

// przejmuje k
void add(int n, Splay_tree *k, Splay_tree **T) {
    auto res = _add(n, k, *T, nullptr);
    splay(res.second);
    *T = res.second;
}*/

Splay_tree *_find(int k, Splay_tree *T) {
    while (true) {
        push_down_reverse(T);
        auto left = T->left_s;
        auto right = T->right_s;
        int c = get_count(left);
        if (c == k) {
            return T;
        } else if (c < k) {
            k = k - c - 1;
            T = right;
        } else {
            T = left;
        }
    }
}

int find(const int &k, Splay_tree **T) {
    auto res = _find(k, *T);
    splay(res);
    *T = res;
    return res->key;
}

/*pair<Splay_tree *, Splay_tree *> _remove(int n, Splay_tree *T, Splay_tree *F) {
    assert(T != nullptr);
    push_down_reverse(T);
    int c = get_count(T->left_s);
    if (c == n) {
        if (T->right_s == nullptr && T->left_s == nullptr) {
            free(T);
            return {nullptr, F};
        } else if (T->right_s == nullptr) {
            auto tmp = T->left_s;
            tmp->father = F;
            free(T);
            return {tmp, F};
        } else if (T->left_s == nullptr) {
            auto tmp = T->right_s;
            tmp->father = F;
            free(T);
            return {tmp, F};
        } else {
            int new_key = _find(0, T->right_s)->key; /// ???
            auto res = _remove(0, T->right_s, T);
            T->right_s = res.first;
            T->key = new_key;
            return {T, res.second};
        }
    } else if (c > n) {
        auto res = _remove(n, T->left_s, T);
        T->left_s = res.first;
        return {T, res.second};
    } else {
        auto res = _remove(n - c - 1, T->right_s, T);
        T->right_s = res.first;
        return {T, res.second};
    }
}

void remove(int n, Splay_tree **T) {
    auto res = _remove(n, *T, nullptr);
    splay(res.second);
    *T = res.second;
}*/

void swap_child(Splay_tree *T, Splay_tree *C, Splay_tree *NC) {
    if (T != nullptr) {
        //assert(T->left_s == C || T->right_s == C);
        if (T->left_s == C)
            T->left_s = NC;
        else if (T->right_s == C)
            T->right_s = NC;
        else
            assert(false);
    }
}

seq_info_t _max_seq(Splay_tree *T, const int &i, const int &j) {
    if (T == nullptr) return empty_seq;
    //assert(0 <= i && i <= j && j < T->count);
    if (i == 0 && j == T->count - 1) {
        return get_seq_info(T);
    } else {
        push_down_reverse(T);
        seq_info_t res = empty_seq;
        int cl = get_count(T->left_s);
        if (i <= cl && cl <= j) res = seq_info_t(T->key);
        if (i < cl) {
            res = seq_info_t(_max_seq(T->left_s, i, min(j, cl - 1)), res);
        }
        if (cl < j)
            res = seq_info_t(res, _max_seq(T->right_s, max(0, i - cl - 1),
                                           j - cl - 1));
        return res;
    }
}

int max_seq(Splay_tree **T, const int &i, const int &j) {
    //splay(max(0, i - 1), T);
    splay(i, T);
    return _max_seq(*T, i, j).longest_seq;
}

void rotate_right(Splay_tree *X) {
    Splay_tree *Y = X->father;

    Splay_tree *B = X->right_s;

    X->father = Y->father;
    swap_child(Y->father, Y, X);

    X->right_s = Y;
    Y->father = X;

    Y->left_s = B;
    if (B != nullptr) B->father = Y;

    update(Y);
    update(X);
}

void rotate_left(Splay_tree *Y) {
    Splay_tree *X = Y->father;

    Splay_tree *B = Y->left_s;

    Y->father = X->father;
    swap_child(X->father, X, Y);

    Y->left_s = X;
    X->father = Y;

    X->right_s = B;
    if (B != nullptr) B->father = X;

    update(X);
    update(Y);
}

// -1 - left, 0 - no father, 1 - right
int which_child(Splay_tree *T) {
    //assert(T != nullptr);
    if (T->father == nullptr)
        return 0;
    if (T->father->left_s == T)
        return -1;
    if (T->father->right_s == T)
        return 1;
    cerr << "which_child" << endl;
    assert(false);
}

void local_splay(Splay_tree *T) {
    if (T == nullptr || T->father == nullptr)
        return;
    push_down_reverse(T->father);
    push_down_reverse(T);
    if (T->father->father == nullptr) {
        int which = which_child(T);
        if (which == -1)
            rotate_right(T);
        else
            rotate_left(T);
    } else {
        int which_1 = which_child(T);
        int which_2 = which_child(T->father);
        if (which_1 == -1 && which_2 == -1) {
            rotate_right(T);
            rotate_right(T);
        } else if (which_1 == 1 && which_2 == 1) {
            rotate_left(T);
            rotate_left(T);
        } else if (which_1 == 1 && which_2 == -1) {
            rotate_left(T);
            rotate_right(T);
        } else if (which_1 == -1 && which_2 == 1) {
            rotate_right(T);
            rotate_left(T);
        } else {
            assert(false);
        }
    }
}

void splay(Splay_tree *T) {
    while (T->father != nullptr) {
        local_splay(T);
    }
}

void splay(const int &n, Splay_tree **T) {
    *T = _find(n, *T);
    splay(*T);
}

void reverse_pref(Splay_tree **T, const int &n) {
    //assert(n < (*T)->count);
    if (n == (*T)->count - 1)
        (*T)->reverse = !(*T)->reverse;
    else {
        splay(n + 1, T);
        (*T)->left_s->reverse = !(*T)->left_s->reverse;
    }
}

void reverse_range(Splay_tree **T, const int &i, const int &j) {
    //assert(i >= 0 && i <= j && j < (*T)->count);
    reverse_pref(T, j);
    reverse_pref(T, j - i);
    reverse_pref(T, j);
}

void move_range(Splay_tree **T, const int &j, const int &l, const int &k) {
    //assert(k >= 0);
    int a, b, c;

    if (k >= j) {
        a = j;
        b = l - j + 1;
        c = k - j;
    } else {
        a = k;
        b = j - k;
        c = l - j + 1;
    }
    if (a + b - 1 >= 0) reverse_pref(T, a + b - 1);
    if (a + b + c - 1 >= 0) reverse_pref(T, a + b + c - 1);
    if (a + c - 1 >= 0) reverse_pref(T, a + c - 1);
    if (a - 1 >= 0) reverse_pref(T, a - 1);
}

/*bool compare_seq(const seq_info_t &a, const seq_info_t &b) {
    return a.size == b.size && a.right_seq == b.right_seq &&
           a.left_seq == b.left_seq && a.left_seq_t == b.left_seq_t &&
           a.right_seq_t == b.right_seq_t && a.longest_seq == b.longest_seq;
}*/

/*void safe_print(Splay_tree *T, const string &pref) {
    if (T == nullptr) {
        cout << pref << '\n';
        return;
    }
    if (T->reverse) {
        cout << pref << T->key << " reverse" << '\n';
    } else {
        cout << pref << T->key << " normal" << '\n';
    }
    safe_print(T->left_s, pref + "--");
    safe_print(T->right_s, pref + "--");
}*/

char DNA[1000000 + 5];
string DNA_string;

Splay_tree *create_tree(const int &p, const int &k, Splay_tree *f) {
    if (p == k)
        return new Splay_tree(DNA[p], f);

    int s = (p + k) / 2;
    auto new_node = new Splay_tree(DNA[s], f);
    if (p < s)
        new_node->left_s = create_tree(p, s - 1, new_node);
    if (k > s)
        new_node->right_s = create_tree(s + 1, k, new_node);
    update(new_node);
    return new_node;
}

// przejmuje L na własność
void merge(Splay_tree *L, Splay_tree **R) {
    if (*R == nullptr) {
        *R = L;
        return;
    } else if (L == nullptr)
        return;
    splay(0, R);
    (*R)->left_s = L;
    L->father = *R;
    update(*R);
}

// przekazany wskaźnik staje się zdezaktualizowany
pair<Splay_tree*, Splay_tree*> split(Splay_tree *T, const int &n) {
    if (T == nullptr)
        return {nullptr, nullptr};
    if (T->count <= n)
        return {T, nullptr};
    splay(n, &T);
    //push_down_reverse(T); //???
    Splay_tree *L = T->left_s;
    T->left_s = nullptr;
    if (L != nullptr)
        L->father = nullptr;
    update(T);
    return {L, T};
}

int new_max_seq(Splay_tree **T, const int &i, const int &j) {
    auto firstSplit = split(*T, j + 1);
    auto A = firstSplit.first;
    auto C = firstSplit.second;
    auto secondSplit = split(A, i);
    A = secondSplit.first;
    auto B = secondSplit.second;
    int r = B->seq_info.longest_seq;
    merge(A, &B);
    merge(B, &C);
    *T = C;
    return r;
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);

    Splay_tree *T = nullptr;

    int N, M;
    cin >> N >> M;
    cin >> DNA_string;
    for (int i = 0; i < N; i++) {
        DNA[i] = DNA_string[i];
    }

    T = create_tree(0, N - 1, nullptr);

//    cout << "Input_test" << endl;
//    return 0;

    string what;
    int x, y, z;
    for (int i = 0; i < M; i++) {
        cin >> what;
        if (what == "O") {
            cin >> x >> y;
            reverse_range(&T, x - 1, y - 1);
        } else if (what == "P") {
            cin >> x >> y >> z;
            move_range(&T, x - 1, y - 1, z - 1);
        } else if (what == "N") {
            cin >> x >> y;
            cout << new_max_seq(&T, x - 1, y - 1) << '\n';
        } /*else {
            //assert(false);
            int n = T->count;
            for (int i = 0; i < n; i++)
                cout << find(i, &T) << '\n';
            cout << '\n';
        }*/
    }

    return 0;
}