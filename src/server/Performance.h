#pragma once
#include <atomic>
#include <mutex>
#include <tuple>

namespace server {
    class Performance {
    public:
        // 删除拷贝构造函数和赋值运算符
        Performance(const Performance&) = delete;
        Performance& operator=(const Performance&) = delete;

        // 获取单例实例
        static Performance& getInstance();

        // 开始记录tps
        void tpsBeginRecord();
        // 结束记录tps
        void tpsEndRecord();

    private:
        // 私有默认构造函数
        Performance() = default;
        // 私有析构函数
        ~Performance();

        // 记录tps以及mspt
        void tpsRecordData(size_t tps, size_t mspt);
        // 发送tps记录数据
        void sendTpsRecordData();

        // 是否开始记录tps
        std::atomic<bool> tpsRecordFlag = false;
        // 互斥锁
        std::mutex tpsRecordMutex;
        // 条件变量
        std::condition_variable tpsRecordCV;
        // 记录的tps以及mspt
        std::deque<std::tuple<size_t, size_t>> tpsRecordDeque;
        // 记录tps的线程
        std::function<void()> recordTask;
    };
} // namespace server
