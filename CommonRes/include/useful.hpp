#pragma once

#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <queue>

template<class T>
using uptr = std::unique_ptr<T>;

template<class T>
class ThreadSafeQueue {
private:
    std::queue<T> queue;
    std::mutex mutex;
public:
    void Push(T t) {
        mutex.lock();
        queue.push(t);
        mutex.unlock();
    }

    T Pop() {
        mutex.lock();
		if (queue.empty())
			return nullptr;
        T t = queue.front(); 
		queue.pop();
        mutex.unlock();
        return t;
    }

    bool Empty() {
        mutex.lock();
        bool empty = queue.empty();
        mutex.unlock();
        return empty;
    }
};

class Log {
public:
	Log& operator<<(char c) {
		std::cout << c << ' ';
		return *this;
	}

	Log& operator<<(long long int a) {
		std::cout << a << ' ';
		return *this;
	}

	Log& operator<<(std::string str) {
		std::cout << str << ' ';
		return *this;
	}

	Log& operator<<(std::ostream& f(std::ostream &os)) {
		std::cout << std::endl;
		return *this;
	}
};