#include "quantile.h"

namespace xgboost {
namespace common {
namespace {

constexpr bool kEnableObliviousCombine = false;
constexpr bool kEnableObliviousPrune = false;
constexpr bool kEnableObliviousDebugCheck = true;

} // namespace

bool ObliviousSetCombineEnabled() {
  return kEnableObliviousCombine;
}

bool ObliviousSetPruneEnabled() {
  return kEnableObliviousPrune;
}

bool ObliviousDebugCheckEnabled() {
  return kEnableObliviousDebugCheck;
}

}  // namespace common
}  // namespace xgboost