#pragma once
#include "parse_error.hpp"

namespace parser {

auto print_error(const parse_error &error) -> void;

}
