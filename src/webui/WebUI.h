#pragma once
#include <mutex>
#include <nlohmann/json.hpp>
#include <thread>

namespace webuiServer {
    class WebUI {
    public:
        // 删除拷贝构造函数和赋值运算符
        WebUI(const WebUI&) = delete;
        WebUI& operator=(const WebUI&) = delete;

        // 获取单例实例
        static WebUI& getInstance();

        // 启动服务线程
        void startServer(const std::string& url, int port = -1);
        // 重启服务线程
        void restartServer();
        // 停止服务线程（优雅关闭）
        void stopServer();

        // 发送TPS数据
        void sendTpsRecordData(size_t tps, size_t mspt);

        // 发送实体数据
        void sendEntityRecordData(const nlohmann::json& actorInfoList);

    private:
        // 私有默认构造函数
        WebUI();
        // 私有析构函数
        ~WebUI();

        std::thread m_serverThread;
        std::mutex m_mutex;
        std::string m_url;
    };
} // namespace webuiServer
