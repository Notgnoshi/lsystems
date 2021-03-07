#include "geom2graph/io/tgf-graph-reader.h"

namespace geom2graph::io {

geom2graph::noding::GeometryGraph TGFGraphReader::read() noexcept
{
    for (std::string line; std::getline(m_input, line);)
    {
        this->read(line);
    }

    return geom2graph::noding::GeometryGraph(std::move(this->m_nodes), m_factory);
}

void TGFGraphReader::read(const std::string& line) noexcept
{
    if (m_reading_nodes)
    {
        read_node(line);
    } else
    {
        read_edge(line);
    }
}

void TGFGraphReader::read_node(const std::string& line) noexcept
{
    (void)line;
}

void TGFGraphReader::read_edge(const std::string& line) noexcept
{
    (void)line;
}
}  // namespace geom2graph::io
