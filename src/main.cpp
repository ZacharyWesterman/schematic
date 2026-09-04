#include "parser/node/lexer.hpp"
#include "parser/parse_error.hpp"
#include "version.hpp"
#include <iostream>

int main() {
	std::cout << "Node Parser version " << VERSION << std::endl;

	auto node_lexer = parser::node_lexer(R"(
		@math as "Arithmetic"

		[@math]
		node add as "Add" {
			"Add two numbers"

			//in val1: number as "V1"
			//in val2: number as "V2"
		}
	)");

	try {
		for (auto token : node_lexer) {
			token.text.writeln(std::cout);
		}
	} catch (const parser::parse_error &e) {
		("ERROR: "_zs + e.context.start + ", " + e.context.end + ": " + e.message).writeln(std::cout);
	}
}
