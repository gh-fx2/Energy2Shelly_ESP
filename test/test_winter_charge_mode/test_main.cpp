#include <unity.h>
#include <limits>
#include "../../src/winter_charge_mode.h"

void test_disabled_mode_preserves_normal_reporting() {
  WinterChargeMode mode;
  mode.updateSoc(40);
  TEST_ASSERT_FALSE(mode.isActive());
}

void test_enabled_mode_fails_safe_until_soc_arrives() {
  WinterChargeMode mode;
  mode.setEnabled(true);
  TEST_ASSERT_TRUE(mode.isActive());
  TEST_ASSERT_EQUAL_DOUBLE(0, mode.reportedPower(200));
}

void test_winter_mode_limits_power_until_soc_reaches_full() {
  WinterChargeMode mode;
  mode.setEnabled(true);
  TEST_ASSERT_TRUE(mode.updateSoc(99.9));
  TEST_ASSERT_TRUE(mode.isActive());
  TEST_ASSERT_EQUAL_DOUBLE(0, mode.reportedPower(200));
  TEST_ASSERT_TRUE(mode.updateBatteryChargePower(200));
  TEST_ASSERT_TRUE(mode.updateSoc(100));
  TEST_ASSERT_FALSE(mode.isEnabled());
  TEST_ASSERT_FALSE(mode.isActive());
  TEST_ASSERT_EQUAL_DOUBLE(300, mode.reportedPower(300));
}

void test_invalid_soc_does_not_change_mode_or_previous_reading() {
  WinterChargeMode mode;
  mode.setEnabled(true);
  TEST_ASSERT_TRUE(mode.updateSoc(50));
  TEST_ASSERT_FALSE(mode.updateSoc(101));
  TEST_ASSERT_FALSE(mode.updateSoc(-1));
  TEST_ASSERT_FALSE(mode.updateSoc(std::numeric_limits<double>::quiet_NaN()));
  TEST_ASSERT_EQUAL_DOUBLE(50, mode.soc());
  TEST_ASSERT_TRUE(mode.isActive());
}

void test_manual_disable_restores_normal_reporting() {
  WinterChargeMode mode;
  mode.setEnabled(true);
  mode.updateSoc(50);
  mode.setEnabled(false);
  TEST_ASSERT_FALSE(mode.isActive());
}

void test_cannot_enable_mode_when_soc_is_already_full() {
  WinterChargeMode mode;
  TEST_ASSERT_TRUE(mode.updateSoc(100));
  mode.setEnabled(true);
  TEST_ASSERT_FALSE(mode.isEnabled());
  TEST_ASSERT_FALSE(mode.isActive());
}

void test_default_soc_threshold_is_95_percent() {
  WinterChargeMode mode;
  TEST_ASSERT_EQUAL_DOUBLE(95, mode.socThreshold());
  mode.setEnabled(true);
  TEST_ASSERT_TRUE(mode.updateSoc(94.9));
  TEST_ASSERT_TRUE(mode.isActive());
  TEST_ASSERT_TRUE(mode.updateSoc(95));
  TEST_ASSERT_FALSE(mode.isEnabled());
  TEST_ASSERT_FALSE(mode.isActive());
}

void test_custom_soc_threshold_disables_winter_mode() {
  WinterChargeMode mode;
  TEST_ASSERT_TRUE(mode.setSocThreshold(90));
  mode.setEnabled(true);
  TEST_ASSERT_TRUE(mode.updateSoc(89.9));
  TEST_ASSERT_TRUE(mode.isActive());
  TEST_ASSERT_TRUE(mode.updateSoc(90));
  TEST_ASSERT_FALSE(mode.isEnabled());
}

void test_100_percent_threshold_preserves_full_soc_behavior() {
  WinterChargeMode mode;
  TEST_ASSERT_TRUE(mode.setSocThreshold(100));
  mode.setEnabled(true);
  TEST_ASSERT_TRUE(mode.updateSoc(99.9));
  TEST_ASSERT_TRUE(mode.isActive());
  TEST_ASSERT_TRUE(mode.updateSoc(100));
  TEST_ASSERT_FALSE(mode.isEnabled());
}

void test_invalid_soc_threshold_is_rejected_without_replacing_value() {
  WinterChargeMode mode;
  TEST_ASSERT_FALSE(mode.setSocThreshold(-1));
  TEST_ASSERT_FALSE(mode.setSocThreshold(101));
  TEST_ASSERT_FALSE(mode.setSocThreshold(
      std::numeric_limits<double>::quiet_NaN()));
  TEST_ASSERT_EQUAL_DOUBLE(95, mode.socThreshold());
}

void test_import_power_is_limited_to_current_battery_charge_power() {
  WinterChargeMode mode;
  mode.setEnabled(true);
  TEST_ASSERT_TRUE(mode.updateSoc(80));
  TEST_ASSERT_TRUE(mode.updateBatteryChargePower(200));
  TEST_ASSERT_EQUAL_DOUBLE(200, mode.reportedPower(600));
  TEST_ASSERT_EQUAL_DOUBLE(150, mode.reportedPower(150));
}

void test_export_power_passes_through_unchanged() {
  WinterChargeMode mode;
  mode.setEnabled(true);
  TEST_ASSERT_TRUE(mode.updateSoc(80));
  TEST_ASSERT_TRUE(mode.updateBatteryChargePower(200));
  TEST_ASSERT_EQUAL_DOUBLE(-200, mode.reportedPower(-200));
}

void test_import_is_zero_when_charge_power_is_missing_or_not_positive() {
  WinterChargeMode mode;
  mode.setEnabled(true);
  TEST_ASSERT_TRUE(mode.updateSoc(80));
  TEST_ASSERT_EQUAL_DOUBLE(0, mode.reportedPower(200));
  TEST_ASSERT_TRUE(mode.updateBatteryChargePower(-20));
  TEST_ASSERT_EQUAL_DOUBLE(0, mode.reportedPower(200));
}

void test_invalid_charge_power_does_not_replace_previous_reading() {
  WinterChargeMode mode;
  mode.setEnabled(true);
  TEST_ASSERT_TRUE(mode.updateSoc(80));
  TEST_ASSERT_TRUE(mode.updateBatteryChargePower(120));
  TEST_ASSERT_FALSE(mode.updateBatteryChargePower(
      std::numeric_limits<double>::quiet_NaN()));
  TEST_ASSERT_EQUAL_DOUBLE(120, mode.reportedPower(200));
}

void test_disabled_mode_does_not_limit_power() {
  WinterChargeMode mode;
  TEST_ASSERT_TRUE(mode.updateBatteryChargePower(120));
  TEST_ASSERT_EQUAL_DOUBLE(200, mode.reportedPower(200));
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_disabled_mode_preserves_normal_reporting);
  RUN_TEST(test_enabled_mode_fails_safe_until_soc_arrives);
  RUN_TEST(test_winter_mode_limits_power_until_soc_reaches_full);
  RUN_TEST(test_invalid_soc_does_not_change_mode_or_previous_reading);
  RUN_TEST(test_manual_disable_restores_normal_reporting);
  RUN_TEST(test_cannot_enable_mode_when_soc_is_already_full);
  RUN_TEST(test_default_soc_threshold_is_95_percent);
  RUN_TEST(test_custom_soc_threshold_disables_winter_mode);
  RUN_TEST(test_100_percent_threshold_preserves_full_soc_behavior);
  RUN_TEST(test_invalid_soc_threshold_is_rejected_without_replacing_value);
  RUN_TEST(test_import_power_is_limited_to_current_battery_charge_power);
  RUN_TEST(test_export_power_passes_through_unchanged);
  RUN_TEST(test_import_is_zero_when_charge_power_is_missing_or_not_positive);
  RUN_TEST(test_invalid_charge_power_does_not_replace_previous_reading);
  RUN_TEST(test_disabled_mode_does_not_limit_power);
  return UNITY_END();
}
