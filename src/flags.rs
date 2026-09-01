use std::path::PathBuf;
use structopt::StructOpt;

#[derive(Debug, StructOpt)]
#[structopt(
	name = "Schematic",
	about = "A WIP project for compiling a node-based language."
)]
pub struct Options {
	/// Output detailed info in an easy-to-parse format
	#[structopt(long)]
	pub language_server: bool,

	/// The input file
	#[structopt(parse(from_os_str))]
	pub input: PathBuf,
}

pub fn read() -> Options {
	return Options::from_args();
}
