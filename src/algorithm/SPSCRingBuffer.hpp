#include <atomic>
#include <bit>
#include <stdexcept>

template<typename T, size_t Capacity>
class SPSCRingBuffer {
    static_assert(std::has_single_bit(Capacity), "Capacity must be a power of two");
    static_assert(Capacity > 1, "Capacity must be greater than 1");

    static constexpr size_t Alignment = std::hardware_destructive_interference_size;

    struct alignas(Alignment) PaddedAtomic {
        std::atomic<size_t> value = 0;
    };

    PaddedAtomic head_;  // 消费者位置（单调递增）
    PaddedAtomic tail_;  // 生产者位置（单调递增）
    alignas(Alignment) T buffer_[Capacity];  // 环形存储

public:
    SPSCRingBuffer() = default;

    void push(T&& value) noexcept {
        const size_t current_tail = tail_.value.load(std::memory_order_relaxed);

        // 直接赋值
        buffer_[current_tail % Capacity] = std::forward<T>(value);

        // 更新tail（保证写入可见性）
        tail_.value.store(current_tail + 1, std::memory_order_release);
    }

    bool pop(T& out) noexcept {
        size_t current_head = head_.value.load(std::memory_order_relaxed);
        const size_t current_tail = tail_.value.load(std::memory_order_acquire);

        // 计算需要跳过的元素数量
        const size_t new_head = current_tail - Capacity;
        // 处理数据覆盖
        if (new_head  > current_head) {
            // 更新head（无需逐个析构，依赖自动管理）
            head_.value.store(new_head, std::memory_order_relaxed);
            current_head = new_head;
        }

        if (current_head == current_tail) {
            return false;
        }

        out = std::move(buffer_[current_head % Capacity]);

        // 更新head
        head_.value.store(current_head + 1, std::memory_order_relaxed);
        return true;
    }
};
