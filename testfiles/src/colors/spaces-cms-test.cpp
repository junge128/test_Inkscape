// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Unit tests for color objects.
 *
 * Copyright (C) 2023 Authors
 *
 * Released under GNU GPL v2+, read the file 'COPYING' for more information.
 */

#include <gtest/gtest.h>

#include "colors/cms/profile.h"
#include "colors/color.h"
#include "colors/parser.h"
#include "colors/spaces/cms.h"
#include "colors/document-cms.h"

using namespace Inkscape::Colors;

static std::string cmyk_icc = INKSCAPE_TESTS_DIR "/data/colors/default_cmyk.icc";

namespace {

// Get access to protected members for testing
class CMS : public Space::CMS
{
public:
    CMS(unsigned size, Space::Type type = Space::Type::CMYK)
        : Space::CMS("test-profile", size, type){};
    CMS(std::shared_ptr<Inkscape::Colors::CMS::Profile> profile)
        : Space::CMS(profile){};
    std::string toString(std::vector<double> const &values, bool opacity = true) const
    {
        return Space::CMS::toString(values, opacity);
    }
    bool testOverInk(double black) { return overInk({1.0, 1.0, 1.0, black}); }
    bool testOutOfGamut(std::vector<double> values, std::shared_ptr<Space::AnySpace> space)
    {
        return outOfGamut(values, space);
    }
    void testName(std::string const &name) { setName(name); }
};

TEST(ColorsSpacesCms, parseColor)
{
    auto parser = Space::CMS::CmsParser();
    ASSERT_EQ(parser.getPrefix(), "icc-color");

    bool more = false;
    std::vector<double> output;
    std::istringstream ss("stress-test, 0.2, 90%,2,   .3 5%)");
    auto name = parser.parseColor(ss, output, more);
    ASSERT_EQ(name, "stress-test");
    ASSERT_EQ(output.size(), 5);
    ASSERT_EQ(output[0], 0.2);
    ASSERT_EQ(output[1], 0.9);
    ASSERT_EQ(output[2], 2.0);
    ASSERT_EQ(output[3], 0.3);
    ASSERT_EQ(output[4], 0.05);
}

TEST(ColorsSpacesCms, realColor)
{
    auto cmyk_profile = Inkscape::Colors::CMS::Profile::create_from_uri(cmyk_icc);
    auto cmyk = std::make_shared<CMS>(cmyk_profile);
    auto color = Color(cmyk, {0, 0, 0, 1});

    EXPECT_EQ(color.toString(), "#2c292a icc-color(Artifex-CMYK-SWOP-Profile, 0, 0, 0, 1)");
    EXPECT_EQ(color.toRGBA(), 0x2c292aff);
    EXPECT_EQ(color.toRGBA(0.5), 0x2c292a80);
    EXPECT_EQ(color.converted(Space::Type::RGB)->toString(), "#2c292a");
    EXPECT_FALSE(color.hasOpacity());
    EXPECT_EQ(color.getOpacity(), 1.0);

    color.addOpacity(0.5);
    // Opacity isn't stored in the icc-color string, because it's not supported.
    EXPECT_TRUE(color.hasOpacity());
    EXPECT_EQ(color.getOpacity(), 0.5);
    EXPECT_EQ(color.toString(), "#2c292a icc-color(Artifex-CMYK-SWOP-Profile, 0, 0, 0, 1)");
    EXPECT_EQ(color.toRGBA(), 0x2c292a80);
    EXPECT_EQ(color.toRGBA(0.5), 0x2c292a40);
    EXPECT_EQ(color.converted(Space::Type::RGB)->toString(), "#2c292a80");

    color = Color(0x2c292aff);
    EXPECT_TRUE(color.convert(cmyk));
    EXPECT_EQ(color.toString(), "#201c1c icc-color(Artifex-CMYK-SWOP-Profile, 0.697, 0.696, 0.658, 0.856)");
}

TEST(ColorsSpacesCms, fallbackColor)
{
    auto tr = DocumentCMS(nullptr);
    auto color = *tr.parse("#0080ff icc-color(missing-profile, 1, 2, 3)");
    EXPECT_EQ(color.toString(), "#0080ff icc-color(missing-profile, 1, 2, 3)");
    EXPECT_EQ(color.toRGBA(), 0x0080ffff);
    EXPECT_EQ(color.toRGBA(0.5), 0x0080ff80);
    EXPECT_EQ(color.converted(Space::Type::RGB)->toString(), "#0080ff");
    EXPECT_FALSE(color.hasOpacity());
    EXPECT_EQ(color.getOpacity(), 1.0);

    color.addOpacity(0.5);
    EXPECT_EQ(color.toString(), "#0080ff icc-color(missing-profile, 1, 2, 3)");
    EXPECT_EQ(color.toRGBA(), 0x0080ff80);
    EXPECT_EQ(color.toRGBA(0.5), 0x0080ff40);
}

TEST(ColorsSpacesCms, renderingIntent)
{
    auto cmyk_profile = Inkscape::Colors::CMS::Profile::create_from_uri(cmyk_icc);
    auto cmyk = std::make_shared<CMS>(cmyk_profile);
    cmyk->testName("vals");

    auto color1 = Color(cmyk, {0, 0, 0, 1});
    auto color2 = Color(cmyk, {0.5, 0, 0, 0});
    EXPECT_EQ(cmyk->getIntent(), RenderingIntent::UNKNOWN);

    cmyk->setIntent(RenderingIntent::PERCEPTUAL);
    EXPECT_EQ(cmyk->getIntent(), RenderingIntent::PERCEPTUAL);
    EXPECT_EQ(color1.toString(), "#2c292a icc-color(vals, 0, 0, 0, 1)");
    EXPECT_EQ(color1.converted(Space::Type::RGB)->toString(), "#2c292a");
    EXPECT_EQ(color2.toString(), "#70d0f6 icc-color(vals, 0.5, 0, 0, 0)");

    cmyk->setIntent(RenderingIntent::RELATIVE_COLORIMETRIC);
    EXPECT_EQ(cmyk->getIntent(), RenderingIntent::RELATIVE_COLORIMETRIC);
    EXPECT_EQ(color1.toString(), "#231f20 icc-color(vals, 0, 0, 0, 1)");
    EXPECT_EQ(color1.converted(Space::Type::RGB)->toString(), "#231f20");
    EXPECT_EQ(color2.toString(), "#6dcff6 icc-color(vals, 0.5, 0, 0, 0)");

    cmyk->setIntent(RenderingIntent::SATURATION);
    EXPECT_EQ(cmyk->getIntent(), RenderingIntent::SATURATION);
    EXPECT_EQ(color1.toString(), "#2c292a icc-color(vals, 0, 0, 0, 1)");
    EXPECT_EQ(color1.converted(Space::Type::RGB)->toString(), "#2c292a");
    EXPECT_EQ(color2.toString(), "#70d0f6 icc-color(vals, 0.5, 0, 0, 0)");

    cmyk->setIntent(RenderingIntent::ABSOLUTE_COLORIMETRIC);
    EXPECT_EQ(cmyk->getIntent(), RenderingIntent::ABSOLUTE_COLORIMETRIC);
    EXPECT_EQ(color1.toString(), "#2f2d2c icc-color(vals, 0, 0, 0, 1)");
    EXPECT_EQ(color1.converted(Space::Type::RGB)->toString(), "#2f2d2c");
    EXPECT_EQ(color2.toString(), "#69b6d1 icc-color(vals, 0.5, 0, 0, 0)");

    cmyk->setIntent(RenderingIntent::RELATIVE_COLORIMETRIC_NOBPC);
    EXPECT_EQ(cmyk->getIntent(), RenderingIntent::RELATIVE_COLORIMETRIC_NOBPC);
    EXPECT_EQ(color1.toString(), "#373535 icc-color(vals, 0, 0, 0, 1)");
    EXPECT_EQ(color1.converted(Space::Type::RGB)->toString(), "#373535");
    EXPECT_EQ(color2.toString(), "#73d1f6 icc-color(vals, 0.5, 0, 0, 0)");

    // These should be the same as PERCEPTUAL
    cmyk->setIntent(RenderingIntent::UNKNOWN);
    EXPECT_EQ(cmyk->getIntent(), RenderingIntent::UNKNOWN);
    EXPECT_EQ(color1.toString(), "#2c292a icc-color(vals, 0, 0, 0, 1)");
    EXPECT_EQ(color1.converted(Space::Type::RGB)->toString(), "#2c292a");
    EXPECT_EQ(color2.toString(), "#70d0f6 icc-color(vals, 0.5, 0, 0, 0)");

    cmyk->setIntent(RenderingIntent::AUTO);
    EXPECT_EQ(cmyk->getIntent(), RenderingIntent::AUTO);
    EXPECT_EQ(color1.toString(), "#2c292a icc-color(vals, 0, 0, 0, 1)");
    EXPECT_EQ(color1.converted(Space::Type::RGB)->toString(), "#2c292a");
    EXPECT_EQ(color2.toString(), "#70d0f6 icc-color(vals, 0.5, 0, 0, 0)");
}

TEST(ColorsSpacesCms, printColor)
{
    auto space = CMS(4);

    ASSERT_FALSE(space.isValid());
    ASSERT_EQ(space.toString({}), "");
    ASSERT_EQ(space.toString({1}), "");
    ASSERT_EQ(space.toString({1, 2, 3, 4}), "");
    ASSERT_EQ(space.toString({0, 0.5, 1, 1, 2, 3, 4}), "#0080ff icc-color(test-profile, 1, 2, 3, 4)");

    space = CMS(2);
    ASSERT_FALSE(space.isValid());
    ASSERT_EQ(space.toString({1}), "");
    ASSERT_EQ(space.toString({0, 0.5, 1, 1, 2}), "#0080ff icc-color(test-profile, 1, 2)");
    ASSERT_EQ(space.toString({0, 0, 0, 1, 2, 3}), "#000000 icc-color(test-profile, 1, 2)");

    auto srgb = Inkscape::Colors::CMS::Profile::create_srgb();
    space = CMS(srgb);
    space.setIntent(RenderingIntent::AUTO);
    ASSERT_TRUE(space.isValid());
    ASSERT_EQ(space.toString({1}), "");
    ASSERT_EQ(space.toString({0, 0.5001, 1}), "#0080ff icc-color(sRGB-built-in, 0, 0.5, 1)");
}

TEST(ColorsSpacesCms, outOfGamut)
{
    auto srgb = Inkscape::Colors::CMS::Profile::create_srgb();
    auto cmyk = Inkscape::Colors::CMS::Profile::create_from_uri(cmyk_icc);
    auto space = CMS(srgb);
    auto to_space = std::make_shared<CMS>(cmyk);

    EXPECT_FALSE(space.testOutOfGamut({0.83, 0.19, 0.49}, to_space));
    // An RGB color (magenta) which is outside the cmyk color profile
    EXPECT_TRUE(space.testOutOfGamut({1.0, 0.0, 1.0}, to_space));

    auto from_space = std::make_shared<CMS>(srgb);
    auto pink = Inkscape::Colors::Color(from_space, {0.83, 0.19, 0.49});
    EXPECT_FALSE(pink.isOutOfGamut(to_space));

    auto magenta = Inkscape::Colors::Color(from_space, {1.0, 0.0, 1.0});
    EXPECT_TRUE(magenta.isOutOfGamut(to_space));
}

TEST(ColorsSpacesCms, overInk)
{
    auto space = CMS(4, Space::Type::CMYK);
    ASSERT_TRUE(space.testOverInk(0.21));
    ASSERT_FALSE(space.testOverInk(0.19));
    space = CMS(4, Space::Type::RGB);
    ASSERT_FALSE(space.testOverInk(0.21));
    ASSERT_FALSE(space.testOverInk(0.19));
}

}; // namespace

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
