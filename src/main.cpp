#include "parser/node/lexer.hpp"
#include <iostream>
#include <regex>

int main() {
	auto node_parser = parser::node_parser("[@tag] node math as \"Math Node\" {}");

	for (auto token : node_parser) {
		token.text.writeln(std::cout);
	}

	// "Hello World"_zs.writeln(std::cout);
}
