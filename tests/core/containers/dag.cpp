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
                using Edge           = std::pair<dag::VertexID, dag::VertexID>;
                constexpr auto edges = into_array(Edge { 0, 5 },
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

                auto dag = DAG<i32> { dag::DIRECTED };
                for (auto i : range(7)) dag.add_vertex(i);

                for (auto&& [from, to] : edges) dag.add_edge(from, to);

                auto result = dag.topological_sort();
                if (not result) {
                    for (auto id : result.error()) std::println("    {} ->", id);
                }
                EXPECTS((result.has_value()));

                auto ordered = std::move(*result);

                auto orders = HashMap<dag::VertexID, u32> {};
                {
                    auto i = 0;
                    for (auto id : ordered) orders.emplace(id, i++);
                }

                for (auto&& [from, to] : edges) EXPECTS(orders[from] < orders[to]);
            } },
          { "remove_edge_and_vertex",
            [] {
                auto dag = DAG<std::string> { dag::DIRECTED };
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
                auto dag = DAG<i32> { dag::DIRECTED };
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
                    EXPECTS(dag.get_vertex_value(cycle[0]).value == 0);
                    EXPECTS(dag.get_vertex_value(cycle[1]).value == 1);
                    EXPECTS(dag.get_vertex_value(cycle[2]).value == 6);
                    EXPECTS(dag.get_vertex_value(cycle[3]).value == 0);
                }

                {
                    auto result = dag.topological_sort();
                    EXPECTS(not result.has_value());
                }
            } },
          }
    };
} // namespace
