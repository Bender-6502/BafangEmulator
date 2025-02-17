#include "gtest/gtest.h"
#include "profile.h"


TEST(profile_test, save)
{
  core::profile profile1("em3ev.el");

  profile1.save_as("test.el");

  core::profile profile2("test.el");

  EXPECT_TRUE(profile1 == profile2);
}

TEST(profile_test, compare)
{
  core::profile profile1("DefaultProfile.el");
  core::profile profile2("Penoff.el");

  EXPECT_FALSE(profile1 == profile2);
}

TEST(profile_test, add)
{
  core::profile profile1("DefaultProfile.el");
  profile1.save_as("test.el");
  core::profile profile2("test.el");

  EXPECT_EQ(profile2.find("Throttle Handle", "MODE", 100), 0);
  profile2.add("Throttle Handle", "MODE", 101);
  EXPECT_EQ(profile2.find("Throttle Handle", "MODE", 100), 101);
  EXPECT_FALSE(profile1 == profile2);
}
