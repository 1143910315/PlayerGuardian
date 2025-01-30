#pragma once

namespace server {
    class Performance {
    public:
        // 删除拷贝构造函数和赋值运算符
        Performance(const Performance&) = delete;
        Performance& operator=(const Performance&) = delete;

        // 获取单例实例
        static Performance& getInstance();
    private:
        // 私有默认构造函数
        Performance() = default;
        // 私有析构函数
        ~Performance() = default;
    };
} // namespace server
