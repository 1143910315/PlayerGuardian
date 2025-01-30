#pragma once

namespace command
{
    class UICommand {
    public:
        // 删除拷贝构造函数和赋值运算符
        UICommand(const UICommand&) = delete;
        UICommand& operator=(const UICommand&) = delete;

        // 获取单例实例
        static UICommand& getInstance();

        void registerCommand();

    private:
        // 私有默认构造函数
        UICommand() = default;
        // 私有析构函数
        ~UICommand() = default;
    };
    
} // namespace command
