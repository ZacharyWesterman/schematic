#pragma once
#include "parse_error.hpp"
#include <istream>

namespace parser {

auto print_error(std::istream &file, const parse_error &error) -> void;

}
