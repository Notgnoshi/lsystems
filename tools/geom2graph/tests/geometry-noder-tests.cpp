#include "geom2graph/io/wkt.h"
#include "geom2graph/noding/geometry-noder.h"

#include <geos/geom/Geometry.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/noding/IteratedNoder.h>
#include <geos/noding/snap/SnappingNoder.h>
#include <geos/noding/snapround/SnapRoundingNoder.h>

#include <gtest/gtest.h>

using geom2graph::io::from_wkt;

TEST(GeometryNoderTests, DisjointPoint)
{
    const std::unique_ptr<geos::geom::Geometry> geometries = from_wkt(
        // clang-format off
        "GEOMETRYCOLLECTION("
            "LINESTRING(0 0, 2 2),"
            "POINT(1 1)," // midpoint of line
            "POINT(0 1)"  // not on line
        ")"
        // clang-format on
    );
    ASSERT_TRUE(geometries);
    // Noding ignores POINTs, even if they're on an existing edge.
    const std::unique_ptr<geos::geom::Geometry> expected = from_wkt("MULTILINESTRING((0 0, 2 2))");
    ASSERT_TRUE(expected);

    const std::unique_ptr<geos::geom::Geometry> noded =
        geom2graph::noding::GeometryNoder::node(*geometries);
    ASSERT_TRUE(noded);
    // std::cerr << noded->toString() << std::endl;

    EXPECT_EQ(noded->getGeometryType(), expected->getGeometryType());
    EXPECT_TRUE(noded->equals(expected.get()));
}

TEST(GeometryNoderTests, SimpleRectangle)
{
    const std::unique_ptr<geos::geom::Geometry> rectangle = from_wkt(
        // clang-format off
        "GEOMETRYCOLLECTION("
            "LINESTRING(2 0, 2 8),"  // left vertical
            "LINESTRING(6 0, 6 8),"  // right vertical
            "LINESTRING(0 2, 8 2),"  // bottom horizontal
            "LINESTRING(0 6, 8 6)"  // top horizontal
        ")"
        // clang-format on
    );
    ASSERT_TRUE(rectangle);
    const std::unique_ptr<geos::geom::Geometry> expected = from_wkt(
        // clang-format off
        "MULTILINESTRING ("
            "(2 0, 2 2),"
            "(2 2, 2 6),"
            "(2 6, 2 8),"
            "(6 0, 6 2),"
            "(6 2, 6 6),"
            "(6 6, 6 8),"
            "(0 2, 2 2),"
            "(2 2, 6 2),"
            "(6 2, 8 2),"
            "(0 6, 2 6),"
            "(2 6, 6 6),"
            "(6 6, 8 6)"
        ")"
        // clang-format on
    );
    ASSERT_TRUE(expected);

    const std::unique_ptr<geos::geom::Geometry> noded =
        geom2graph::noding::GeometryNoder::node(*rectangle);
    ASSERT_TRUE(noded);
    // std::cerr << noded->toString() << std::endl;

    EXPECT_EQ(noded->getGeometryType(), expected->getGeometryType());
    EXPECT_TRUE(noded->equals(expected.get()));
}

TEST(GeometryNoderTests, ProvideOwnNoder)
{
    const std::unique_ptr<geos::geom::Geometry> rectangle = from_wkt(
        // clang-format off
        "GEOMETRYCOLLECTION("
            "LINESTRING(2 0, 2 8),"  // left vertical
            "LINESTRING(6 0, 6 8),"  // right vertical
            "LINESTRING(0 2, 8 2),"  // bottom horizontal
            "LINESTRING(0 6, 8 6)"  // top horizontal
        ")"
        // clang-format on
    );
    ASSERT_TRUE(rectangle);
    const std::unique_ptr<geos::geom::Geometry> expected = from_wkt(
        // clang-format off
        "MULTILINESTRING ("
            "(2 0, 2 2),"
            "(2 2, 2 6),"
            "(2 6, 2 8),"
            "(6 0, 6 2),"
            "(6 2, 6 6),"
            "(6 6, 6 8),"
            "(0 2, 2 2),"
            "(2 2, 6 2),"
            "(6 2, 8 2),"
            "(0 6, 2 6),"
            "(2 6, 6 6),"
            "(6 6, 8 6)"
        ")"
        // clang-format on
    );
    ASSERT_TRUE(expected);

    // The same as the SimpleRectangle case, except this time we're providing our own IteratedNoder
    const geos::geom::PrecisionModel* pm = rectangle->getFactory()->getPrecisionModel();
    auto noder = std::make_unique<geos::noding::IteratedNoder>(pm);
    const std::unique_ptr<geos::geom::Geometry> noded =
        geom2graph::noding::GeometryNoder::node(*rectangle, std::move(noder));
    ASSERT_TRUE(noded);
    // std::cerr << noded->toString() << std::endl;

    EXPECT_EQ(noded->getGeometryType(), expected->getGeometryType());
    EXPECT_TRUE(noded->equals(expected.get()));
}

TEST(GeometryNoderTests, SnappingNoder)
{
    const std::unique_ptr<geos::geom::Geometry> input = from_wkt(
        // clang-format off
            "GEOMETRYCOLLECTION("
                "LINESTRING(0 1, 0 2),"
                "LINESTRING(0 2.001, 0 3)"
            ")"
        // clang-format on
    );
    ASSERT_TRUE(input);
    const std::unique_ptr<geos::geom::Geometry> expected = from_wkt(
        // clang-format off
            "MULTILINESTRING("
                "(0 1, 0 2),"
                "(0 2, 0 3)"  // I think it snaps to whichever node came first?
            ")"
        // clang-format on
    );
    ASSERT_TRUE(expected);

    const double tolerance = 0.01;  // A larger tolerance than the deviation.
    auto noder = std::make_unique<geos::noding::snap::SnappingNoder>(tolerance);
    const std::unique_ptr<geos::geom::Geometry> noded =
        geom2graph::noding::GeometryNoder::node(*input, std::move(noder));
    ASSERT_TRUE(noded);
    // std::cerr << noded->toString() << std::endl;

    EXPECT_EQ(noded->getGeometryType(), expected->getGeometryType());
    EXPECT_TRUE(noded->equals(expected.get()));
}

//! @todo Use google benchmark.
TEST(GeometryNoderTests, DISABLED_SnappingNoderPerformance)
{
}
