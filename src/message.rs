use crate::lexer::Span;
use colored::Colorize;
use json;
use std::sync::Mutex;

//Thread safety. Not necessary yet but good practice.
static DID_ERROR: Mutex<bool> = Mutex::new(false);
pub static LANGUAGE_SERVER: Mutex<bool> = Mutex::new(false);

pub struct Context<'a> {
	pub filename: &'a String,
	pub source: &'a String,
}

fn print_message(text: String, span: Option<Span>, context: Option<&Context>) {
	eprintln!("{}", text);

	match context {
		None => {}
		Some(context) => {
			match span {
				None => {
					//"None" branch should only happen on EOF errors.

					let lines: Vec<&str> = context.source.lines().collect();

					//Print filename, and the last lines of the file
					eprintln!(
						"  {} {}:{}",
						"-->".bright_blue().bold(),
						context.filename,
						lines.len()
					);
					eprintln!("   {}", "|".bright_blue().bold());
					if lines.len() > 1 {
						eprintln!(
							"{:<3}{} {}",
							format!("{}", lines.len() - 1).bright_blue().bold(),
							"|".bright_blue().bold(),
							&lines[lines.len() - 2]
						);
					}
					if lines.len() > 0 {
						eprintln!(
							"{:<3}{} {} {}",
							format!("{}", lines.len()).bright_blue().bold(),
							"|".bright_blue().bold(),
							&lines[lines.len() - 1],
							"(EOF)".bright_blue().bold()
						);
						eprintln!(
							"   {} {} {}",
							"|".bright_blue().bold(),
							" ".repeat(lines[lines.len() - 1].len()),
							" ^^^".bright_blue().bold()
						);
					} else {
						eprintln!(
							"{:<3}{} {}",
							format!("{}", lines.len()).bright_blue().bold(),
							"|".bright_blue().bold(),
							"(EOF)".bright_blue().bold()
						);
						eprintln!("   {}", "|  ^^^".bright_blue().bold());
					}
				}
				Some(span) => {
					print_context(Some(context.filename), context.source, span);
				}
			};
		}
	};
}

pub fn abort() {
	if *LANGUAGE_SERVER.lock().unwrap() {
		return;
	}

	eprintln!(
		"{}: {}",
		"aborted".red().bold(),
		"Unable to continue due to previous errors".bold()
	);
}

pub fn error(text: String, span: Option<Span>, context: Option<&Context>) {
	let mut data = DID_ERROR.lock().unwrap();
	*data = true;

	if *LANGUAGE_SERVER.lock().unwrap() {
		match span {
			None => {}
			Some(s) => {
				let diagnostic = json::object! {
					"type": "error",
					"span": [s.lo, s.hi],
					"message": text,
				};
				println!("{}", json::stringify(diagnostic));
			}
		}
		return;
	}

	print_message(
		format!("{}: {}", "error".red().bold(), text.bold()),
		span,
		context,
	);
}

pub fn warning(text: String, span: Option<Span>, context: Option<&Context>) {
	if *LANGUAGE_SERVER.lock().unwrap() {
		match span {
			None => {}
			Some(s) => {
				let diagnostic = json::object! {
					"type": "warning",
					"span": [s.lo, s.hi],
					"message": text,
				};
				println!("{}", json::stringify(diagnostic));
			}
		}
		return;
	}

	print_message(
		format!("{}: {}", "warning".yellow().bold(), text.bold()),
		span,
		context,
	);
}

pub fn hint(text: String, span: Option<Span>, context: Option<&Context>) {
	if *LANGUAGE_SERVER.lock().unwrap() {
		match span {
			None => {}
			Some(s) => {
				let diagnostic = json::object! {
					"type": "hint",
					"span": [s.lo, s.hi],
					"message": text,
				};
				println!("{}", json::stringify(diagnostic));
			}
		}
		return;
	}

	match context {
		Some(ctx) => {
			match span {
				Some(span) => {
					let before = &ctx.source[0..span.lo];
					let line_begin = (1 + {
						let mut ix = -1;
						for (index, c) in before.char_indices().rev() {
							if c == '\n' {
								ix = index as isize;
								break;
							}
						}
						ix
					}) as usize;

					let col_no = span.lo - line_begin;

					//If hint is related to a previous message, print it differently
					eprintln!(
						"   {} {}{} {}",
						"|".bright_blue().bold(),
						" ".repeat(col_no),
						"∟".bright_blue().bold(),
						text
					);
				}

				None => {
					print_message(
						format!(
							"   {} {}: {}",
							"=".bright_blue().bold(),
							"hint".bold(),
							text
						),
						span,
						context,
					);
				}
			}
		}
		None => {
			print_message(
				format!(
					"   {} {}: {}",
					"=".bright_blue().bold(),
					"hint".bold(),
					text
				),
				span,
				context,
			);
		}
	}
}

pub fn info(text: &str) {
	if *LANGUAGE_SERVER.lock().unwrap() {
		return;
	}
	eprintln!("{}: {}", "info".bold(), text);
}

pub fn errored() -> bool {
	*DID_ERROR.lock().unwrap()
}

pub fn context(span: Span, context: &Context) {
	if *LANGUAGE_SERVER.lock().unwrap() {
		return;
	}
	print_context(Some(context.filename), context.source, span);
}

fn print_context(filename: Option<&String>, full_text: &String, span: Span) {
	let before = &full_text[0..span.lo];
	let after = &full_text[span.hi..full_text.len()];
	let line_begin = (1 + {
		let mut ix = -1;
		for (index, c) in before.char_indices().rev() {
			if c == '\n' {
				ix = index as isize;
				break;
			}
		}
		ix
	}) as usize;
	let line_end = span.hi + {
		let mut ix = 0;
		for (index, c) in after.char_indices() {
			ix = index;
			if c == '\n' {
				break;
			}
		}
		ix
	} - 1;

	let line_no = before.chars().filter(|&c| c == '\n').count() + 1;
	let col_no = span.lo - line_begin;

	//Print filename, line number and column number.
	match filename {
		None => {
			eprintln!(
				"  {} stdin:{}:{}",
				"-->".bright_blue().bold(),
				line_no,
				col_no
			);
		}
		Some(s) => {
			eprintln!(
				"  {} {}:{}:{}",
				"-->".bright_blue().bold(),
				s,
				line_no,
				col_no
			);
		}
	}

	//Print the lines in question and highlight what element is being referred to.
	eprintln!("   {}", "|".bright_blue().bold());

	let lines: Vec<&str> = full_text[line_begin..=line_end].lines().collect();
	let total = lines.len();
	let mut ct = 0;
	let mut max_len = 1;
	for line in lines {
		if ct == 0 || ct == total - 1 {
			eprintln!(
				"{:<3}{} {}",
				format!("{}", line_no + ct).bright_blue().bold(),
				"|".bright_blue().bold(),
				line
			);
			max_len = std::cmp::max(max_len, line.len() - 1);
		}
		if ct == 1 && total > 2 {
			eprintln!("   {}", "|    ...".bright_blue().bold());
		}
		ct += 1;
	}

	if ct == 1 {
		eprintln!(
			"   {} {}{}",
			"|".bright_blue().bold(),
			" ".repeat(span.lo - line_begin),
			"^".repeat(span.hi - span.lo).bright_blue().bold()
		);
	} else {
		eprintln!(
			"   {} {}{}",
			"|".bright_blue().bold(),
			" ".repeat(span.lo - line_begin),
			"^".repeat(max_len).bright_blue().bold()
		);
	}
}
