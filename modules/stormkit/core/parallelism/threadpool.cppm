// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/api.hpp>
#include <stormkit/core/platform_macro.hpp>

export module stormkit.core:parallelism.threadpool;

import std;

import :parallelism.threadutils;

import :meta;

import :utils.numeric_range;
import :typesafe.integer;

export namespace stormkit { inline namespace core {
    class STORMKIT_CORE_API ThreadPool {
      public:
        static constexpr struct NoFutureType {
        } NO_FUTURE = {};

        template<class T>
        using Closure = std::function<T()>;
        // using Closure = std::move_only_function<T()>;

        explicit ThreadPool(u32 worker_count = std::thread::hardware_concurrency() / 2);
        ~ThreadPool();

        ThreadPool(const ThreadPool&)                    = delete;
        auto operator=(const ThreadPool&) -> ThreadPool& = delete;

        ThreadPool(ThreadPool&&) noexcept;
        auto operator=(ThreadPool&&) noexcept -> ThreadPool&;

        [[nodiscard]]
        auto worker_count() const noexcept -> u32;

        template<class T>
        [[nodiscard]]
        auto post_task(Closure<T> task) noexcept -> decltype(auto);

        template<class T>
        auto post_task(Closure<T> task, NoFutureType) noexcept -> void;

        auto join_all() noexcept -> void;

        auto set_name(std::string_view name) noexcept -> void;

      private:
        struct Task {
            enum class Type {
                Standard,
                Terminate,
            };

            Task() = default;

            inline Task(Type _type, std::function<void()> _work) : type { _type }, work { std::move(_work) } {}

            Task(Task&&) noexcept                    = default;
            auto operator=(Task&&) noexcept -> Task& = default;

            Type                  type;
            std::function<void()> work;
        };

        template<class T>
        auto post_task(Task::Type type, Closure<T> task) noexcept -> decltype(auto);

        template<class T>
        auto post_task(Task::Type type, Closure<T> task, NoFutureType) noexcept -> void;

        auto worker_main() noexcept -> void;

        u32 m_worker_count = 0;

        std::vector<std::jthread> m_workers;

        mutable std::mutex      m_mutex;
        std::condition_variable m_work_signal;
        std::queue<Task>        m_tasks;
    };

    template<std::ranges::input_range Range, std::invocable<meta::RangeType<Range>&> F>
    auto parallel_for(ThreadPool& pool, Range&& range, F&& f) noexcept -> void;

    template<std::ranges::input_range Range, std::invocable<meta::RangeType<Range>&> F>
    auto parallel_for_async(ThreadPool& pool, Range& range, F&& f) noexcept -> std::vector<std::future<void>>;
}} // namespace stormkit::core

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stdr = std::ranges;
namespace stdv = std::views;

namespace stormkit { inline namespace core {
    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline ThreadPool::ThreadPool(u32 worker_count)
        : m_worker_count { worker_count } {
        m_workers.reserve(m_worker_count);

        for (auto i : range(m_worker_count)) {
            auto& worker = m_workers.emplace_back([this] { worker_main(); });
            set_thread_name(worker, std::format("StormKit:WorkerThread:{}", i));
        }
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline ThreadPool::~ThreadPool() {
        join_all();
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto ThreadPool::worker_count() const noexcept -> u32 {
        return m_worker_count;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto ThreadPool::set_name(std::string_view name) noexcept -> void {
        // for (auto&& [i, worker] : stdv::enumerate(m_workers))
        for (auto i : range(m_worker_count)) set_thread_name(m_workers[i], std::format("{}:{}", name, i));
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class T>
    STORMKIT_FORCE_INLINE
    inline auto ThreadPool::post_task(Closure<T> task) noexcept -> decltype(auto) {
        return post_task<T>(Task::Type::Standard, std::move(task));
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class T>
    STORMKIT_FORCE_INLINE
    inline auto ThreadPool::post_task(Closure<T> task, NoFutureType t) noexcept -> void {
        auto _ = post_task<T>(Task::Type::Standard, std::move(task), t);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class T>
    inline auto ThreadPool::post_task(Task::Type type, Closure<T> closure) noexcept -> decltype(auto) {
        auto packaged_task = std::make_shared<std::packaged_task<T()>>(std::move(closure));

        auto future = packaged_task->get_future();

        auto task = Task { type, [task = std::move(packaged_task)]() { (*task)(); } };

        {
            auto _ = std::unique_lock { m_mutex };

            m_tasks.emplace(std::move(task));
        }

        m_work_signal.notify_one();

        return future;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<class T>
    inline auto ThreadPool::post_task(Task::Type type, Closure<T> closure, NoFutureType) noexcept -> void {
        auto task = Task { type, [task = std::move(closure)]() { task(); } };

        {
            auto lock = std::unique_lock { m_mutex };

            m_tasks.emplace(std::move(task));
        }

        m_work_signal.notify_one();
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<std::ranges::input_range Range, std::invocable<meta::RangeType<Range>&> F>
    inline auto parallel_for(ThreadPool& pool, Range&& range, F&& f) noexcept -> void {
        auto futures = parallel_for_async(pool, range, std::forward<F>(f));
        wait_all(futures);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<std::ranges::input_range Range, std::invocable<meta::RangeType<Range>&> F>
    inline auto parallel_for_async(ThreadPool& pool, Range& range, F&& f) noexcept -> std::vector<std::future<void>> {
        const auto size        = stdr::size(range);
        const auto chunk_size  = size / pool.worker_count();
        const auto chunk_count = size / chunk_size;

        auto out = std::vector<std::future<void>> {};
        out.reserve(chunk_count);

        for (auto chunk : stormkit::range(chunk_count)) {
            const auto start = chunk * chunk_size;
            auto       end   = (chunk + 1u) * chunk_size;
            if (end >= (chunk_count * size)) end = size;

            out.emplace_back(pool.post_task<void>([&f, &range, start, end] mutable noexcept {
                auto it = std::begin(range) + start;
                for (auto _ : stormkit::range(start, end)) {
                    f(*it);
                    ++it;
                }
            }));
        }

        return out;
    }
}} // namespace stormkit::core
