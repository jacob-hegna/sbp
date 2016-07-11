#ifndef __TIMER_DEBUG_H_
#define __TIMER_DEBUG_H_

template<typename T>
class TimerWrapper {
public:
    TimerWrapper(T function, std::string msg) : call(function),
                    start(std::chrono::system_clock::now()) {
        std::cout << msg << "..." << std::flush;
    }

    ~TimerWrapper() {
        end = std::chrono::system_clock::now();
        dur = end - start;

        std::cout << "done! [" << dur.count() << "]" << std::endl;
    }

    T call;

private:
    std::chrono::time_point<std::chrono::system_clock> start;
    std::chrono::time_point<std::chrono::system_clock> end;
    std::chrono::duration<double>                      dur;

    T function;
};

template<typename T>
TimerWrapper<T> time_debug(T f, std::string msg) {
    return TimerWrapper<T>(f, msg);
}

#endif