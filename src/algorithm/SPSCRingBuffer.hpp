#include <atomic>
#include <bit>
#include <memory>
#include <stdexcept>

template <typename T, size_t Capacity>
class SPSCRingBuffer {
    static_assert(std::has_single_bit(Capacity), "Capacity must be a power of two");
    static_assert(Capacity > 1, "Capacity must be greater than 1");

    struct alignas(64) PaddedAtomicSize {
        std::atomic<size_t> value = 0;
    };

    // 缓存行对齐防止false sharing
    PaddedAtomicSize head_;  // 消费者位置
    PaddedAtomicSize tail_;  // 生产者位置
    
    // 存储缓冲区（使用unique_ptr保证内存正确对齐）
    static constexpr size_t Alignment = std::hardware_destructive_interference_size;
    alignas(Alignment) T buffer_[Capacity];

public:
    SPSCRingBuffer() {
        static_assert(alignof(SPSCRingBuffer) >= Alignment, 
            "Alignment requirement not satisfied");
    }

    // 生产数据（返回是否覆盖了未读数据）
    bool push(T value) {
        const size_t current_tail = tail_.value.load(std::memory_order_relaxed);
        const size_t next_tail = next_index(current_tail);
        
        // 快速路径：缓冲区未满
        if (next_tail != head_.value.load(std::memory_order_acquire)) {
            buffer_[current_tail] = std::move(value);
            tail_.value.store(next_tail, std::memory_order_release);
            return false;
        }

        // 慢速路径：缓冲区已满，覆盖旧数据
        return push_slow_path(std::move(value), current_tail, next_tail);
    }

    // 消费数据（返回是否成功）
    bool pop(T& out) {
        const size_t current_head = head_.value.load(std::memory_order_relaxed);
        if (current_head == tail_.value.load(std::memory_order_acquire)) {
            return false; // 缓冲区为空
        }

        out = std::move(buffer_[current_head]);
        head_.value.store(next_index(current_head), std::memory_order_release);
        return true;
    }

private:
    static constexpr size_t next_index(size_t idx) noexcept {
        return (idx + 1) & (Capacity - 1); // 利用位运算代替取模
    }

    bool push_slow_path(T&& value, size_t current_tail, size_t next_tail) {
        // 计算需要覆盖的头部
        size_t current_head = head_.value.load(std::memory_order_acquire);
        
        // 检查是否仍然需要覆盖（可能其他线程已经移动头部）
        if (next_tail != current_head) {
            return false; // 已经被其他操作处理，回到快速路径
        }

        // 移动头部（覆盖最旧的数据）
        const size_t new_head = next_index(current_head);
        head_.value.store(new_head, std::memory_order_release);

        // 写入新数据
        buffer_[current_tail] = std::move(value);
        
        // 确保写入完成后再更新尾部
        std::atomic_thread_fence(std::memory_order_release);
        tail_.value.store(next_tail, std::memory_order_relaxed);
        return true;
    }
};