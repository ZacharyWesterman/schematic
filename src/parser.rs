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
		TagStmt(Box<Tag>),
		NodeStmt(Box<Node>),
	}

	#[derive(Debug)]
	pub struct Tag {
		pub span: Span,
		pub name: TagIdent,
		pub parent: Option<TagIdent>,
		pub label: Text,
		pub description: Option<String>,
	}

	#[derive(Debug)]
	pub struct Node {
		pub span: Span,
		pub name: Ident,
		pub label: Text,
		pub description: Option<String>,
		pub body: Vec<NodeBlock>,
		pub tags: Vec<TagIdent>,
		pub partial: bool,
		pub parent: Option<Ident>,
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
		NodeCode(Box<Code>),
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
		pub constraints: Vec<Constraint>,
	}

	#[derive(Debug)]
	pub struct Event {
		pub span: Span,
		pub triggers: Vec<EventType>,
		pub code: Code,
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

	#[derive(Debug)]
	pub struct TagIdent {
		pub span: Span,
		pub value: String,
	}

	#[derive(Debug)]
	pub struct Number {
		pub span: Span,
		pub value: f64,
	}

	#[derive(Debug)]
	pub struct Constraint {
		pub span: Span,
		pub name: Ident,
		pub values: Vec<ConstraintValue>,
	}

	#[derive(Debug)]
	pub enum ConstraintValue {
		NumberConstraint(Number),
		IdentConstraint(Ident),
		TextConstraint(Text),
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
		statements[mut st] statement[n] => {
			st.push(n);
			st
		},
	}

	statement: Statement {
		tag[n] => Statement {
			span: span!(),
			node: Stmt::TagStmt(Box::new(n)),
		},

		node[n] => Statement {
			span: span!(),
			node: Stmt::NodeStmt(Box::new(n)),
		},
	}

	tag: Tag {
		tag_ident[name] tag_parent[parent] KwdAs text[label]  LBrace description[desc] RBrace => Tag {
			span: span!(),
			name: name,
			parent: parent,
			label: label,
			description: desc,
		},

		tag_ident[name] tag_parent[parent] KwdAs text[label] => Tag {
			span: span!(),
			name: name,
			parent: parent,
			label: label,
			description: None,
		},
	}

	description: Option<String> {
		=> None,
		TextBlock(desc) => Some(desc),
	}

	tag_parent: Option<TagIdent> {
		=> None,
		KwdIn tag_ident[parent] => Some(parent),
	}

	tag_decl_list: Vec<Tag> {
		=> vec![],
		tag_decl_list[mut st] tag[n] => {
			st.push(n);
			st
		},
	}

	node: Node {
		node_tags[tags] node_partial[partial] KwdNode ident[name] node_parent[parent] KwdAs text[label] LBrace description[desc] node_body[body] RBrace => Node {
			span: span!(),
			name: name,
			label: label,
			description: desc,
			body: body,
			tags: tags,
			partial: partial,
			parent: parent,
		},
	}

	node_partial: bool {
		KwdPartial => true,
		=> false,
	}

	node_parent: Option<Ident> {
		KwdExtends ident[n] => Some(n),
		=> None,
	}

	node_body: Vec<NodeBlock> {
		=> vec![],
		node_body[mut st] node_block[n] => {
			st.push(n);
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

		code[n] => NodeBlock {
			span: span!(),
			block: Block::NodeCode(Box::new(n)),
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
		KwdOn event_triggers[triggers] code[event_code] => Event {
			span: span!(),
			triggers: triggers,
			code: event_code,
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

	field_list: Vec<Field> {
		=> vec![],
		field_list[mut st] field[n] => {
			st.push(n);
			st
		},
	}

	field: Field {
		ident[name] Colon ident[typedecl] KwdAs text[label] constraint_block[constraints] => Field {
			span: span!(),
			name: name,
			typedecl: typedecl,
			label: label,
			constraints: constraints,
		},
	}

	constraint_block: Vec<Constraint> {
		=> vec![],
		LBrace constraint_list[constraints] RBrace => constraints,
		LBrace constraint_list[constraints] Comma RBrace => constraints,
	}

	constraint_list: Vec<Constraint> {
		constraint[n] => vec![n],
		constraint_list[mut st] Comma constraint[n] => {
			st.push(n);
			st
		},
	}

	constraint: Constraint {
		ident[name] Colon constraint_values[values] => Constraint {
			span: span!(),
			name: name,
			values: values,
		},
	}

	constraint_values: Vec<ConstraintValue> {
		constraint_value[n] => vec![n],
		constraint_values[mut st] constraint_value[n] => {
			st.push(n);
			st
		},
	}

	constraint_value: ConstraintValue {
		ident[n] => ConstraintValue::IdentConstraint(n),
		text[n] => ConstraintValue::TextConstraint(n),
		number[n] => ConstraintValue::NumberConstraint(n),
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

	number: Number {
		NumberLiteral(value) => Number {
			span: span!(),
			value: value,
		},
	}

	tag_ident: TagIdent {
		TagIdentifier(value) => TagIdent {
			span: span!(),
			value: value,
		},
	}

	node_tags: Vec<TagIdent> {
		=> vec![],
		LBracket tag_list[n] RBracket => n,
	}

	tag_list: Vec<TagIdent> {
		tag_ident[n] => vec![n],
		tag_list[mut st] Comma tag_ident[n] => {
			st.push(n);
			st
		},
	}
}

pub fn parse<I: Iterator<Item = (Token, Span)>>(
	i: I,
) -> Result<Program, (Option<(Token, Span)>, &'static str)> {
	parse_(i)
}

// #[cfg(debug_assertions)]
use colored::Colorize;
// #[cfg(debug_assertions)]
use regex::Regex;
// #[cfg(debug_assertions)]
pub fn pretty(ast: &Program) -> String {
	let fluff = Regex::new(r"\n *[\)\}\]],?").unwrap();
	let spans = Regex::new(r"\n *(lo|hi)").unwrap();
	let other = Regex::new(r"\n *span:[^\n]+").unwrap();

	let text = format!("{:#?}", ast).replace("    ", "  ");

	let s1 = fluff.replace_all(&text, "");
	let s2 = spans.replace_all(&s1, " $1");
	let s3 = other.replace_all(&s2, "$1 $3".bold().yellow().to_string());

	return s3.to_string();
}
