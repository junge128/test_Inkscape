// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Unit tests for color xml conversions.
 *
 * Copyright (C) 2023 Authors
 *
 * Released under GNU GPL v2+, read the file 'COPYING' for more information.
 */

#include "colors/xml-color.h"

#include <gtest/gtest.h>

#include "colors/cms/profile.h"
#include "colors/cms/system.h"
#include "colors/color.h"
#include "colors/manager.h"
#include "colors/spaces/cms.h"
#include "preferences.h"

using namespace Inkscape;
using namespace Inkscape::Colors;

static std::string cmyk_profile = INKSCAPE_TESTS_DIR "/data/colors/default_cmyk.icc";

namespace {

class ColorXmlColor : public ::testing::Test
{
    void SetUp() override
    {
        auto prefs = Inkscape::Preferences::get();
        prefs->setBool("/options/svgoutput/inlineattrs", false);
    }
};

TEST_F(ColorXmlColor, test_paint_to_xml_string)
{
    ASSERT_EQ(paint_to_xml_string({}), R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<paint>
  <nocolor />
</paint>
)");
    ASSERT_EQ(paint_to_xml_string(NoColor()), R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<paint>
  <nocolor />
</paint>
)");
    ASSERT_EQ(paint_to_xml_string(Color(0xcf321244)), R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<paint>
  <color
     space="RGB"
     opacity="0.26666667"
     r="0.81176471"
     g="0.19607843"
     b="0.07058824" />
</paint>
)");
    ASSERT_EQ(paint_to_xml_string(*Color::parse("hsl(180,1,1)")), R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<paint>
  <color
     space="HSL"
     h="0.5"
     s="1"
     l="1" />
</paint>
)");
}

TEST_F(ColorXmlColor, test_icc_paint_xml)
{
    auto profile = CMS::Profile::create_from_uri(cmyk_profile);
    CMS::System::get().addProfile(profile);
    auto space = std::make_shared<Space::CMS>(profile);
    space->setIntent(RenderingIntent::AUTO);
    std::vector<double> vals = {0.5, 0.2, 0.1, 0.23};
    auto color = Color(space, vals);
    auto str = paint_to_xml_string(color);

    ASSERT_EQ(str, R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<paint>
  <color
     space="Artifex-CMYK-SWOP-Profile"
     icc="f9185275b4d6bcee53bc48dddb70fce5"
     c="0.5"
     m="0.2"
     y="0.1"
     k="0.23" />
</paint>
)");

    auto reverse = xml_string_to_paint(str, nullptr);
    ASSERT_EQ(std::get<Color>(reverse).toString(), color.toString());
}

TEST_F(ColorXmlColor, test_xml_string_to_paint)
{
    ASSERT_TRUE(std::holds_alternative<NoColor>(xml_string_to_paint(R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<paint>
  <nocolor />
</paint>
)",
                                     nullptr)));
    ASSERT_EQ(std::get<Color>(xml_string_to_paint(R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<paint>
  <color
     space="RGB"
     opacity="0.26666667"
     r="0.81176471"
     g="0.19607843"
     b="0.07058824" />
</paint>
)",
                                  nullptr)
                  ).toString(),
              "#cf321244");
    ASSERT_EQ(std::get<Color>(xml_string_to_paint(R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<paint>
  <color
     space="HSL"
     h="0.5"
     s="1"
     l="1" />
</paint>
)",
                                  nullptr)
                ).toString(),
              "hsl(180, 1, 1)");
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
