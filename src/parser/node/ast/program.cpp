#include "program.hpp"
#include "../../parse_error.hpp"
#include <z/core/string.hpp>

namespace parser::node::ast {

auto program::print(std::ostream &stream, int indent) -> void {
	(" "_zs.repeat(indent * 2) + "program").writeln(stream);
	for (auto tag : tags) {
		tag->print(stream, indent + 1);
	}
	for (auto node : nodes) {
		node->print(stream, indent + 1);
	}
}

auto program::get_parent_tag_index(const token &tag) const -> int {
	for (int i = 0; i < tags.length(); i++) {
		if (tags[i]->name.text == tag.text) {
			return i;
		}
	}

	throw parse_error("Tag `"_zs + tag.text + "` does not exist.", tag.filename, tag.range);
}

auto program::validate() const -> void {
	for (int i = 0; i < tags.length(); i++) {
		// Make sure tag hasn't already been defined.
		for (int j = 0; j < i; j++) {
			if (j < i && tags[i]->name.text == tags[j]->name.text) {
				throw parse_error("Tag `"_zs + tags[i]->name.text + "` defined multiple times.", tags[i]->filename, tags[i]->name.range);
			}
		}

		// Make sure tag parent exists, and isn't circularly defined (e.g. @1 -> @2 -> @1, etc)
		int tag_index = i;
		zstring parent_route = tags[i]->name.text;
		while (tags[tag_index]->parent) {
			parent_route += "->"_zs + tags[tag_index]->parent.value().text;
			tag_index = get_parent_tag_index(tags[tag_index]->parent.value());
			if (tag_index == i) {
				throw parse_error("Tag `"_zs + tags[i]->name.text + "` has a circular dependency. (" + parent_route + ")", tags[i]->filename, tags[i]->name.range);
			}
		}
	}

	for (int i = 0; i < nodes.length(); i++) {
		// Make sure node hasn't already been defined.
		for (int j = 0; j < i; j++) {
			if (nodes[i]->name.text == nodes[j]->name.text) {
				throw parse_error("Node `"_zs + nodes[i]->name.text + "` defined multiple times.", nodes[i]->filename, nodes[i]->name.range);
			}
		}

		// Make sure all tags exist.
		for (auto tag : nodes[i]->tags) {
			get_parent_tag_index(tag);
		}

		nodes[i]->validate();
	}
}

} // namespace parser::node::ast
