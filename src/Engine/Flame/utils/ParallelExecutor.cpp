#include "ParallelExecutor.h"

#include <condition_variable>
#include <functional>
#include <iostream>

namespace Flame {
  ParallelExecutor::ParallelExecutor(uint32_t threadsCount) {
    m_waitTriggered = false;
    m_isRunning = true;
    m_threadsFinished = 0;

    m_threads.reserve(threadsCount);
    for (uint32_t i = 0; i < threadsCount; ++i) {
      m_threads.emplace_back([this, i] {
        WorkLoop(i);
      });
    }
  }

  ParallelExecutor::~ParallelExecutor() {
    Wait();
    m_isRunning = false;
    m_workCv.notify_all();

    for (auto& t : m_threads) {
      t.join();
    }
  }

  bool ParallelExecutor::IsWorking() const {
    return m_threadsFinished < m_threads.size();
  }

  void ParallelExecutor::Wait() {
    if (!IsWorking()) {
      return;
    }

    std::unique_lock lock(m_mutex);
    // Re-check for a case when threads finished and m_waitCV is notified before the lock is acquired
    if (!IsWorking()) {
      return;
    }

    m_waitCv.wait(lock, [this] {
      return m_waitTriggered;
    });
    m_waitTriggered = false;
  }

  void ParallelExecutor::Execute(const std::function<void(uint32_t, uint32_t)>& task, uint32_t tasksCount, uint32_t tasksPerBatch) {
    ExecuteAsync(task, tasksCount, tasksPerBatch);
    Wait();
  }

  void ParallelExecutor::ExecuteAsync(const std::function<void(uint32_t, uint32_t)>& task, uint32_t tasksCount, uint32_t tasksPerBatch) {
    Wait();

    m_batchesCompleted = 0;
    m_threadsFinished = 0;
    uint32_t batches = (tasksCount + tasksPerBatch - 1) / tasksPerBatch;

    m_task = [this, task, tasksPerBatch, batches, tasksCount](uint32_t threadNumber) {
      while (true) {
        uint32_t prevBatch = m_batchesCompleted.fetch_add(1);
        if (prevBatch >= batches) {
          return;
        }

        uint32_t begin = prevBatch * tasksPerBatch;
        uint32_t end = (prevBatch + 1) * tasksPerBatch;
        if (end > tasksCount) {
          end = tasksCount;
        }

        while (begin < end) {
          task(threadNumber, begin);
          ++begin;
        }
      }
    };

    m_workCv.notify_all();
  }

  void ParallelExecutor::WorkLoop(uint32_t threadNumber) {
    while (true) {
      {
        std::shared_lock lock(m_mutex);
        uint32_t prevThreadsFinished = m_threadsFinished.fetch_add(1);
        if (prevThreadsFinished + 1 == m_threads.size()) {
          lock.unlock();
          {
            // TODO: Is this better than simply using unique_lock instead of shared_lock?
            std::unique_lock writeLock(m_mutex);
            m_waitTriggered = true;
          }

          lock.lock();
          m_waitCv.notify_all();

          /*
				   * If an outer thread waits on m_waitCV, it will remain blocked until this thread enters m_workCV.wait(),
				   * because both CVs wait with the same m_mutex. This is needed to avoid this thread missing
				   * a notification on m_workCV in a situation when an outer thread unblocks after this line and before this thread
				   * enters m_workCV.wait(), which would result in this thread being blocked until a next notification.
				   */
        }

        m_workCv.wait(lock);
      }

      if (!m_isRunning) {
        return;
      }

      m_task(threadNumber);
    }
  }
}
