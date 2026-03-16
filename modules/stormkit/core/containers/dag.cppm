// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/memory_macro.hpp>

#include <stormkit/core/platform_macro.hpp>

#include <stormkit/core/contract_macro.hpp>

export module stormkit.core:containers.dag;

import std;

import :typesafe.integer;
import :typesafe.byte;
import :typesafe.safecasts;
import :utils.handle;
import :utils.contract;
import :utils.function_ref;
import :utils.filesystem;
import :string.format;

namespace stdr  = std::ranges;
namespace stdfs = std::filesystem;

export namespace stormkit { inline namespace core {
    namespace dag {
        using VertexID = u32;

        struct Edge {
            VertexID from;
            VertexID to;
        };

        template<typename VertexValue>
        struct Vertex {
            dag::VertexID id;
            VertexValue   value;
        };
    } // namespace dag

    template<typename VertexValue>
    class DAG {
      public:
        using Vertex             = dag::Vertex<VertexValue>;
        using ColorizeClosure    = FunctionRef<std::string(const VertexValue&)>;
        using FormatValueClosure = FunctionRef<std::string(const VertexValue&)>;

        using ValueType = Vertex;

        struct Closures {
            std::optional<ColorizeClosure>    colorize     = std::nullopt;
            std::optional<FormatValueClosure> format_value = []() static noexcept -> std::optional<FormatValueClosure> {
                if constexpr (std::formattable<VertexValue, char>)
                    return [](const auto& value) static noexcept { return std::format("{}", value); };
                else
                    return std::nullopt;
            }();
        };

        constexpr explicit DAG(std::optional<usize> reserve = std::nullopt) noexcept;
        constexpr ~DAG() noexcept;

        constexpr DAG(DAG&&) noexcept;
        constexpr DAG(const DAG&) noexcept = delete;

        constexpr auto operator=(DAG&&) noexcept -> DAG&;
        constexpr auto operator=(const DAG&) noexcept -> DAG& = delete;

        constexpr auto directed() const noexcept -> bool;

        constexpr auto clear() noexcept -> void;
        constexpr auto empty() const noexcept -> bool;

        constexpr auto add_vertex(const VertexValue& vertex) noexcept -> dag::VertexID
            requires(meta::IsCopyConstructible<VertexValue>);
        constexpr auto add_vertex(VertexValue&& vertex) noexcept -> dag::VertexID
            requires(meta::IsMoveConstructible<VertexValue>);
        template<typename... Args>
        constexpr auto emplace_vertex(Args&&... vertex) noexcept -> dag::VertexID
            requires(meta::IsConstructible<VertexValue, Args...>);
        template<typename Self>
        constexpr auto get_vertex_value(this Self&& self, dag::VertexID id) noexcept -> meta::ForwardLike<Self, VertexValue>;
        constexpr auto has_vertex(const VertexValue& vertex) const noexcept -> bool
            requires(meta::HasEqualityOperator<VertexValue, VertexValue>);
        constexpr auto has_vertex(dag::VertexID vertex) const noexcept -> bool;
        constexpr auto remove_vertex(const VertexValue& vertex) noexcept -> void
            requires(meta::HasEqualityOperator<VertexValue, VertexValue>);
        constexpr auto remove_vertex(dag::VertexID id) noexcept -> void;

        constexpr auto add_edge(dag::VertexID from, dag::VertexID to) noexcept -> void;
        constexpr auto has_edge(dag::VertexID from, dag::VertexID to) const noexcept -> bool;
        constexpr auto remove_edge(dag::VertexID from, dag::VertexID to) noexcept -> void;
        constexpr auto adjacent_edges(dag::VertexID vertex) const noexcept -> const std::vector<dag::Edge>&;

        constexpr auto vertices() const noexcept -> const std::vector<Vertex>&;
        constexpr auto vertices_count() const noexcept -> usize;

        constexpr auto edges() const noexcept -> const std::vector<dag::Edge>&;
        constexpr auto edges_count() const noexcept -> usize;

        constexpr auto topological_sort() const noexcept -> std::expected<std::vector<dag::VertexID>, std::vector<dag::VertexID>>;
        constexpr auto find_cycle() const noexcept -> std::optional<std::vector<dag::VertexID>>;

        constexpr auto reverse_view() const noexcept -> DAG<ref<const VertexValue>>;
        constexpr auto reverse_clone() const noexcept -> DAG<VertexValue>;

        constexpr auto dump(Closures closures = {}) const noexcept -> std::string;

        // FIXME find a way to make it not accessible to user
        template<typename FromDAG, bool AS_REF>
        static constexpr auto reverse_from(dag::VertexID,
                                           const std::vector<typename FromDAG::Vertex>&,
                                           const std::vector<dag::Edge>&) noexcept -> DAG<VertexValue>;

      private:
        dag::VertexID m_next_id = 0;

        std::vector<Vertex>                                           m_vertices;
        std::vector<dag::Edge>                                        m_edges;
        std::vector<std::pair<dag::VertexID, std::vector<dag::Edge>>> m_adjacent_edges;
    };

    namespace dag {
        template<typename FormatContext>
        auto format_as(const Edge& edge, FormatContext&) noexcept -> FormatContext::iterator;

        template<typename VertexValue, typename FormatContext>
        auto format_as(const dag::Vertex<VertexValue>& vertex, FormatContext&) noexcept -> FormatContext::iterator;

        [[nodiscard]]
        constexpr auto to_string(const Edge& edge) noexcept -> std::string;

        template<typename VertexValue>
        [[nodiscard]]
        constexpr auto to_string(const dag::Vertex<VertexValue>& vertex) noexcept -> std::string;
    } // namespace dag
}} // namespace stormkit::core

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace stormkit { inline namespace core {
    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename VertexValue>
    constexpr DAG<VertexValue>::DAG(std::optional<usize> reserve) noexcept {
        if (reserve) m_vertices.reserve(*reserve);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename VertexValue>
    constexpr DAG<VertexValue>::~DAG() noexcept = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename VertexValue>
    constexpr DAG<VertexValue>::DAG(DAG&&) noexcept = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename VertexValue>
    constexpr auto DAG<VertexValue>::operator=(DAG&&) noexcept -> DAG& = default;

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename VertexValue>
    constexpr auto DAG<VertexValue>::clear() noexcept -> void {
        m_adjacent_edges.clear();
        m_edges.clear();
        m_vertices.clear();
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename VertexValue>
    STORMKIT_FORCE_INLINE
    constexpr auto DAG<VertexValue>::empty() const noexcept -> bool {
        return stdr::empty(m_vertices);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename VertexValue>
    constexpr auto DAG<VertexValue>::add_vertex(const VertexValue& vertex) noexcept -> dag::VertexID
        requires(meta::IsCopyConstructible<VertexValue>)
    {
        const auto id = m_next_id++;
        m_vertices.emplace_back(id, vertex);
        m_adjacent_edges.emplace_back(id, std::vector<dag::Edge> {});
        return id;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename VertexValue>
    constexpr auto DAG<VertexValue>::add_vertex(VertexValue&& vertex) noexcept -> dag::VertexID
        requires(meta::IsMoveConstructible<VertexValue>)
    {
        const auto id = m_next_id++;
        m_vertices.emplace_back(id, std::move(vertex));
        m_adjacent_edges.emplace_back(id, std::vector<dag::Edge> {});
        return id;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename VertexValue>
    template<typename... Args>
    constexpr auto DAG<VertexValue>::emplace_vertex(Args&&... args) noexcept -> dag::VertexID
        requires(meta::IsConstructible<VertexValue, Args...>)
    {
        return add_vertex(VertexValue { std::forward<Args>(args)... });
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename VertexValue>
    template<typename Self>
    constexpr auto DAG<VertexValue>::get_vertex_value(this Self&& self, dag::VertexID id) noexcept
      -> meta::ForwardLike<Self, VertexValue> {
        expects(self.has_vertex(id), std::format("Unknown DAG vertex id: {}!", id));

        return std::forward_like<Self>(stdr::find_if(self.m_vertices, [id](const auto& other) noexcept { return other.id == id; })
                                         ->value);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename VertexValue>
    STORMKIT_FORCE_INLINE
    constexpr auto DAG<VertexValue>::has_vertex(const VertexValue& vertex) const noexcept -> bool
        requires(meta::HasEqualityOperator<VertexValue, VertexValue>)
    {
        return stdr::any_of(m_vertices, [&vertex](const auto& other) noexcept { return vertex == other.value; });
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename VertexValue>
    STORMKIT_FORCE_INLINE
    constexpr auto DAG<VertexValue>::has_vertex(dag::VertexID id) const noexcept -> bool {
        return stdr::any_of(m_vertices, [&id](const auto& vertex) noexcept { return vertex.id == id; });
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename VertexValue>
    constexpr auto DAG<VertexValue>::remove_vertex(const VertexValue& vertex) noexcept -> void
        requires(meta::HasEqualityOperator<VertexValue, VertexValue>)
    {
        if constexpr (std::formattable<VertexValue, char>) expects(has_vertex(vertex), "Unknown DAG vertex value: {}!", vertex);
        else
            expects(has_vertex(vertex), "Unknown DAG vertex value!");

        auto it = stdr::find_if(m_vertices, [&value = vertex](const auto& vertex) noexcept { return vertex.value == value; });
        const auto id = it->id;

        remove_vertex(id);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename VertexValue>
    constexpr auto DAG<VertexValue>::remove_vertex(dag::VertexID id) noexcept -> void {
        expects(has_vertex(id), std::format("Unknown DAG vertex id: {}!", id));

        auto touching = std::vector<dag::Edge> {};
        for (auto&& edge : m_edges)
            if (edge.from == id or edge.to == id) touching.emplace_back(edge);

        for (auto&& [from, to] : touching) remove_edge(from, to);

        auto&& [begin, end]   = stdr::remove_if(m_vertices, [id](const auto& vertex) noexcept { return vertex.id == id; });
        auto&& [begin2, end2] = stdr::remove_if(m_adjacent_edges, [id](const auto& pair) noexcept { return pair.first == id; });

        m_adjacent_edges.erase(begin2, end2);
        m_vertices.erase(begin, end);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename VertexValue>
    constexpr auto DAG<VertexValue>::add_edge(dag::VertexID from, dag::VertexID to) noexcept -> void {
        expects(has_vertex(from), std::format("Unknown DAG vertex from: {}", from));
        expects(has_vertex(to), std::format("Unknown DAG vertex to: {}", to));
        if (has_edge(from, to)) return;

        const auto& edge = m_edges.emplace_back(from, to);

        auto& adjacent_edges = stdr::find_if(m_adjacent_edges, [id = from](const auto& pair) noexcept {
                                   return pair.first == id;
                               })->second;
        adjacent_edges.emplace_back(edge);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename VertexValue>
    constexpr auto DAG<VertexValue>::has_edge(dag::VertexID from, dag::VertexID to) const noexcept -> bool {
        if (not has_vertex(from) or not has_vertex(to)) return false;

        return stdr::any_of(m_edges, [from, to](auto&& edge) noexcept { return edge.from == from and edge.to == to; });
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename VertexValue>
    constexpr auto DAG<VertexValue>::remove_edge(dag::VertexID from, dag::VertexID to) noexcept -> void {
        expects(has_vertex(from), std::format("Unknown DAG vertex from: {}", from));
        expects(has_vertex(to), std::format("Unknown DAG vertex to: {}", to));

        if (not has_edge(from, to)) return;

        {
            auto&& [begin, end] = stdr::remove_if(m_edges, [from, to](auto&& edge) noexcept {
                return edge.from == from and edge.to == to;
            });

            m_edges.erase(begin, end);
        }
        for (auto&& [_, edges] : m_adjacent_edges) {
            auto&& [begin, end] = stdr::remove_if(edges, [from, to](auto&& edge) noexcept {
                return edge.from == from and edge.to == to;
            });
            edges.erase(begin, end);
        }
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename VertexValue>
    constexpr auto DAG<VertexValue>::adjacent_edges(dag::VertexID id) const noexcept -> const std::vector<dag::Edge>& {
        expects(has_vertex(id), std::format("Unknown DAG vertex id: {}!", id));

        const auto& adjacent_edges = stdr::find_if(m_adjacent_edges, [id](const auto& pair) noexcept { return pair.first == id; })
                                       ->second;
        return adjacent_edges;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename VertexValue>
    STORMKIT_FORCE_INLINE
    constexpr auto DAG<VertexValue>::vertices() const noexcept -> const std::vector<Vertex>& {
        return m_vertices;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename VertexValue>
    STORMKIT_FORCE_INLINE
    constexpr auto DAG<VertexValue>::vertices_count() const noexcept -> usize {
        return stdr::size(m_vertices);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename VertexValue>
    STORMKIT_FORCE_INLINE
    constexpr auto DAG<VertexValue>::edges() const noexcept -> const std::vector<dag::Edge>& {
        return m_edges;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename VertexValue>
    STORMKIT_FORCE_INLINE
    constexpr auto DAG<VertexValue>::edges_count() const noexcept -> usize {
        return stdr::size(m_edges);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename VertexValue>
    constexpr auto DAG<VertexValue>::topological_sort() const noexcept
      -> std::expected<std::vector<dag::VertexID>, std::vector<dag::VertexID>> {
        if (auto result = find_cycle(); result.has_value()) return std::unexpected { std::move(*result) };

        struct Degree {
            dag::VertexID id;
            u32           d = 0;
        };

        auto in_degree = std::vector<Degree> {};
        in_degree.reserve(stdr::size(m_vertices));
        for (const auto& [id, _] : m_vertices) { in_degree.emplace_back(id, 0); }

        for (const auto& [id, _] : m_vertices) {
            const auto& edges = adjacent_edges(id);

            if (not stdr::empty(edges)) {
                for (auto&& [from, to] : edges) {
                    if (from != id) continue;
                    auto&& [_, d] = *stdr::find_if(in_degree, [to](auto&& other) noexcept { return other.id == to; });
                    d += 1;
                }
            }
        }

        auto queue = std::queue<dag::VertexID> {};

        for (const auto& [id, _] : m_vertices) {
            auto&& [_, d] = *stdr::find_if(in_degree, [id](auto&& other) noexcept { return other.id == id; });
            if (d == 0) queue.push(id);
        }

        auto ordered_vertices = std::vector<dag::VertexID> {};
        ordered_vertices.reserve(stdr::size(m_vertices));
        while (not stdr::empty(queue)) {
            auto id = queue.front();
            queue.pop();

            ordered_vertices.emplace_back(id);

            const auto& edges = adjacent_edges(id);
            if (not stdr::empty(edges)) {
                for (auto&& [from, to] : edges) {
                    if (from == id) {
                        auto&& [_, d] = *stdr::find_if(in_degree, [to](auto&& other) noexcept { return other.id == to; });
                        if (d > 0) d -= 1;
                        if (d == 0) queue.push(to);
                    }
                }
            }
        }

        return ordered_vertices;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename VertexValue>
    constexpr auto DAG<VertexValue>::find_cycle() const noexcept -> std::optional<std::vector<dag::VertexID>> {
        auto out = std::optional<std::vector<dag::VertexID>> { std::nullopt };

        auto visited = std::vector<dag::VertexID> {};
        visited.reserve(stdr::size(m_vertices));
        auto stack = std::vector<dag::VertexID> {};

        auto dfs = [&visited, &stack, &out, this](auto&& dfs, auto&& id) mutable noexcept -> bool {
            visited.emplace_back(id);

            const auto& edges = adjacent_edges(id);
            if (stdr::empty(edges)) return false;

            stack.emplace_back(id);

            for (const auto& [from, to] : edges) {
                const auto w = (from == id) ? to : from;
                if (not stdr::contains(visited, w)) {
                    if (dfs(dfs, w)) {
                        return true;
                    } else if (auto it = stdr::find(stack, w); it != stdr::cend(stack)) {
                        auto cycle = std::vector<dag::VertexID>(it, stdr::end(stack));
                        cycle.emplace_back(w);
                        out = std::move(cycle);
                        return true;
                    }
                } else if (auto it = stdr::find(stack, w); it != stdr::cend(stack)) {
                    auto cycle = std::vector<dag::VertexID>(it, stdr::end(stack));
                    cycle.emplace_back(w);
                    out = std::move(cycle);
                    return true;
                }
            }

            auto [begin, end] = stdr::remove(stack, id);
            stack.erase(begin, end);
            return false;
        };

        for (auto&& [id, _] : m_vertices) {
            if (stdr::contains(visited, id)) continue;

            if (dfs(dfs, id)) break;
        }

        return out;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename VertexValue>
    constexpr auto DAG<VertexValue>::reverse_view() const noexcept -> DAG<ref<const VertexValue>> {
        return DAG<ref<const VertexValue>>::template reverse_from<DAG<VertexValue>, true>(m_next_id, m_vertices, m_edges);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename VertexValue>
    constexpr auto DAG<VertexValue>::reverse_clone() const noexcept -> DAG<VertexValue> {
        return reverse_from<DAG<VertexValue>, false>(m_next_id, m_vertices, m_edges);
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename VertexValue>
    constexpr auto DAG<VertexValue>::dump(Closures closures) const noexcept -> std::string {
        auto out = std::string { "digraph G {\n"
                                 "    rankdir = LR\n"
                                 "    bgcolor = black\n"
                                 "    node [shape=box, fontname=\"helvetica\", fontsize=12];\n\n" };

        if (closures.format_value)
            for (const auto& [id, value] : m_vertices) {
                out += std::format("    \"node{}\" [label=\"id: {} value: {}\", style=filled,color=\"{}\"];\n",
                                   id,
                                   id,
                                   closures.format_value.value()(value),
                                   closures.colorize ? closures.colorize.value()(value) : "white");
            }
        else
            for (const auto& [id, value] : m_vertices) {
                out += std::format("    \"node{}\" [label=\"id: {}\", style=filled,color=\"{}\"];\n",
                                   id,
                                   id,
                                   closures.colorize ? closures.colorize.value()(value) : "white");
            }

        for (const auto& [from, to] : m_edges) out += std::format("    \"node{}\" -> \"node{}\" [color=seagreen];\n", from, to);

        out += "}";

        return out;
    }

    ////////////////////////////////////////
    ////////////////////////////////////////
    template<typename VertexValue>
    template<typename FromDAG, bool AS_REF>
    constexpr auto DAG<VertexValue>::reverse_from(dag::VertexID                                next_id,
                                                  const std::vector<typename FromDAG::Vertex>& vertices,
                                                  const std::vector<dag::Edge>& edges) noexcept -> DAG<VertexValue> {
        auto out      = DAG<VertexValue> {};
        out.m_next_id = next_id;

        out.m_adjacent_edges.reserve(stdr::size(vertices));
        if constexpr (AS_REF) {
            for (const auto& [id, vertice] : vertices) {
                out.m_vertices.emplace_back(id, as_ref(vertice));
                out.m_adjacent_edges.emplace_back(id, std::vector<dag::Edge> {});
            }
        } else {
            for (const auto& [id, vertice] : vertices) {
                out.m_vertices.emplace_back(id, auto(vertice));
                out.m_adjacent_edges.emplace_back(id, std::vector<dag::Edge> {});
            }
        }

        out.m_edges.reserve(stdr::size(edges));
        for (auto&& [from, to] : edges) {
            const auto& edge = out.m_edges.emplace_back(to, from);

            auto& adjacent_edges = stdr::find_if(out.m_adjacent_edges, [id = to](const auto& pair) noexcept {
                                       return pair.first == id;
                                   })->second;
            adjacent_edges.emplace_back(edge);
        }

        return out;
    }

    namespace dag {
        /////////////////////////////////////
        /////////////////////////////////////
        template<class FormatContext>
        STORMKIT_FORCE_INLINE
        inline auto format_as(const Edge& edge, FormatContext& ctx) noexcept -> FormatContext::iterator {
            auto&& out = ctx.out();
            return std::format_to(out, "[dag_edge from: {}, to: {}]", edge.from, edge.to);
        }

        ////////////////////////////////////////
        ////////////////////////////////////////
        template<typename VertexValue, typename FormatContext>
        STORMKIT_FORCE_INLINE
        inline auto format_as(const dag::Vertex<VertexValue>& vertex, FormatContext& ctx) noexcept -> FormatContext::iterator {
            auto&& out = ctx.out();
            if constexpr (std::formattable<VertexValue, char>)
                return std::format_to(out, "[dag_vertex id: {}, value: {}]", vertex.id, vertex.value);
            else
                return std::format_to(out, "[dag_vertex id: {}, value: <no formatter>]", vertex.id);
        }

        ////////////////////////////////////////
        ////////////////////////////////////////
        STORMKIT_FORCE_INLINE STORMKIT_PURE
        constexpr auto to_string(const dag::Edge& edge) noexcept -> std::string {
            return std::format("{}", edge);
        }

        ////////////////////////////////////////
        ////////////////////////////////////////
        template<typename VertexValue>
        STORMKIT_FORCE_INLINE STORMKIT_PURE
        constexpr auto to_string(const dag::Vertex<VertexValue>& vertex) noexcept -> std::string {
            return std::format("{}", vertex);
        }
    } // namespace dag
}} // namespace stormkit::core
