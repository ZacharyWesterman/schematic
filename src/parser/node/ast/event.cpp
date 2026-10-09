#include "event.hpp"
#include "../../../libs/luau.hpp"
#include "../../parse_error.hpp"

namespace parser::node::ast {

static auto text(token tok) -> zstring {
	return tok.text;
}

static auto join(const zstring &a, const zstring &b) -> zstring {
	return a + ", " + b;
}

auto event::print(std::ostream &stream, int indent) -> void {
	auto indent_text = " "_zs.repeat(indent * 2);
	(indent_text + "event [" + triggers.map<zstring>(text).reduce(join) + "] = `...`").writeln(stream);
}

auto event::validate() const -> void {
	// Make sure code block at least compiles.
	auto state = luaL_newstate();
	size_t chunksize = 0;
	auto chunk = luau_compile(code_block.text.cstring(), code_block.text.length(), nullptr, &chunksize);
	int error_code = luau_load(state, "code block", chunk, chunksize, 0);
	if (error_code) {
		int index = lua_gettop(state);
		auto error_msg = lua_tostring(state, index);
		throw parse_error("Lua error in event handler: "_zs + error_msg, filename, code_block.range);
	}
	lua_close(state);
}

} // namespace parser::node::ast
