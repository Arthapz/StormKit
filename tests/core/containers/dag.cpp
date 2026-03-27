// Copyright (C) 2024 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

import std;

import stormkit.core;
import stormkit.test;

#include <stormkit/test/test_macro.hpp>

using namespace stormkit::core;
using namespace std::literals;

namespace stdr = std::ranges;

namespace {
    auto _ = test::TestSuite {
        "core.containers.dag",
        {
          { "topological_sort",
            [] {
                using Edge = std::pair<dag::VertexID, dag::VertexID>;
                constexpr auto
                  edges = into_array(Edge { 0, 5 },
                                     Edge { 0, 2 },
                                     Edge { 0, 1 },
                                     Edge { 3, 6 },
                                     Edge { 3, 5 },
                                     Edge { 3, 4 },
                                     Edge { 5, 4 },
                                     Edge { 6, 4 },
                                     Edge { 6, 0 },
                                     Edge { 3, 2 },
                                     Edge { 1, 4 });

                auto dag = DAG<i32> {};
                for (auto i : range(7)) dag.add_vertex(i);

                for (auto&& [from, to] : edges) dag.add_edge(from, to);

                auto result = dag.topological_sort();
                if (not result) {
                    for (auto id : result.error()) std::println("    {} ->", id);
                }
                EXPECTS((result.has_value()));

                auto ordered = std::move(*result);

                auto orders = hash_map<dag::VertexID, u32> {};
                {
                    auto i = 0;
                    for (auto id : ordered) orders.emplace(id, i++);
                }

                for (auto&& [from, to] : edges) EXPECTS(orders[from] < orders[to]);
            } },
          { "remove_edge_and_vertex",
            [] {
                auto dag = DAG<string> {};
                dag.emplace_vertex("a");
                dag.emplace_vertex("b");
                dag.emplace_vertex("c");
                dag.emplace_vertex("d");

                dag.add_edge(0, 1);
                dag.add_edge(1, 2);
                dag.add_edge(2, 3);
                dag.add_edge(0, 3);

                EXPECTS(dag.has_edge(0, 1));

                dag.remove_edge(0, 1);

                EXPECTS(not dag.has_edge(0, 1));
                EXPECTS(dag.has_edge(0, 3));

                dag.remove_vertex(2);

                EXPECTS(not dag.has_edge(1, 2));
                EXPECTS(not dag.has_edge(2, 3));

                EXPECTS(stdr::size(dag.vertices()) == 3);

                {
                    auto result = dag.topological_sort();
                    if (not result) {
                        for (auto id : result.error()) std::println("    {} ->", id);
                    }
                    EXPECTS(result.has_value());
                    EXPECTS(stdr::size(*result) == 3);
                }

                dag.add_edge(1, 0);
                dag.add_edge(3, 1);
                {
                    auto result = dag.topological_sort();
                    EXPECTS((not result.has_value()));
                }
            } },
          { "find_cycle",
            [] {
                auto dag = DAG<i32> {};
                dag.add_vertex(0);
                dag.add_vertex(1);
                dag.add_vertex(4);
                dag.add_vertex(5);
                dag.add_vertex(6);
                dag.add_vertex(9);

                dag.add_edge(5, 1);
                dag.add_edge(1, 4);
                dag.add_edge(4, 0);
                dag.add_edge(0, 1);
                dag.add_edge(2, 3);

                {
                    auto result = dag.find_cycle();
                    EXPECTS(result.has_value());

                    auto&& cycle = std::move(*result);
                    EXPECTS(stdr::size(cycle) == 4);
                    EXPECTS(dag.get_vertex_value(cycle[0]) == 0);
                    EXPECTS(dag.get_vertex_value(cycle[1]) == 1);
                    EXPECTS(dag.get_vertex_value(cycle[2]) == 6);
                    EXPECTS(dag.get_vertex_value(cycle[3]) == 0);
                }

                {
                    auto result = dag.topological_sort();
                    EXPECTS(not result.has_value());
                }
            } },
          { "reverse_view",
            [] {
                auto dag = DAG<string> {};
                dag.emplace_vertex("a");
                dag.emplace_vertex("b");
                dag.emplace_vertex("c");
                dag.emplace_vertex("d");

                dag.add_edge(0, 1);
                dag.add_edge(1, 2);
                dag.add_edge(2, 3);
                dag.add_edge(0, 3);

                auto reversed = dag.reverse_view();

                const auto& edges  = dag.edges();
                const auto& redges = reversed.edges();

                for (auto i : range(4u)) {
                    const auto [from, to]   = edges[i];
                    const auto [rfrom, rto] = redges[i];
                    EXPECTS(from == rto and rfrom == to);
                }
            } },
          { "reverse_clone",
            [] {
                auto dag = DAG<string> {};
                dag.emplace_vertex("a");
                dag.emplace_vertex("b");
                dag.emplace_vertex("c");
                dag.emplace_vertex("d");

                dag.add_edge(0, 1);
                dag.add_edge(1, 2);
                dag.add_edge(2, 3);
                dag.add_edge(0, 3);

                auto reversed = dag.reverse_clone();

                const auto& edges  = dag.edges();
                const auto& redges = reversed.edges();

                for (auto i : range(4u)) {
                    const auto [from, to]   = edges[i];
                    const auto [rfrom, rto] = redges[i];
                    EXPECTS(from == rto and rfrom == to);
                }
            } },
          { "dump",
            [] {
                auto dag = DAG<string> {};
                dag.emplace_vertex("a");
                dag.emplace_vertex("b");
                dag.emplace_vertex("c");
                dag.emplace_vertex("d");

                dag.add_edge(0, 1);
                dag.add_edge(1, 2);
                dag.add_edge(2, 3);
                dag.add_edge(0, 3);

                const auto required_result = "digraph G {\n"
                                             "    rankdir = LR\n"
                                             "    bgcolor = black\n"
                                             "    node [shape=box, fontname=\"helvetica\", fontsize=12];\n\n"
                                             "    \"node0\" [label=\"id: 0 value: a\", style=filled,color=\"white\"];\n"
                                             "    \"node1\" [label=\"id: 1 value: b\", style=filled,color=\"white\"];\n"
                                             "    \"node2\" [label=\"id: 2 value: c\", style=filled,color=\"white\"];\n"
                                             "    \"node3\" [label=\"id: 3 value: d\", style=filled,color=\"white\"];\n"
                                             "    \"node0\" -> \"node1\" [color=seagreen];\n"
                                             "    \"node1\" -> \"node2\" [color=seagreen];\n"
                                             "    \"node2\" -> \"node3\" [color=seagreen];\n"
                                             "    \"node0\" -> \"node3\" [color=seagreen];\n"
                                             "}"sv;

                const auto out = dag.dump();
                EXPECTS(out == required_result);
            } },
          }
    };
} // namespace
