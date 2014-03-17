#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-ESP");
    BOOST_CHECK(GetBoolArg("-ESP"));
    BOOST_CHECK(GetBoolArg("-ESP", false));
    BOOST_CHECK(GetBoolArg("-ESP", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-ESPo"));
    BOOST_CHECK(!GetBoolArg("-ESPo", false));
    BOOST_CHECK(GetBoolArg("-ESPo", true));

    ResetArgs("-ESP=0");
    BOOST_CHECK(!GetBoolArg("-ESP"));
    BOOST_CHECK(!GetBoolArg("-ESP", false));
    BOOST_CHECK(!GetBoolArg("-ESP", true));

    ResetArgs("-ESP=1");
    BOOST_CHECK(GetBoolArg("-ESP"));
    BOOST_CHECK(GetBoolArg("-ESP", false));
    BOOST_CHECK(GetBoolArg("-ESP", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noESP");
    BOOST_CHECK(!GetBoolArg("-ESP"));
    BOOST_CHECK(!GetBoolArg("-ESP", false));
    BOOST_CHECK(!GetBoolArg("-ESP", true));

    ResetArgs("-noESP=1");
    BOOST_CHECK(!GetBoolArg("-ESP"));
    BOOST_CHECK(!GetBoolArg("-ESP", false));
    BOOST_CHECK(!GetBoolArg("-ESP", true));

    ResetArgs("-ESP -noESP");  // -ESP should win
    BOOST_CHECK(GetBoolArg("-ESP"));
    BOOST_CHECK(GetBoolArg("-ESP", false));
    BOOST_CHECK(GetBoolArg("-ESP", true));

    ResetArgs("-ESP=1 -noESP=1");  // -ESP should win
    BOOST_CHECK(GetBoolArg("-ESP"));
    BOOST_CHECK(GetBoolArg("-ESP", false));
    BOOST_CHECK(GetBoolArg("-ESP", true));

    ResetArgs("-ESP=0 -noESP=0");  // -ESP should win
    BOOST_CHECK(!GetBoolArg("-ESP"));
    BOOST_CHECK(!GetBoolArg("-ESP", false));
    BOOST_CHECK(!GetBoolArg("-ESP", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--ESP=1");
    BOOST_CHECK(GetBoolArg("-ESP"));
    BOOST_CHECK(GetBoolArg("-ESP", false));
    BOOST_CHECK(GetBoolArg("-ESP", true));

    ResetArgs("--noESP=1");
    BOOST_CHECK(!GetBoolArg("-ESP"));
    BOOST_CHECK(!GetBoolArg("-ESP", false));
    BOOST_CHECK(!GetBoolArg("-ESP", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-ESP", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-ESP", "eleven"), "eleven");

    ResetArgs("-ESP -bar");
    BOOST_CHECK_EQUAL(GetArg("-ESP", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-ESP", "eleven"), "");

    ResetArgs("-ESP=");
    BOOST_CHECK_EQUAL(GetArg("-ESP", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-ESP", "eleven"), "");

    ResetArgs("-ESP=11");
    BOOST_CHECK_EQUAL(GetArg("-ESP", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-ESP", "eleven"), "11");

    ResetArgs("-ESP=eleven");
    BOOST_CHECK_EQUAL(GetArg("-ESP", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-ESP", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-ESP", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-ESP", 0), 0);

    ResetArgs("-ESP -bar");
    BOOST_CHECK_EQUAL(GetArg("-ESP", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-ESP=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-ESP", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-ESP=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-ESP", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--ESP");
    BOOST_CHECK_EQUAL(GetBoolArg("-ESP"), true);

    ResetArgs("--ESP=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-ESP", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noESP");
    BOOST_CHECK(!GetBoolArg("-ESP"));
    BOOST_CHECK(!GetBoolArg("-ESP", true));
    BOOST_CHECK(!GetBoolArg("-ESP", false));

    ResetArgs("-noESP=1");
    BOOST_CHECK(!GetBoolArg("-ESP"));
    BOOST_CHECK(!GetBoolArg("-ESP", true));
    BOOST_CHECK(!GetBoolArg("-ESP", false));

    ResetArgs("-noESP=0");
    BOOST_CHECK(GetBoolArg("-ESP"));
    BOOST_CHECK(GetBoolArg("-ESP", true));
    BOOST_CHECK(GetBoolArg("-ESP", false));

    ResetArgs("-ESP --noESP");
    BOOST_CHECK(GetBoolArg("-ESP"));

    ResetArgs("-noESP -ESP"); // ESP always wins:
    BOOST_CHECK(GetBoolArg("-ESP"));
}

BOOST_AUTO_TEST_SUITE_END()
