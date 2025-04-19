#include "algorithm/SPSCRingBuffer.hpp"
#include "mc/server/ServerLevel.h"
#include "Performance.h"
#include "thread/ThreadPool.h"
#include "webui/WebUI.h"
#include <chrono>
#include <functional>
#include <ll/api/base/StdInt.h>
#include <ll/api/chrono/GameChrono.h>
#include <ll/api/coro/CoroTask.h>
#include <ll/api/event/EventBus.h>
#include <ll/api/event/Listener.h>
#include <ll/api/event/server/ServerStoppingEvent.h>
#include <ll/api/Expected.h>
#include <ll/api/memory/Hook.h>
#include <ll/api/service/Bedrock.h>
#include <ll/api/thread/ServerThreadExecutor.h>
#include <ll/api/thread/ThreadPoolExecutor.h>
#include <mc/deps/core/math/Vec3.h>
#include <mc/legacy/ActorRuntimeID.h>
#include <mc/world/actor/Actor.h>
#include <mc/world/actor/ActorCategory.h>
#include <mc/world/actor/ActorFlags.h>
#include <mc/world/actor/item/ItemActor.h>
#include <mc/world/actor/provider/SynchedActorDataAccess.h>
#include <mc/world/level/dimension/Dimension.h>
#include <memory>
#include <mutex>
#include <tuple>
#include <utility>
#include <vector>

using namespace ll::literals::chrono_literals;

static SPSCRingBuffer<std::tuple<std::chrono::steady_clock::time_point, std::chrono::steady_clock::time_point>, 64> tickTimes;
static SPSCRingBuffer<std::vector<server::ActorInfo>, 4> actorInfoBuffer;

LL_TYPE_INSTANCE_HOOK(PerformanceLevelTickEventHook, HookPriority::Lowest, ServerLevel, &ServerLevel::$_subTick, void) {
    // 记录开始时间
    auto start = std::chrono::steady_clock::now();
    origin();
    tickTimes.push(std::make_tuple(start, std::chrono::steady_clock::now()));
}

server::Performance &server::Performance::getInstance() {
    static Performance instance;
    return instance;
}

server::Performance& server::Performance::tpsBeginRecord() {
    tpsEndRecord();
    tpsRecordFlag = true;
    PerformanceLevelTickEventHook::hook();
    tpsRecordTask = [this]() {
        if (tpsRecordFlag) {
            std::tuple<std::chrono::steady_clock::time_point, std::chrono::steady_clock::time_point> timePair;
            size_t tickCount = 0;
            size_t sum = 0;
            const size_t initialSize = tickTimes.size();
            const size_t max = (initialSize <= 20) ? initialSize : 20 + (initialSize - 20) / 2;
            while ((tickCount < max) && tickTimes.pop(timePair)) {
                tickCount++;
                // 结构化绑定解包时间点
                const auto& [start, end] = timePair;

                // 计算时间差并转换为微秒（平台兼容性关键）
                const auto us_diff = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
                sum += us_diff.count();
            }
            if (tickCount == 0) [[unlikely]] {
                sendTpsRecordData(0, 0);
            } else {
                sendTpsRecordData(tickCount, sum / tickCount / 1000);
            }
            thread::ThreadPool::defaultThreadPool().addDelayTask(std::chrono::milliseconds(1000), tpsRecordTask);
        } else {
            std::lock_guard lock(tpsRecordMutex);
            tpsRecordCV.notify_all();
        }
    };
    thread::ThreadPool::defaultThreadPool().addDelayTask(std::chrono::milliseconds(1000), tpsRecordTask);
    return *this;
}

server::Performance& server::Performance::tpsEndRecord() {
    if (tpsRecordFlag) {
        std::unique_lock<std::mutex> lock(tpsRecordMutex);
        tpsRecordFlag = false;
        // 等待条件变量的通知
        tpsRecordCV.wait(lock);
        PerformanceLevelTickEventHook::unhook();
    }
    return *this;
}

server::Performance& server::Performance::entityBeginRecord() {
    entityEndRecord();
    entityRecordFlag = true;
    ll::coro::keepThis([this]() -> ll::coro::CoroTask<> {
            while (entityRecordFlag) {
                co_await 1s;
                auto actorList = ll::service::getLevel()->getRuntimeActorList();
                actorInfoBuffer.push([&actorList](std::vector<ActorInfo>& actorInfoList) {
                        actorInfoList.clear();
                        for (auto& actor : actorList) {
                            const Vec3 feetPos = actor->getFeetPos();
                            auto isItem = actor->hasCategory(ActorCategory::Item);
                            actorInfoList.emplace_back(ActorInfo{
                        .typeName = actor->getTypeName(),
                        .nameTag = actor->getNameTag(),
                        .itemTypeName = isItem ? ((ItemActor *)actor)->item().getTypeName() : "",
                        .runtimeID = actor->getRuntimeID().rawID,
                        .category = (uint)actor->mCategories,
                        .dimensionId = actor->getDimension().getDimensionId().id,
                        .age = isItem ? ((ItemActor *)actor)->age() : 0,
                        .x = feetPos.x,
                        .y = feetPos.y,
                        .z = feetPos.z,
                        .isLeashed = actor->isLeashed(),
                        .isTame = actor->isTame(),
                        .isTrusting = SynchedActorDataAccess::getActorFlag(actor->getEntityContext(), ActorFlags::Trusting)
                    });
                        }
                    });
            }
            co_return;
        }).launch(ll::thread::ServerThreadExecutor::getDefault());
    serverStoppingListener = ll::event::Listener<ll::event::server::ServerStoppingEvent>::create([this](ll::event::server::ServerStoppingEvent& ev) {
            entityEndRecord();
        });
    ll::event::EventBus::getInstance().addListener<ll::event::server::ServerStoppingEvent>(serverStoppingListener);
    entityRecordTask = [this]() {
        if (entityRecordFlag) {
            std::vector<ActorInfo> actorInfoList;
            while (actorInfoBuffer.pop([this](std::vector<ActorInfo>& actorInfoList) {
                    sendEntityRecordData(actorInfoList);
                })) {
            }
            thread::ThreadPool::defaultThreadPool().addDelayTask(std::chrono::milliseconds(1000), entityRecordTask);
        } else {
            std::lock_guard lock(entityRecordMutex);
            entityRecordCV.notify_all();
        }
    };
    thread::ThreadPool::defaultThreadPool().addDelayTask(std::chrono::milliseconds(1000), entityRecordTask);
    return *this;
}

server::Performance& server::Performance::entityEndRecord() {
    if (serverStoppingListener) {
        ll::event::EventBus::getInstance().removeListener(serverStoppingListener);
        serverStoppingListener.reset();
    }
    if (entityRecordFlag) {
        std::unique_lock<std::mutex> lock(entityRecordMutex);
        entityRecordFlag = false;
        // 等待条件变量的通知
        entityRecordCV.wait(lock);
    }
    return *this;
}

server::Performance::Performance() {
}

server::Performance::~Performance() {
    tpsEndRecord();
    entityEndRecord();
}

void server::Performance::sendTpsRecordData(size_t tps, size_t mspt) {
    webuiServer::WebUI::getInstance().sendTpsRecordData(tps, mspt);
}

void server::Performance::sendEntityRecordData(std::vector<ActorInfo>& actorInfoList) {
    webuiServer::WebUI::getInstance().sendEntityRecordData(actorInfoList);
}
