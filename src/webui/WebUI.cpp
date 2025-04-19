#include "thread/ThreadPool.h"
#include "WebUI.h"
#include <webui.hpp>
#undef min
#undef max
#include <chrono>
#include <cstdint>
#include <date/date.h>
#include <filesystem>
#include <format>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

template<typename ... Args>
std::string concatEven(std::string firstString, std::string secondString, std::string str, Args... args);

template<typename ... Args>
std::string concatOdd(std::string firstString, std::string secondString, std::string str, Args... args) {
    return str + firstString + concatEven(firstString, secondString, args ...);
}

template<>
std::string concatOdd(std::string firstString, std::string secondString, std::string str) {
    return str + firstString;
}

template<typename ... Args>
std::string concatEven(std::string firstString, std::string secondString, std::string str, Args... args) {
    return str + secondString + concatOdd(firstString, secondString, args ...);
}

template<>
std::string concatEven(std::string firstString, std::string secondString, std::string str) {
    return str + secondString;
}

// 生成自定义事件执行脚本
template<typename ... Args>
std::string generateCustomEvent(std::string className, std::string eventName, Args... args) {
    std::string str = "for(e of document.getElementsByClassName('" + className + "')){";
    str += "e.dispatchEvent(new CustomEvent('" + eventName + "', { detail: { ";

    str += concatOdd(": '", "', ", args ...);

    str += " }, bubbles: false, cancelable: true }));";
    str += "}";
    return str;
}

namespace detail {
    // 递归终止条件：无参数时返回空字符串
    inline static std::string process() {
        return "";
    }

    // 处理单个剩余参数
    template<typename T>
    std::string process(T&& arg) {
        return std::string(std::forward<T>(arg)) + "，";
    }

    // 处理两个及以上参数的通用情况
    template<typename T1, typename T2, typename ... Rest>
    std::string process(T1&& arg1, T2&& arg2, Rest&&... rest) {
        std::string segment = std::string(std::forward<T1>(arg1)) + "，"
            + std::string(std::forward<T2>(arg2)) + "。";

        if constexpr (sizeof...(Rest) > 0) {
            return segment + process(std::forward<Rest>(rest)...);
        } else {
            return segment;
        }
    }
} // namespace detail

template<typename ... Args>
std::string concatJson(Args&&... args) {
    return detail::process(std::forward<Args>(args)...);
}

// 生成自定义事件执行脚本
std::string generateCustomEventForJson(const std::string& className, const std::string& eventName, const std::string& json) {
    return "for(e of document.getElementsByClassName('" + className + "')){e.dispatchEvent(new CustomEvent('" + eventName + "', { detail: " + json + ", bubbles: false, cancelable: true }));}";
}

int64_t count = 0;
bool running = true;
webui::window myWindow;
std::function<void()> timeSend = []() {
    if (running) {
        // 获取当前时间
        auto now = std::chrono::system_clock::now() + std::chrono::hours(8);
        // 转换为系统时间
        auto currentTime = date::floor<std::chrono::seconds>(now);
        // 设置自定义格式
        std::ostringstream oss;
        oss << date::format("%Y-%m-%d %H:%M:%S", currentTime);
        myWindow.run(generateCustomEvent("receiveCppData", "receive-data", "message", "当前时间是：" + oss.str()));
        thread::ThreadPool::defaultThreadPool().addDelayTask(std::chrono::seconds(1), timeSend);
    }
};

static void runServer(const std::string& url, int port = -1) {
    if (port != -1) {
        myWindow.set_size(port);
    }
    myWindow.show(url);
    webui::wait();
}

webuiServer::WebUI::WebUI() {
    webui::set_default_root_folder("plugins/PlayerGuardian/dist");
    auto u8Path = std::string((char *)std::filesystem::current_path().generic_u8string().c_str());
    myWindow.set_profile("cache", std::format("{}/plugins/PlayerGuardian/cache", u8Path));

    myWindow.bind("", [](webui::window::event *e) {
            if (e->event_type == WEBUI_EVENT_CONNECTED) {
                thread::ThreadPool::defaultThreadPool().addDelayTask(std::chrono::seconds(1), timeSend);
                std::cout << "Connected." << std::endl;
            } else if (e->event_type == WEBUI_EVENT_DISCONNECTED) {
                running = false;
                std::cout << "Disconnected." << std::endl;
            } else if (e->event_type == WEBUI_EVENT_MOUSE_CLICK) {
                std::cout << "Click." << std::endl;
            } else if (e->event_type == WEBUI_EVENT_NAVIGATION) {
                auto str = e->get_string_view();
                std::cout << "Starting navigation to: " << str << std::endl;
                //myWindow.navigate(str);
            }
        });
    myWindow.bind("clickCount", [](webui::window::event *e) {
            count++;
            std::cout << e->get_string_view(0) << std::endl;
            e->return_string(std::format("{}", count));
        });
}

webuiServer::WebUI::~WebUI() {
    if (m_serverThread.joinable()) {
        webui::exit();
        m_serverThread.join();
    }
}

webuiServer::WebUI &webuiServer::WebUI::getInstance() {
    static WebUI instance;
    return instance;
}

void webuiServer::WebUI::startServer(const std::string& url, int port) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_url = url;
    if (m_serverThread.joinable()) {
        stopServer();
    }
    // 通过 thread 传递参数并启动线程
    m_serverThread = std::thread([this, url, port]() {
            // 实际服务逻辑
            runServer(url, port);
        });
}

void webuiServer::WebUI::restartServer() {
    startServer(m_url, -1);
}

void webuiServer::WebUI::stopServer() {
    if (m_serverThread.joinable()) {
        myWindow.close();
        m_serverThread.join();
    }
}

void webuiServer::WebUI::sendTpsRecordData(size_t tps, size_t mspt) {
    myWindow.run(generateCustomEvent("receiveTpsData", "tps-data", "TPS", std::to_string(tps), "MSPT", std::to_string(mspt)));
}

void webuiServer::WebUI::sendEntityRecordData(const nlohmann::json& actorInfoList) {
    myWindow.run(generateCustomEventForJson("receiveEntityData", "entity-data", actorInfoList.dump()));
}
