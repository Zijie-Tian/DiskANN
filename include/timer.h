// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

#include <chrono>

namespace diskann
{
class Timer
{
    typedef std::chrono::high_resolution_clock _clock;
    std::chrono::time_point<_clock> check_point;

  public:
    Timer() : check_point(_clock::now())
    {
    }

    void reset()
    {
        check_point = _clock::now();
    }

    long long elapsed() const
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(_clock::now() - check_point).count();
    }

    float elapsed_seconds() const
    {
        return (float)elapsed() / 1000000.0f;
    }

    std::string elapsed_seconds_for_step(const std::string &step) const
    {
        return std::string("Time for ") + step + std::string(": ") + std::to_string(elapsed_seconds()) +
               std::string(" seconds");
    }
};

class GlobalTimer {
private:
using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;

TimePoint start_time;

// 私有的构造函数，以确保外部不能实例化
GlobalTimer() : start_time(Clock::now()) {
}

public:
// 删除复制构造函数和赋值操作
GlobalTimer(const GlobalTimer&) = delete;
GlobalTimer& operator=(const GlobalTimer&) = delete;

// 提供一个静态方法来获取单例实例
//! 第一次要调用这个。
static GlobalTimer& getInstance() {
    static GlobalTimer
        instance;  // Guaranteed to be destroyed. Instantiated on first use.
    return instance;
}

long long elapsed() const {
    auto current_time = Clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
        current_time - start_time);
    return duration.count();
}

void reset() {
    start_time = Clock::now();
}
};

} // namespace diskann
