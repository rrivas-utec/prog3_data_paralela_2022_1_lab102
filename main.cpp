#include <iostream>
#include <algorithm>
#include <numeric>
#include <vector>
#include <thread>
#include <cmath>
#include <fstream>
#include <random>
#include <iterator>
#include <future>
using namespace std;

int rand_int(int first, int last) {
    random_device rd;                               // equivalente: vaso
    uniform_int_distribution<int> ud(first, last);  // equivalente a dados
    return ud(rd);
}

void generar_archivo(const string& name_file, int cantidad) {
    ofstream file(name_file);
    for (int i = 0; i < cantidad; ++i)
        file << rand_int(1, 100) << " ";
}

vector<int> generar_vector(const string& name_file) {
    vector<int> result;
    ifstream file(name_file);
    copy(istream_iterator<int>(file), istream_iterator<int>(), back_inserter(result));
    return result;
}

void ejemplo_1_serial() {
//    vector<int> vec = {
//            1, 2, 3, 4, 5,          // subtotal 1
//            6, 7, 8, 9, 10,         // subtotal 2
//            11, 12, 13, 14, 15,     // subtotal 3
//            16, 17, 18, 19, 20      // subtotal 4
//    };
    vector<int> vec = generar_vector("../dato18.txt");

    auto total = accumulate(begin(vec), end(vec), 0);
    cout << total << endl;
}

auto sumarizar = [](int start, int end, vector<int>& v, int& total) {
    total = accumulate(
            next(begin(v), start),
            next(begin(v), end + 1),
            0
    );
};

void ejemplo_2_paralelo() {
    vector<int> vec = {
            1, 2, 3, 4, 5,          // subtotal 1
            6, 7, 8, 9, 10,         // subtotal 2
            11, 12, 13, 14, 15,     // subtotal 3
            16, 17, 18, 19, 20      // subtotal 4
    };
    int s1 = 0;
    thread t1(sumarizar, 0, 4, ref(vec), ref(s1));
    int s2 = 0;
    thread t2(sumarizar, 5, 9, ref(vec), ref(s2));
    int s3 = 0;
    thread t3(sumarizar, 10, 14, ref(vec), ref(s3));
    int s4 = 0;
    thread t4(sumarizar, 15, 19, ref(vec), ref(s4));
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    auto total = s1 + s2 + s3 + s4;
    cout << total << endl;
}

template <typename Iterator>
void sumarizar2 (Iterator start, Iterator stop, int& total) {
    total = accumulate(start, stop, 0);
};

void ejemplo_3_paralelo() {
    vector<int> vec = {
            1, 2, 3, 4, 5,          // subtotal 1
            6, 7, 8, 9, 10,         // subtotal 2
            11, 12, 13, 14, 15,     // subtotal 3
            16, 17, 18, 19, 20      // subtotal 4
    };
    int s1 = 0;
    auto iter = begin(vec);
//    thread t1(sumarizar2<vector<int>::iterator>, iter, next(iter, 5), ref(s1))
    thread t1([iter, &s1]{ sumarizar2(iter, next(iter, 5), ref(s1)); });
    int s2 = 0;
    iter = next(iter, 5);
    thread t2([iter, &s2]{ sumarizar2(iter, next(iter, 5), ref(s2)); });
    int s3 = 0;
    iter = next(iter, 5);
    thread t3([iter, &s3]{ sumarizar2(iter, next(iter, 5), ref(s3)); });
    int s4 = 0;
    iter = next(iter, 5);
    thread t4([iter, &s4]{ sumarizar2(iter, next(iter, 5), ref(s4)); });
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    auto total = s1 + s2 + s3 + s4;
    cout << total << endl;
}

template <typename Iterator>
void sumarizar3 (Iterator start, Iterator stop, Iterator result) {
    *result = accumulate(start, stop, 0);
};

void ejemplo_4_paralelo() {
    vector<int> vec = {
            1, 2, 3, 4, 5,          // subtotal 1
            6, 7, 8, 9, 10,         // subtotal 2
            11, 12, 13, 14, 15,     // subtotal 3
            16, 17, 18, 19, 20      // subtotal 4
    };
    int n_hilos = 4;
    int rango = size(vec) / n_hilos;

    vector<int> v_total(n_hilos);
    auto iter = begin(vec);
    auto riter = begin(v_total);


    thread t1([iter, riter, rango]{ sumarizar3(iter, next(iter, rango), riter); });
    advance(riter, 1);
    iter = next(iter, rango);

    thread t2([iter, riter, rango]{ sumarizar3(iter, next(iter, rango), riter); });
    advance(riter, 1);
    iter = next(iter, 5);

    thread t3([iter, riter, rango]{ sumarizar3(iter, next(iter, rango), riter); });
    advance(riter, 1);
    iter = next(iter, 5);

    thread t4([iter, riter, rango]{ sumarizar3(iter, next(iter, rango), riter); });
    advance(riter, 1);
    iter = next(iter, 5);

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    auto total = accumulate(begin(v_total), end(v_total), 0);
    cout << total << endl;
}

void ejemplo_5_paralelo() {
//    vector<int> vec = {
//            1, 2, 3, 4, 5,          // subtotal 1
//            6, 7, 8, 9, 10,         // subtotal 2
//            11, 12, 13, 14, 15,     // subtotal 3
//            16, 17, 18,             // subtotal 4
//    };
    auto vec = generar_vector("../dato18.txt");
    int n_hilos = 4;
    vector<int> v_total(n_hilos);
    vector<thread> v_hilos(n_hilos);
    int rango = ceil(size(vec) * 1.0 / n_hilos);

    auto iter = begin(vec);
    auto riter = begin(v_total);

    for (auto& h: v_hilos) {
        if (distance(iter, end(vec)) < rango)
            rango = distance(iter, end(vec));

        h = thread([iter, riter, rango]{ sumarizar3(iter, next(iter, rango), riter); });
        ++riter;
        iter = next(iter, rango);
    }
    for (auto& h: v_hilos) h.join();

    auto total = accumulate(begin(v_total), end(v_total), 0);
    cout << total << endl;
}

const int expected_range = 25;
int get_number_of_threads(int sz, int rng) {
    int max_threads = (sz + rng - 1) / rng;
    int k_threads = static_cast<int>(thread::hardware_concurrency());
    return min(k_threads != 0? k_threads: 2, max_threads);
}

template <typename Iterator, typename T = typename Iterator::value_type>
T acumular(Iterator start, Iterator stop, T initial) {
    int sz = distance(start, stop);
    int n_hilos = get_number_of_threads(sz, expected_range);
    vector<int> v_total(n_hilos);
    vector<thread> v_hilos(n_hilos);
    int rango = ceil(sz * 1.0 / n_hilos);

    auto iter = start;
    auto riter = begin(v_total);

    for (auto& h: v_hilos) {
        if (distance(iter, stop) < rango)
            rango = distance(iter, stop);

//        h = thread(accumulate)
        h = thread([iter, riter, rango]{
            *riter = accumulate(iter, next(iter, rango), 0);
        });
        ++riter;
        iter = next(iter, rango);
    }
    for (auto& h: v_hilos) h.join();
    return accumulate(begin(v_total), end(v_total), initial);
}

void ejemplo_6_paralelo() {
    auto vec = generar_vector("../dato18.txt");
    cout << acumular(begin(vec), end(vec), 0) << endl;
}

template <typename Iterator, typename T = typename Iterator::value_type>
T acumular_async(Iterator start, Iterator stop, T initial) {
    int sz = distance(start, stop);
    int n_hilos = get_number_of_threads(sz, expected_range);
    vector<future<T>> v_futures(n_hilos);
    int rango = ceil(sz * 1.0 / n_hilos);

    auto iter = start;
    for (auto& f: v_futures) {
        if (distance(iter, stop) < rango)
            rango = distance(iter, stop);
        f = async(accumulate<Iterator, T>, iter, next(iter, rango), 0);
        iter = next(iter, rango);
    }

    T total = T{};
    for (auto& f: v_futures) total += f.get();
    return total + initial;
}

void ejemplo_7_paralelo() {
    auto vec = generar_vector("../dato18.txt");
    cout << acumular_async(begin(vec), end(vec), 0) << endl;
}

int sumar(int a, int b) {
    return a + b;
}

void ejemplo_async() {
    future<int> res = async(sumar, 10, 20);
    cout << res.get() << endl;
}

template <typename Iterator>
auto accumulator_rec_async_(Iterator start, Iterator stop, int range) {
    // Condición Base
    auto size = distance(start, stop);
    if (size < range) return accumulate(start, stop, 0);
    // Condicion Recursiva
    auto middle = next(start, size / 2);
    auto a = async(accumulator_rec_async_<Iterator>, start, middle, range);
    auto b = async(accumulator_rec_async_<Iterator>, middle, stop, range);
    return a.get() + b.get();
}

template <typename Iterator, typename T = typename Iterator::value_type>
T accumulator_recursive_async(Iterator start, Iterator stop, T initial) {
    auto size = distance(start, stop);
    int n_hilos = get_number_of_threads(size, expected_range);
    auto rango = size / n_hilos;
    return initial + accumulator_rec_async_(start, stop, rango) ;
}

int main() {
//    generar_archivo("../dato18.txt", 18);
    ejemplo_1_serial();
    ejemplo_2_paralelo();
    ejemplo_3_paralelo();
    ejemplo_4_paralelo();
    ejemplo_5_paralelo();
    ejemplo_6_paralelo();
    ejemplo_7_paralelo();
    ejemplo_async();
    return 0;
}
