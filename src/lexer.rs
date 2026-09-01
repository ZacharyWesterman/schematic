use crate::message;
use plex::lexer;

#[derive(Debug, Clone)]
pub enum Token {
	//Ignored Tokens
	Whitespace,
	Comment,
	Unknown(String),

	//Keywords
	KwdNode,
	KwdAs,
	KwdIn,
	KwdOut,
	KwdOn,
	KwdInclude,

	//Identifiers
	Identifier(String),
	TextBlock(String),
	CodeBlock(String),
	TagIdentifier(String),
	EventIdentifier(String),
	NumberLiteral(f64),

	//Language Structures
	LBrace,
	RBrace,
	LBracket,
	RBracket,
	Colon,
	Comma,
}

lexer! {
	fn next_token(text: 'a) -> Token;

	//Ignored Tokens
	r"[ \t\r\n]" => Token::Whitespace,
	r"/\*(~(.*\*/.*))\*/" => Token::Comment, // Multi-line comments, `/* ... */`
	r"//[^\n]*" => Token::Comment, // Single-line comments, `//`

	//Keywords
	"node" => Token::KwdNode,
	"as" => Token::KwdAs,
	"in" => Token::KwdIn,
	"out" => Token::KwdOut,
	"on" => Token::KwdOn,
	"include" => Token::KwdInclude,

	//Values
	"[a-zA-Z_][a-zA-Z_0-9]*" => Token::Identifier(text.to_owned()),
	"\"[^\"]*\"" => Token::TextBlock(text[1..text.len()-1].to_owned()),
	"`[^`]*`" => Token::CodeBlock(text[1..text.len()-1].to_owned()),
	"@[a-zA-Z_0-9]+" => Token::TagIdentifier(text[1..].to_owned()),
	"![a-zA-Z_0-9]+" => Token::EventIdentifier(text[1..].to_owned()),
	r"[0-9][0-9_]*(\.[0-9_]*)?" => Token::NumberLiteral(text.replace(r"_", "").parse::<f64>().unwrap_or(0.0)),

	//Language Structures
	r"\{" => Token::LBrace,
	r"\}" => Token::RBrace,
	r"\[" => Token::LBracket,
	r"\]" => Token::RBracket,
	":" => Token::Colon,
	"," => Token::Comma,
}

pub struct Lexer<'a> {
	original: &'a str,
	remaining: &'a str,
	context: &'a message::Context<'a>,
}

impl<'a> Lexer<'a> {
	pub fn new(context: &'a message::Context) -> Lexer<'a> {
		Lexer {
			original: context.source,
			remaining: context.source,
			context: context,
		}
	}
}

#[derive(Debug, Clone, Copy)]
pub struct Span {
	pub lo: usize,
	pub hi: usize,
}

impl<'a> Iterator for Lexer<'a> {
	type Item = (Token, Span);
	fn next(&mut self) -> Option<(Token, Span)> {
		loop {
			let (tok, span) = if let Some((tok, new_remaining)) = next_token(self.remaining) {
				let lo = self.original.len() - self.remaining.len();
				let hi = self.original.len() - new_remaining.len();
				self.remaining = new_remaining;
				(tok, Span { lo, hi })
			} else {
				return None;
			};

			match tok {
				Token::Whitespace | Token::Comment => {
					continue;
				}

				Token::Unknown(text) => {
					message::error(
						format!("Unexpected character `{}`", text),
						Some(span),
						Some(&self.context),
					);
					continue;
				}

				tok => {
					return Some((tok, span));
				}
			}
		}
	}
}
