pub mod ast {
	use crate::lexer::Span;

	#[derive(Debug)]
	pub struct Program {
		pub stmts: Vec<Statement>,
	}

	#[derive(Debug)]
	pub struct Statement {
		pub span: Span,
		pub node: Stmt,
	}

	#[derive(Debug)]
	pub enum Stmt {
		NodeStmt(Box<Node>),
	}

	#[derive(Debug)]
	pub struct Node {
		pub span: Span,
		pub name: Ident,
		pub label: Text,
		pub description: String,
		pub body: Vec<NodeBlock>,
	}

	#[derive(Debug)]
	pub struct NodeBlock {
		pub span: Span,
		pub block: Block,
	}

	#[derive(Debug)]
	pub enum Block {
		NodeInputs(Box<FieldList>),
		NodeOutputs(Box<FieldList>),
		NodeEvent(Box<Event>),
	}

	#[derive(Debug)]
	pub struct FieldList {
		pub span: Span,
		pub fields: Vec<Field>,
	}

	#[derive(Debug)]
	pub struct Field {
		pub span: Span,
		pub name: Ident,
		pub typedecl: Ident,
		pub label: Text,
	}

	#[derive(Debug)]
	pub struct Event {
		pub span: Span,
		pub triggers: Vec<EventType>,
		pub outputs: Vec<EventOutput>,
	}

	#[derive(Debug)]
	pub struct EventOutput {
		pub span: Span,
		pub name: Ident,
		pub code: Option<Code>,
	}

	#[derive(Debug)]
	pub enum EventType {
		InputEvent(Ident),
		InternalEvent(EventIdent),
	}

	#[derive(Debug)]
	pub struct EventIdent {
		pub span: Span,
		pub value: String,
	}

	#[derive(Debug)]
	pub struct Ident {
		pub span: Span,
		pub value: String,
	}

	#[derive(Debug)]
	pub struct Text {
		pub span: Span,
		pub value: String,
	}

	#[derive(Debug)]
	pub struct Code {
		pub span: Span,
		pub value: String,
	}
}

use crate::lexer::Token::*;
use crate::lexer::*;
use ast::*;
use plex::parser;

parser! {
	fn parse_(Token, Span);

	//Combine two spans.
	(a, b) {
		Span {
			lo: a.lo,
			hi: b.hi,
		}
	}

	program: Program {
		statements[s] => Program { stmts: s },
	}

	statements: Vec<Statement> {
		=> vec![],
		statements[mut st] statement[e] => {
			st.push(e);
			st
		},
	}

	statement: Statement {
		node[n] => Statement {
			span: span!(),
			node: Stmt::NodeStmt(Box::new(n)),
		},
	}

	node: Node {
		KwdNode ident[name] KwdAs text[label] LBrace TextBlock(description) node_body[body] RBrace => Node {
			span: span!(),
			name: name,
			label: label,
			description: description,
			body: body,
		},
	}

	node_body: Vec<NodeBlock> {
		=> vec![],
		node_body[mut st] node_block[e] => {
			st.push(e);
			st
		},
	}

	node_block: NodeBlock {
		inputs[n] => NodeBlock {
			span: span!(),
			block: Block::NodeInputs(Box::new(n)),
		},

		outputs[n] => NodeBlock {
			span: span!(),
			block: Block::NodeOutputs(Box::new(n)),
		},

		event[n] => NodeBlock {
			span: span!(),
			block: Block::NodeEvent(Box::new(n)),
		},
	}

	inputs: FieldList {
		KwdIn LBrace field_list[n] RBrace => FieldList {
			span: span!(),
			fields: n,
		},

		KwdIn field[n] => FieldList {
			span: span!(),
			fields: vec![n],
		},
	}

	outputs: FieldList {
		KwdOut LBrace field_list[n] RBrace => FieldList {
			span: span!(),
			fields: n,
		},

		KwdOut field[n] => FieldList {
			span: span!(),
			fields: vec![n],
		},
	}

	event: Event {
		KwdOn event_triggers[triggers] LBrace event_outputs[output_list] RBrace => Event {
			span: span!(),
			triggers: triggers,
			outputs: output_list,
		},
	}

	event_triggers: Vec<EventType> {
		event_type[n] => vec![n],
		event_triggers[mut st] Comma event_type[n] => {
			st.push(n);
			st
		},
	}

	event_type: EventType {
		ident[n] => EventType::InputEvent(n),
		event_ident[n] => EventType::InternalEvent(n),
	}

	event_outputs: Vec<EventOutput> {
		=> vec![],
		event_outputs[mut st] event_output[n] => {
			st.push(n);
			st
		},
	}

	event_output: EventOutput {
		ident[name] code[value] => EventOutput {
			span: span!(),
			name: name,
			code: Some(value),
		},

		ident[name] => EventOutput {
			span: span!(),
			name: name,
			code: None,
		},
	}

	field_list: Vec<Field> {
		=> vec![],
		field_list[mut st] field[n] => {
			st.push(n);
			st
		},
	}

	field: Field {
		ident[name] Colon ident[typedecl] KwdAs text[label] => Field {
			span: span!(),
			name: name,
			typedecl: typedecl,
			label: label,
		},
	}

	event_ident: EventIdent {
		EventIdentifier(value) => EventIdent {
			span: span!(),
			value: value,
		},
	}

	ident: Ident {
		Identifier(value) => Ident {
			span: span!(),
			value: value,
		},
	}

	text: Text {
		TextBlock(value) => Text {
			span: span!(),
			value: value,
		},
	}

	code: Code {
		CodeBlock(value) => Code {
			span: span!(),
			value: value,
		},
	}
}

pub fn parse<I: Iterator<Item = (Token, Span)>>(
	i: I,
) -> Result<Program, (Option<(Token, Span)>, &'static str)> {
	parse_(i)
}

#[cfg(debug_assertions)]
use colored::Colorize;
#[cfg(debug_assertions)]
use regex::Regex;
#[cfg(debug_assertions)]
pub fn pretty(ast: &Program) -> String {
	let fluff = Regex::new(r"\n *[\)\}\]],?").unwrap();
	let spans = Regex::new(r"\n *(lo|hi)").unwrap();
	let other = Regex::new(r"((Literal|Var)\()\n *([^\n]+)").unwrap();

	let text = format!("{:#?}", ast).replace("    ", "  ");

	let s1 = fluff.replace_all(&text, "");
	let s2 = spans.replace_all(&s1, " $1");
	let s3 = other.replace_all(&s2, "$1 $3".bold().yellow().to_string());

	return s3.to_string();
}
