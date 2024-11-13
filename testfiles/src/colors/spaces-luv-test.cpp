// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Unit tests for the LUV color space
 *
 * Copyright (C) 2023 Authors
 *
 * Released under GNU GPL v2+, read the file 'COPYING' for more information.
 */

#include "colors/spaces/luv.h"
#include "spaces-testbase.h"

namespace {

using Space::Type::LUV;
using Space::Type::RGB;

// clang-format off
// There is no CSS string for Luv colors
GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(fromString);
GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(badColorString);
GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(toString);

INSTANTIATE_TEST_SUITE_P(ColorsSpacesLuv, convertColorSpace, testing::Values(
    // No conversion
    _P(inb, LUV, {1.000, 0.400, 0.200}, LUV, {1.000, 0.400, 0.200})
));

INSTANTIATE_TEST_SUITE_P(ColorsSpacesLuv, normalize, testing::Values(
    _P(inb, LUV, { 0.5,   0.5,   0.5,   0.5  }, LUV, { 0.5,   0.5,   0.5,   0.5  }),
    _P(inb, LUV, { 1.2,   1.2,   1.2,   1.2  }, LUV, { 1.0,   1.0,   1.0,   1.0  }),
    _P(inb, LUV, {-0.2,  -0.2,  -0.2,  -0.2  }, LUV, { 0.0,   0.0,   0.0,   0.0  }),
    _P(inb, LUV, { 0.0,   0.0,   0.0,   0.0  }, LUV, { 0.0,   0.0,   0.0,   0.0  }),
    _P(inb, LUV, { 1.0,   1.0,   1.0,   1.0  }, LUV, { 1.0,   1.0,   1.0,   1.0  })
));
// clang-format on

TEST(ColorsSpacesLuv, manualConversion)
{
    // This output is unscaled, so Luv values are between L:0..100 and etc.
    EXPECT_TRUE(ManualPassFunc(Space::Luv::fromXYZ, {0.5, 0.2, 0.4}, Space::Luv::toXYZ, {51.837, 153.445, -57.51}));
}

TEST(ColorsSpacesLuv, randomConversion)
{
    // Isolate conversion functions
    EXPECT_TRUE(RandomPassFunc(Space::Luv::fromXYZ, Space::Luv::toXYZ, 1000));

    // Full stack conversion
    EXPECT_TRUE(RandomPassthrough(LUV, RGB, 1000));
}

TEST(ColorsSpacesLuv, components)
{
    auto c = Manager::get().find(LUV)->getComponents();
    ASSERT_EQ(c.size(), 3);
    ASSERT_EQ(c[0].id, "l");
    ASSERT_EQ(c[1].id, "u");
    ASSERT_EQ(c[2].id, "v");
}

} // namespace

/*
  Local Variables:
  mode:c++
  c-file-style:"stroustrup"
  c-file-offsets:((innamespace . 0)(inline-open . 0))
  indent-tabs-mode:nil
  fill-column:99
  End:
*/
// vim: expandtab:shiftwidth=4:tabstop=8:softtabstop=4 :
