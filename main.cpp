#include <iostream>
#include <algorithm>
#include <numeric>
#include <vector>
#include <thread>
using namespace std;

void ejemplo_1_serial() {
    vector<int> vec = {
            1, 2, 3, 4, 5,          // subtotal 1
            6, 7, 8, 9, 10,         // subtotal 2
            11, 12, 13, 14, 15,     // subtotal 3
            16, 17, 18, 19, 20      // subtotal 4
    };

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
    vector<int> v_total(n_hilos);
    auto iter = begin(vec);
    auto riter = begin(v_total);
    thread t1([iter, riter]{ sumarizar3(iter, next(iter, 5), riter); });

    advance(riter, 1);
    iter = next(iter, 5);
    thread t2([iter, riter]{ sumarizar3(iter, next(iter, 5), riter); });

    advance(riter, 1);
    iter = next(iter, 5);
    thread t3([iter, riter]{ sumarizar3(iter, next(iter, 5), riter); });

    advance(riter, 1);
    iter = next(iter, 5);
    thread t4([iter, riter]{ sumarizar3(iter, next(iter, 5), riter); });
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    auto total = accumulate(begin(v_total), end(v_total), 0);
    cout << total << endl;
}

void ejemplo_5_paralelo() {
    vector<int> vec = {
            1, 2, 3, 4, 5,          // subtotal 1
            6, 7, 8, 9, 10,         // subtotal 2
            11, 12, 13, 14, 15,     // subtotal 3
            16, 17, 18,             // subtotal 4
    };

    int n_hilos = 4;
    vector<int> v_total(n_hilos);
    vector<thread> v_hilos(n_hilos);

    auto iter = begin(vec);
    auto riter = begin(v_total);

    for (auto& h: v_hilos) {
        h = thread([iter, riter]{ sumarizar3(iter, next(iter, 5), riter); });
        advance(riter, 1);
        iter = next(iter, 5);
    }
    for (auto& h: v_hilos) h.join();

    auto total = accumulate(begin(v_total), end(v_total), 0);
    cout << total << endl;
}

int main() {
    ejemplo_1_serial();
    ejemplo_2_paralelo();
    ejemplo_3_paralelo();
    ejemplo_4_paralelo();
    ejemplo_5_paralelo();
    return 0;
}
