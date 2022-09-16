use crate::Extract::*;
use clap::{App, Arg};
use csv::{StringRecord, ReaderBuilder, WriterBuilder};
use std::error::Error;
use std::fs::File;
use std::io::{self, BufRead, BufReader};
use std::num::NonZeroUsize;
use std::ops::Range;

type MyResult<T> = Result<T, Box<dyn Error>>;
type PositionList = Vec<Range<usize>>;

#[derive(Debug)]
pub enum Extract {
    Fields(PositionList),
    Bytes(PositionList),
    Chars(PositionList),
}

#[derive(Debug)]
pub struct Config {
    files: Vec<String>,
    delimiter: u8,
    extract: Extract,
}

pub fn get_args() -> MyResult<Config> {
    let matches = App::new("cut")
        .version("0.0.1")
        .about("rust version cut")
        .author("lyyyuna")
        .arg(
            Arg::with_name("files")
                .value_name("FILE")
                .help("input files")
                .multiple(true)
                .default_value("-"),
        )
        .arg(
            Arg::with_name("delimiter")
                .value_name("DELIMITER")
                .short("d")
                .long("delim")
                .help("field delimiter")
                .default_value("\t"),
        )
        .arg(
            Arg::with_name("fields")
                .value_name("FIELDS")
                .short("f")
                .long("fields")
                .help("selected fields")
                .conflicts_with_all(&["chars", "bytes"]),
        )
        .arg(
            Arg::with_name("bytes")
                .value_name("BYTES")
                .short("b")
                .long("bytes")
                .help("selected bytes")
                .conflicts_with_all(&["fields", "chars"]),
        )
        .arg(
            Arg::with_name("chars")
                .value_name("CHARS")
                .short("c")
                .long("chars")
                .help("selected characters")
                .conflicts_with_all(&["fields", "bytes"]),
        )
        .get_matches();

    let delimiter = matches.value_of("delimiter").unwrap();
    let delim_bytes = delimiter.as_bytes();
    if delim_bytes.len() != 1 {
        return Err(From::from(format!(
            "--delim \"{}\" must be single byte",
            delimiter
        )));
    }

    let fields = matches.value_of("fields").map(parse_pos).transpose()?;
    let bytes = matches.value_of("bytes").map(parse_pos).transpose()?;
    let chars = matches.value_of("chars").map(parse_pos).transpose()?;

    let extract = if let Some(field_pos) = fields {
        Fields(field_pos)
    } else if let Some(byte_pos) = bytes {
        Bytes(byte_pos)
    } else if let Some(char_pos) = chars {
        Chars(char_pos)
    } else {
        return Err(From::from("must have --fields, --bytes, or --chars"));
    };

    Ok(Config {
        files: matches.values_of_lossy("files").unwrap(),
        delimiter: *delim_bytes.first().unwrap(),
        extract,
    })
}

fn open(filename: &str) -> MyResult<Box<dyn BufRead>> {
    match filename {
        "-" => Ok(Box::new(BufReader::new(io::stdin()))),
        _ => Ok(Box::new(BufReader::new(File::open(filename)?))),
    }
}

pub fn run(config: Config) -> MyResult<()> {
    for filename in &config.files {
        match open(filename) {
            Err(err) => eprintln!("{}: {}", filename, err),
            Ok(file) => match &config.extract {
                Fields(field_pos) => {
                    let mut reader = ReaderBuilder::new()
                            .delimiter(config.delimiter)
                            .has_headers(false)
                            .from_reader(file);
                    let mut wtr = WriterBuilder::new()
                            .delimiter(config.delimiter)
                            .from_writer(io::stdout());
                    
                    for record in reader.records() {
                        let record = record?;
                        wtr.write_record(extract_fields(&record, field_pos))?;
                    }
                },
                Chars(char_pos) => {
                    for line in file.lines() {
                        println!("{}", extract_chars(&line?, char_pos));
                    }
                },
                Bytes(byte_pos) => {
                    for line in file.lines() {
                        println!("{}", extract_bytes(&line?, byte_pos));
                    }
                },
            },
        }
    }

    Ok(())
}

fn parse_pos(range: &str) -> MyResult<PositionList> {
    let range_re = regex::Regex::new(r"^(\d+)-(\d+)$").unwrap();

    range
        .split(',')
        .into_iter()
        .map(|val| {
            parse_index(val).map(|n| n..n + 1).or_else(|e| {
                range_re.captures(val).ok_or(e).and_then(|captures| {
                    let n1 = parse_index(&captures[1])?;
                    let n2 = parse_index(&captures[2])?;
                    if n1 >= n2 {
                        Err(format!(
                            "first number in range {} must be slower than second number {}",
                            n1, n2
                        ))
                    } else {
                        Ok(n1..n2 + 1)
                    }
                })
            })
        })
        .collect::<Result<_, _>>()
        .map_err(From::from)
}

fn parse_index(input: &str) -> Result<usize, String> {
    let value_error = || format!("illegal list value: \"{}\"", input);

    input
        .starts_with("+")
        .then(|| Err(value_error()))
        .unwrap_or_else(|| {
            input
                .parse::<NonZeroUsize>()
                .map(|n| usize::from(n) - 1)
                .map_err(|_| value_error())
        })
}

fn extract_chars(line: &str, char_pos: &[Range<usize>]) -> String {
    let chars: Vec<_> = line.chars().collect();
    let mut selected: Vec<char> = vec![];

    // for range in char_pos.iter().cloned() {
    //     for i in range {
    //         if let Some(val) = chars.get(i) {
    //             selected.push(*val);
    //         }
    //     }
    // }

    // for range in char_pos.iter().cloned() {
    //     selected.extend(range.filter_map(|i| chars.get(i)))
    // }

    // char_pos.iter()
    //     .cloned()
    //     .map(|range| range.filter_map(|i| chars.get(i)))
    //     .flatten()
    //     .collect()

    char_pos.iter()
        .cloned()
        .flat_map(|range| range.filter_map(|i| chars.get(i)))
        .collect()

    // selected.iter().collect()
}

fn extract_bytes(line: &str, byte_pos: &[Range<usize>]) -> String {
    let bytes = line.as_bytes();
    let selected: Vec<_> = byte_pos.iter()
        .cloned()
        .flat_map(|range| range.filter_map(|i| bytes.get(i).copied()))
        .collect();

    String::from_utf8_lossy(&selected).into_owned()
}

fn extract_fields(record: &StringRecord, field_pos: &[Range<usize>]) -> Vec<String> {
    field_pos.iter()
        .cloned()
        .flat_map(|range| range.filter_map(|i| record.get(i)))
        .map(String::from)
        .collect()
}