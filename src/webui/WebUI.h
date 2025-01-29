#pragma once
#include <thread>

namespace webuiServer {
class WebUI {
public:
    WebUI();
    ~WebUI();

    // 启动服务线程
        void startServer(const std::string& url, int port = -1);

        // 停止服务线程（优雅关闭）
        void stopServer() ;

private:
    std::thread m_serverThread;  // C++20 协作式线程
};  
} // namespace webuiServer