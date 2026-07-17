/** @file
 *  Static assertion utilities for compile-time validation
 */
#pragma once

#include "indep/common/typedef.hpp"
#include <type_traits>

namespace by {

    template <nbool...> struct boolPod;
    template <nbool... bp> using allTrues = std::is_same<boolPod<bp..., true>, boolPod<true, bp...>>;
} // namespace by
