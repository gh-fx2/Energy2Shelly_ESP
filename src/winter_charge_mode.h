#pragma once

class WinterChargeMode {
 public:
  WinterChargeMode() : enabled_(false), soc_valid_(false), soc_(0) {}

  void setEnabled(bool enabled) {
    enabled_ = enabled && !(soc_valid_ && soc_ >= 100);
  }

  bool updateSoc(double soc) {
    if (!(soc >= 0 && soc <= 100)) {
      return false;
    }

    soc_ = soc;
    soc_valid_ = true;
    if (enabled_ && soc_ >= 100) {
      enabled_ = false;
    }
    return true;
  }

  bool isEnabled() const {
    return enabled_;
  }

  bool hasSoc() const {
    return soc_valid_;
  }

  double soc() const {
    return soc_;
  }

  bool shouldSimulateZeroPower() const {
    return enabled_ && (!soc_valid_ || soc_ < 100);
  }

 private:
  bool enabled_;
  bool soc_valid_;
  double soc_;
};
