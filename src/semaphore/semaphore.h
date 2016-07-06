#ifndef __SEMAPHORE_H_
#define __SEMAPHORE_H_

#include <mutex>
#include <condition_variable>

class Semaphore {
public:
    Semaphore(int cnt = 0) : cnt(cnt) {

    }

    void signal() {
        std::unique_lock<std::mutex> guard(mutex);
        ++cnt;
        cv.notify_one();
    }

    void wait() {
        std::unique_lock<std::mutex> guard(mutex);
        while(cnt == 0) {
            cv.wait(guard);
        }
        --cnt;
    }
private:
    int cnt;

    std::mutex mutex;
    std::condition_variable cv;
};

#endif