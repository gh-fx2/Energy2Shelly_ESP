#include <unity.h>
#include <limits>
#include "../../src/winter_charge_mode.h"

void test_disabled_mode_preserves_normal_reporting() {
  WinterChargeMode mode;
  mode.updateSoc(40);
  TEST_ASSERT_FALSE(mode.shouldSimulateZeroPower());
}

void test_enabled_mode_fails_safe_until_soc_arrives() {
  WinterChargeMode mode;
  mode.setEnabled(true);
  TEST_ASSERT_TRUE(mode.shouldSimulateZeroPower());
}

void test_enabled_mode_simulates_zero_until_full() {
  WinterChargeMode mode;
  mode.setEnabled(true);
  TEST_ASSERT_TRUE(mode.updateSoc(99.9));
  TEST_ASSERT_TRUE(mode.shouldSimulateZeroPower());
  TEST_ASSERT_TRUE(mode.updateSoc(100));
  TEST_ASSERT_FALSE(mode.isEnabled());
  TEST_ASSERT_FALSE(mode.shouldSimulateZeroPower());
}

void test_invalid_soc_does_not_change_mode_or_previous_reading() {
  WinterChargeMode mode;
  mode.setEnabled(true);
  TEST_ASSERT_TRUE(mode.updateSoc(50));
  TEST_ASSERT_FALSE(mode.updateSoc(101));
  TEST_ASSERT_FALSE(mode.updateSoc(-1));
  TEST_ASSERT_FALSE(mode.updateSoc(std::numeric_limits<double>::quiet_NaN()));
  TEST_ASSERT_EQUAL_DOUBLE(50, mode.soc());
  TEST_ASSERT_TRUE(mode.shouldSimulateZeroPower());
}

void test_manual_disable_restores_normal_reporting() {
  WinterChargeMode mode;
  mode.setEnabled(true);
  mode.updateSoc(50);
  mode.setEnabled(false);
  TEST_ASSERT_FALSE(mode.shouldSimulateZeroPower());
}

void test_cannot_enable_mode_when_soc_is_already_full() {
  WinterChargeMode mode;
  TEST_ASSERT_TRUE(mode.updateSoc(100));
  mode.setEnabled(true);
  TEST_ASSERT_FALSE(mode.isEnabled());
  TEST_ASSERT_FALSE(mode.shouldSimulateZeroPower());
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_disabled_mode_preserves_normal_reporting);
  RUN_TEST(test_enabled_mode_fails_safe_until_soc_arrives);
  RUN_TEST(test_enabled_mode_simulates_zero_until_full);
  RUN_TEST(test_invalid_soc_does_not_change_mode_or_previous_reading);
  RUN_TEST(test_manual_disable_restores_normal_reporting);
  RUN_TEST(test_cannot_enable_mode_when_soc_is_already_full);
  return UNITY_END();
}
