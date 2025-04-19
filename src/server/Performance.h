#pragma once
#include <atomic>
#include <deque>
#include <functional>
#include <ll/api/base/StdInt.h>
#include <ll/api/event/Listener.h>
#include <ll/api/event/server/ServerStoppingEvent.h>
#include <mutex>
#include <nlohmann/json.hpp>
#include <string>
#include <tuple>
#include <vector>

namespace server {
    struct ActorInfo {
        std::string typeName;
        std::string nameTag;
        std::string itemTypeName;
        uint64 runtimeID;
        uint category;
        int dimensionId;
        int age;
        float x;
        float y;
        float z;
        bool isLeashed;
        bool isTame;
        bool isTrusting;
        NLOHMANN_DEFINE_TYPE_INTRUSIVE(ActorInfo,
            typeName,
            nameTag,
            itemTypeName,
            runtimeID,
            category,
            dimensionId,
            age,
            x,
            y,
            z,
            isLeashed,
            isTame,
            isTrusting);
    };
    class Performance {
    public:
        // 删除拷贝构造函数和赋值运算符
        Performance(const Performance&) = delete;
        Performance& operator=(const Performance&) = delete;

        // 获取单例实例
        static Performance& getInstance();

        // 开始记录tps
        Performance& tpsBeginRecord();
        // 结束记录tps
        Performance& tpsEndRecord();

        // 开始记录实体数据
        Performance& entityBeginRecord();
        // 结束记录实体数据
        Performance& entityEndRecord();

    private:
        // 私有默认构造函数
        Performance();
        // 私有析构函数
        ~Performance();

        // 发送tps记录数据
        void sendTpsRecordData(size_t tps, size_t mspt);

        // 发送实体数据
        void sendEntityRecordData(std::vector<ActorInfo>& actorInfoList);

        // 是否开始记录tps
        std::atomic<bool> tpsRecordFlag = false;
        // 互斥锁
        std::mutex tpsRecordMutex;
        // 条件变量
        std::condition_variable tpsRecordCV;
        // 记录tps的线程
        std::function<void()> tpsRecordTask;

        // 是否开始记录实体数据
        std::atomic<bool> entityRecordFlag = false;
        // 互斥锁
        std::mutex entityRecordMutex;
        // 条件变量
        std::condition_variable entityRecordCV;
        // 记录实体数据的线程
        std::function<void()> entityRecordTask;

        // 服务器关闭事件监听器
        std::shared_ptr<ll::event::Listener<ll::event::server::ServerStoppingEvent>> serverStoppingListener;
    };
} // namespace server
