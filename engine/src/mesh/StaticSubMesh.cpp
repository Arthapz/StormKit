/////////// - StormKit::engine - ///////////
#include <storm/engine/mesh/StaticSubMesh.hpp>

using namespace storm;
using namespace storm::engine;

////////////////////////////////////////
////////////////////////////////////////
StaticSubMesh::StaticSubMesh(core::UInt32 vertex_count,
                             core::UInt32 first_index,
                             core::UInt32 index_count) noexcept
    : m_first_index { first_index }, m_index_count { index_count }, m_vertex_count {
          vertex_count
      } {
}

////////////////////////////////////////
////////////////////////////////////////
StaticSubMesh::~StaticSubMesh() = default;

////////////////////////////////////////
////////////////////////////////////////
StaticSubMesh::StaticSubMesh(StaticSubMesh &&) = default;

////////////////////////////////////////
////////////////////////////////////////
StaticSubMesh &StaticSubMesh::operator=(StaticSubMesh &&) = default;
