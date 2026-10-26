#pragma once

#include <cmath>

class WinterChargeMode {
 public:
  WinterChargeMode()
      : enabled_(false),
        soc_valid_(false),
        soc_(0),
        battery_charge_power_valid_(false),
        battery_charge_power_(0),
        soc_threshold_(95),
        last_reported_power_(0) {}

  void setEnabled(bool enabled) {
    enabled_ = enabled;
  }

  bool setSocThreshold(double threshold) {
    if (!std::isfinite(threshold) || threshold < 0 || threshold > 100) {
      return false;
    }

    soc_threshold_ = threshold;
    if (enabled_ && soc_valid_ && soc_ >= soc_threshold_) {
      enabled_ = false;
    }
    return true;
  }

  double socThreshold() const {
    return soc_threshold_;
  }

  double batteryChargePower() const {
    return battery_charge_power_;
  }

  bool updateSoc(double soc) {
    if (!std::isfinite(soc) || soc < 0 || soc > 100) {
      return false;
    }

    soc_ = soc;
    soc_valid_ = true;
    if (enabled_ && soc_ >= soc_threshold_) {
      enabled_ = false;
    }
    return true;
  }

  bool updateBatteryChargePower(double power) {
    if (!std::isfinite(power)) {
      return false;
    }

    battery_charge_power_ = power;
    battery_charge_power_valid_ = true;
    return true;
  }

  bool isEnabled() const {
    return enabled_;
  }

  bool hasSoc() const {
    return soc_valid_;
  }

  bool hasBatteryChargePower() const {
    return battery_charge_power_valid_;
  }

  double soc() const {
    return soc_;
  }

  bool isActive() const {
    return enabled_ && (!soc_valid_ || soc_ < soc_threshold_);
  }

  double reportedPower(double netPower) {
    double value = netPower;
    double charge = battery_charge_power_ < 0 ? -battery_charge_power_ : 0;
    double discharge = battery_charge_power_ < 0 ? battery_charge_power_ : 0;
    
    if (isActive() && ( value > charge ))
    {
      if ( discharge > 0) {
        value -= discharge;
      } else if (value > 0) {
        value = (charge*3+last_reported_power_)/4;
      }
    }
    else if ( isActive() && ( discharge > 5 ) )
    {
      value = -discharge;
    }

    last_reported_power_ = value;

    return value;
  }

  double lastReportedPower() const {
    return last_reported_power_;
  }

 private:
  bool enabled_;
  bool soc_valid_;
  double soc_;
  bool battery_charge_power_valid_;
  double battery_charge_power_;
  double soc_threshold_;
  double last_reported_power_;
};
