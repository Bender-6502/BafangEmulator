#pragma once
#include "ctest.h"
#include "profile.h"


TEST(profile_test_save)
{
  core::profile profile1("C:\\Users\\micro\\Source\\Repos\\BafangEmulator\\BafangConfigTool\\em3ev.el");

  profile1.save("test.el");

  core::profile profile2("test.el");

  EXPECT_TRUE(profile1 == profile2);
}

TEST(profile_test_compare)
{
  core::profile profile1("C:\\Users\\micro\\Source\\Repos\\BafangEmulator\\BafangConfigTool\\Executable\\Profiles\\DefaultProfile.el");
  core::profile profile2("C:\\Users\\micro\\Source\\Repos\\BafangEmulator\\BafangConfigTool\\Executable\\Profiles\\Penoff.el");

  EXPECT_FALSE(profile1 == profile2);
}

TEST(profile_test_add)
{
  core::profile profile1("C:\\Users\\micro\\Source\\Repos\\BafangEmulator\\BafangConfigTool\\Executable\\Profiles\\DefaultProfile.el");
  profile1.save("test.el");
  core::profile profile2("test.el");

  EXPECT_EQ(profile2.find("Throttle Handle", "MODE", 100), 0);
  profile2.add("Throttle Handle", "MODE", 101);
  EXPECT_EQ(profile2.find("Throttle Handle", "MODE", 100), 101);
  EXPECT_FALSE(profile1 == profile2);
}
