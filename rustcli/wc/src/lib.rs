use clap::{App, Arg};
use std::{error::Error, io::{BufRead, BufReader, self}, fs::File};

type MyResult<T> = Result<T, Box<dyn Error>>;

#[derive(Debug)]
pub struct Config {
    files: Vec<String>,
    lines: bool,
    words: bool,
    chars: bool,
    bytes: bool,
}

pub fn get_args() -> MyResult<Config> {
    let matches = App::new("wc")
        .version("0.0.1")
        .about("rust version of wc")
        .author("lyy de tiancai jiayi")
        .arg(
            Arg::with_name("files")
                .value_name("FILE")
                .help("Input files")
                .default_value("-")
                .multiple(true)
        )
        .arg(
            Arg::with_name("words")
                .short("w")
                .long("words")
                .help("show word count")
                .takes_value(false)
        )
        .arg(
            Arg::with_name("bytes")
                .short("b")
                .long("bytes")
                .help("show byte count")
                .takes_value(false)
        )
        .arg(
            Arg::with_name("chars")
                .short("c")
                .long("chars")
                .help("show char count")
                .takes_value(false)
                .conflicts_with("bytes")
        )
        .arg(
            Arg::with_name("lines")
                .short("l")
                .long("lines")
                .help("show line count")
                .takes_value(false)
        )
        .get_matches();

    let mut lines = matches.is_present("lines");
    let mut words = matches.is_present("words");
    let mut bytes = matches.is_present("bytes");
    let chars = matches.is_present("chars");

    if [lines, words, bytes, chars].iter().all(|v| v == &false ) {
        lines = true;
        words = true;
        bytes = true;
    }

    Ok(Config {
        files: matches.values_of_lossy("files").unwrap(),
        lines: lines,
        words: words,
        chars: chars,
        bytes: bytes,
    })
}

pub fn run(cfg: Config) -> MyResult<()> {
    for filename in cfg.files {
        match open(&filename) {
            Err(e) => eprintln!("{}: {}", filename, e),
            Ok(file) => {
                if let Ok(info) = count(file) {
                    println!("{}{}{}{}{}",
                        format_field(info.num_lines, cfg.lines),
                        format_field(info.num_words, cfg.words),
                        format_field(info.num_bytes, cfg.bytes),
                        format_field(info.num_chars, cfg.chars),
                        if filename == "-" {
                            "".to_string()
                        } else {
                            format!(" {}", filename)
                        });
                }
            }
        }
    }

    Ok(())
}

fn format_field(value: usize, show: bool) -> String {
    if show {
        format!("{:>8}", value)
    } else {
        "".to_string()
    }
}

fn open(filename: &str) -> MyResult<Box<dyn BufRead>> {
    match filename {
        "-" => Ok(Box::new(BufReader::new(io::stdin()))),
        _ => Ok(Box::new(BufReader::new(File::open(filename)?))),
    }
}

#[derive(Debug, PartialEq)]
struct FileInfo {
    num_lines: usize,
    num_words: usize,
    num_bytes: usize,
    num_chars: usize,
}

fn count(mut file: impl BufRead) -> MyResult<FileInfo> {
    let mut num_lines = 0;
    let mut num_words = 0;
    let mut num_bytes = 0;
    let mut num_chars = 0;

    let mut line = String::new();

    loop {
        let line_bytes = file.read_line(&mut line)?;
        if line_bytes == 0 {
            break;
        }
        num_lines += 1;
        num_words += line.split_whitespace().count();
        num_chars += line.chars().count();
        num_bytes += line_bytes;

        line.clear();
    }

    Ok(FileInfo {
        num_lines,
        num_words,
        num_bytes,
        num_chars,
    })
}