#include "node_decl.hpp"
#include "../../parse_error.hpp"
#include "../tokens.hpp"
#include <z/core/string.hpp>

namespace parser::node::ast {

static auto text(token tok) -> zstring {
	return tok.text;
}

static auto join(const zstring &a, const zstring &b) -> zstring {
	return a + ", " + b;
}

auto node_decl::print(std::ostream &stream, int indent) -> void {
	auto indent_text = " "_zs.repeat(indent * 2);
	(indent_text + "node_decl [" + name.text + "]").writeln(stream);
	indent_text += "  ";

	if (description.text) {
		(indent_text + "desc: " + description.text).writeln(stream);
	}
	(indent_text + "tags: " + tags.map<zstring>(text).reduce(join)).writeln(stream);

	if (help_text) {
		(indent_text + "help-text: " + help_text.value().text).writeln(stream);
	}

	if (inputs.length()) {
		(indent_text + "inputs: [").writeln(stream);
		for (auto i : inputs) {
			i->print(stream, indent + 2);
		}
		(indent_text + "]").writeln(stream);
	}

	if (outputs.length()) {
		(indent_text + "outputs: [").writeln(stream);
		for (auto i : outputs) {
			i->print(stream, indent + 2);
		}
		(indent_text + "]").writeln(stream);
	}

	if (events.length()) {
		(indent_text + "events: [").writeln(stream);
		for (auto i : events) {
			i->print(stream, indent + 2);
		}
		(indent_text + "]").writeln(stream);
	}

	for (auto i : includes) {
		i->print(stream, indent + 1);
	}
}

auto node_decl::validate() const -> void {
	for (int i = 0; i < inputs.length(); i++) {
		// Make sure input names don't repeat.
		for (int j = 0; j < i; j++) {
			if (inputs[i]->name.text == inputs[j]->name.text) {
				throw parse_error("An input named `"_zs + inputs[i]->name.text + "` already exists in this node.", inputs[i]->name.range);
			}
		}

		inputs[i]->validate();
	}

	for (int i = 0; i < outputs.length(); i++) {
		// Make sure output names don't repeat input names.
		for (int j = 0; j < inputs.length(); j++) {
			if (outputs[i]->name.text == inputs[j]->name.text) {
				throw parse_error("An input named `"_zs + outputs[i]->name.text + "` already exists in this node.", outputs[i]->name.range);
			}
		}

		// Make sure output names don't repeat.
		for (int j = 0; j < i; j++) {
			if (outputs[i]->name.text == outputs[j]->name.text) {
				throw parse_error("An output named `"_zs + outputs[i]->name.text + "` already exists in this node.", outputs[i]->name.range);
			}
		}

		outputs[i]->validate();
	}

	for (auto event : events) {
		for (auto trigger : event->triggers) {
			if (trigger.id == tokens::IDENTIFIER) {
				// Make sure every non-automatic event trigger refers to an existing input.
				bool found_input = false;
				for (int j = 0; j < inputs.length(); j++) {
					if (trigger.text == inputs[j]->name.text) {
						found_input = true;
						break;
					}
				}

				if (!found_input) {
					throw parse_error("No input with the name `"_zs + trigger.text + "` exists in this node.", trigger.range);
				}
			} else {
				if (trigger.text != "tick" && trigger.text != "init") {
					throw parse_error("Invalid automatic event `!"_zs + trigger.text + "`. Supported events are `!tick` and `!init`.", trigger.range);
				}
			}
		}
	}
}

} // namespace parser::node::ast
