// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module stormkit.core.parallelism.threadpool;

import std;

namespace stdr = std::ranges;

namespace stormkit {
    /////////////////////////////////////
    /////////////////////////////////////
    ThreadPool::ThreadPool(ThreadPool&& other) noexcept
        : m_worker_count { other.m_worker_count }, m_running_task_counter { m_worker_count } {
        wait_idle();
        other.wait_idle();

        auto lock = std::scoped_lock { other.m_mutex };

        m_tasks = std::move(other.m_tasks);

        m_workers.clear();
        spawn_workers();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto ThreadPool::operator=(ThreadPool&& other) noexcept -> ThreadPool& {
        if (&other == this) [[unlikely]]
            return *this;

        wait_idle();
        other.wait_idle();

        auto lock1 = std::unique_lock { m_mutex, std::defer_lock };
        auto lock2 = std::unique_lock { other.m_mutex, std::defer_lock };
        std::lock(lock1, lock2);

        join_all();

        m_worker_count = other.m_worker_count;
        m_tasks        = std::move(other.m_tasks);

        m_workers.clear();
        spawn_workers();

        return *this;
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto ThreadPool::join_all() noexcept -> void {
        for (const auto _ : range(m_worker_count)) post_task<void>(Task::Type::Terminate, [] {}, ThreadPool::NO_FUTURE);

        for (auto& thread : m_workers)
            if (thread.joinable()) thread.join();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto ThreadPool::wait_idle(bool cancel_tasks) noexcept -> void {
        if (cancel_tasks) {
            auto _ = std::unique_lock { m_mutex };
            while (not stdr::empty(m_tasks)) m_tasks.pop();
        }

        for (;;) {
            {
                auto _ = std::unique_lock { m_mutex };
                if (stdr::empty(m_tasks)) break;
            }
            std::this_thread::yield();
        }

        auto count = worker_count();
        for (;;) {
            while (m_running_task_counter.try_acquire()) --count;

            if (count == 0) break;

            std::this_thread::yield();
        }

        for (auto _ : range(worker_count())) m_running_task_counter.release();
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto ThreadPool::worker_main(i32 id) noexcept -> void {
        set_current_thread_name(std::format("stormkit:worker_thread:{}", id));
        for (;;) {
            auto task = Task {};

            {
                auto lock = std::unique_lock { m_mutex };
                m_work_signal.wait(lock, [this] { return not std::empty(m_tasks); });
                task = std::move(m_tasks.front());
                m_tasks.pop();
            }

            m_running_task_counter.acquire();
            task.work();
            m_running_task_counter.release();

            if (task.type == Task::Type::Terminate) return;
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    auto ThreadPool::spawn_workers() noexcept -> void {
        m_workers.reserve(m_worker_count);

        for (auto i : range(m_worker_count)) m_workers.emplace_back(bind_front(&ThreadPool::worker_main, this, i));
    }
} // namespace stormkit
