#include <iostream>
#include <vector>
#include <thread>
#include <chrono>

#include "ThreadPool.hpp"

struct Coordinates {
    int x;
    int y;

    void display() {
        std::cout << "x = " << x << ", y = " << y << std::endl;
    }
};

double add(double a, double b) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return a + b;
}

int main() {
    Coordinates coordinates {.x = 5, .y = 6};
    double a = 1.2;
    double b = 2.3;
    std::vector<std::future<int>> results;
    const int size = 5;

    thread::ThreadPool pool;
    
    pool.execute([](char c){std::cout << '{' << c << "}" << std::endl;}, 'Z');
    
    pool.execute(&Coordinates::display, coordinates);
    
    std::future<double> sum = pool.execute(add, a, b);
    std::cout << "Sum = " << sum.get() << std::endl;
    
    pool.addNewThread();
    
    for (int i = 0; i < size; ++i) {
        results.emplace_back(pool.execute([=]{return i * i;}));
    }   
    for (int i = 0; i < size; ++i) {
        std::cout << results[i].get() << " ";
    }
    std::cout << std::endl;
    
    std::cout << "Threads count - " << pool.size() << std::endl;

    pool.stop();
    try {
        pool.execute([](){});
    } catch (std::runtime_error ex) {
        std::cerr << ex.what() << std::endl;
    }
}