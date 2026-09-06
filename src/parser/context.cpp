#include "context.hpp"

namespace parser {

context::context(std::istream &stream, span range) : line{0, 0}, col{range.start, range.end} {
	stream.seekg(std::ios_base::beg);

	int start_index = 0;
	int end_index = 0;
	zstring this_line;
	while (!stream.eof()) {
		this_line.read(stream, '\n');
		int char_ct = this_line.length() + 1; // Account for the '\n' character

		if (start_index + char_ct < range.start) {
			// Skip everything before the begin line
			start_index += char_ct;
			end_index = start_index;
			line.end = line.start += 1;
			col.start -= char_ct;
			col.end -= char_ct;
		} else if (end_index + char_ct < range.end) {
			// Include everything inside context
			if (text) {
				text += '\n';
			}
			text += this_line;

			line.end++;
			col.end -= char_ct;
		} else {
			// Read the last line, then exit.
			if (text) {
				text += '\n';
			}
			text += this_line;
			break;
		}
	}

	// Make line and column start at 1 instead of 0.
	line.start++;
	line.end++;
	col.start++;
	col.end++;
}

} // namespace parser
