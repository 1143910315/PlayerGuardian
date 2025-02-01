#include "algorithm/SPSCRingBuffer.hpp"
#include "mc/server/ServerLevel.h"
#include "Performance.h"
#include "thread/ThreadPool.h"
#include "webui/WebUI.h"
#include <chrono>
#include <functional>
#include <ll/api/memory/Hook.h>
#include <tuple>

static SPSCRingBuffer<std::tuple<std::chrono::steady_clock::time_point, std::chrono::steady_clock::time_point>, 64> tickTimes;

server::Performance &server::Performance::getInstance() {
    static Performance instance;
    return instance;
}

void server::Performance::tpsBeginRecord() {
    tpsEndRecord();
    tpsRecordFlag = true;
    recordTask = [this]() {
        if (tpsRecordFlag) {
            std::tuple<std::chrono::steady_clock::time_point, std::chrono::steady_clock::time_point> timePair;
            size_t tickCount = 0;
            size_t sum = 0;
            while (tickTimes.pop(timePair)) {
                tickCount++;
                // 结构化绑定解包时间点
                const auto& [start, end] = timePair;

                // 计算时间差并转换为微秒（平台兼容性关键）
                const auto us_diff = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
                sum += us_diff.count();
            }
            if (tickCount == 0) [[unlikely]] {
                tpsRecordData(0, 0);
            } else {
                tpsRecordData(tickCount, (sum + tickCount / 2) / tickCount);
            }
            sendTpsRecordData();
            thread::ThreadPool::defaultThreadPool().addDelayTask(std::chrono::milliseconds(1000), recordTask);
        } else {
            std::lock_guard lock(tpsRecordMutex);
            tpsRecordCV.notify_all();
        }
    };
    thread::ThreadPool::defaultThreadPool().addDelayTask(std::chrono::milliseconds(1000), recordTask);
}

void server::Performance::tpsEndRecord() {
    if (tpsRecordFlag) {
        std::unique_lock<std::mutex> lock(tpsRecordMutex);
        tpsRecordFlag = false;
        // 等待条件变量的通知
        tpsRecordCV.wait(lock);
    }
}

server::Performance::~Performance() {
    tpsEndRecord();
}

void server::Performance::tpsRecordData(size_t tps, size_t mspt) {
    if (tpsRecordDeque.size() > 2) {
        tpsRecordDeque.pop_front();
    }
    tpsRecordDeque.push_back(std::make_tuple(tps, mspt));
}

void server::Performance::sendTpsRecordData() {
    size_t totalTps = 0;
    size_t totalMspt = 0;
    size_t count = tpsRecordDeque.size();
    for (auto& [tps, mspt] : tpsRecordDeque) {
        totalTps += tps;
        totalMspt += mspt;
    }

    if (count == 0) [[unlikely]] {
        webuiServer::WebUI::getInstance().sendTpsRecordData(0, 0);
    } else {
        webuiServer::WebUI::getInstance().sendTpsRecordData(totalTps / count, (totalMspt + count / 2) / count / 1000);
    }
}

LL_AUTO_TYPE_INSTANCE_HOOK(PerformanceLevelTickEventHook, HookPriority::Lowest, ServerLevel, &ServerLevel::$_subTick, void) {
    // 记录开始时间
    auto start = std::chrono::steady_clock::now();
    origin();
    tickTimes.push(std::make_tuple(start, std::chrono::steady_clock::now()));
}
