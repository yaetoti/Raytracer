#pragma once

#include <functional>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <vector>

namespace Flame {
  struct ParallelExecutor final {
    explicit ParallelExecutor(uint32_t threadsCount);
    ~ParallelExecutor();

    bool IsWorking() const;
    void Wait();
    void Execute(const std::function<void(uint32_t, uint32_t)>& task, uint32_t tasksCount, uint32_t tasksPerBatch);
    void ExecuteAsync(const std::function<void(uint32_t, uint32_t)>& task, uint32_t tasksCount, uint32_t tasksPerBatch);

  private:
    void WorkLoop(uint32_t threadNumber);

  private:
    std::vector<std::thread> m_threads;
    std::shared_mutex m_mutex;
    std::condition_variable_any m_workCv;
    std::condition_variable_any m_waitCv;
    bool m_isRunning;
    bool m_waitTriggered;

    std::function<void(uint32_t)> m_task;
    std::atomic<uint32_t> m_batchesCompleted;
    std::atomic<uint32_t> m_threadsFinished;
  };
}
