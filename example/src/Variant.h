#pragma once

#include <variant>

template<class... As> struct visitor : As... { using As::operator()...; };
template<class... As> visitor(As...) -> visitor<As...>;
