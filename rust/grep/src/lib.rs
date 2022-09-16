use clap::{Arg, App};
use std::fs::{File, self};
use std::{error::Error, io::BufRead, io::BufReader};
use std::{io, mem};
use regex::{Regex, RegexBuilder};

type MyResult<T> = Result<T, Box<dyn Error>>;

#[derive(Debug)]
pub struct Config {
    pattern: Regex,
    files: Vec<String>,
    recursive: bool,
    count: bool,
    invert_match: bool,
}

pub fn get_args() -> MyResult<Config> {
    let matches = App::new("grep")
        .version("0.0.1")
        .author("lyyyuna")
        .about("rust version grep")
        .arg(
            Arg::with_name("pattern")
                .value_name("PATTERN")
                .help("search pattern")
                .required(true)
        )
        .arg(
            Arg::with_name("files")
                .value_name("FILE")
                .help("input files")
                .multiple(true)
                .default_value("-")
        )
        .arg(
            Arg::with_name("insensitive")
                .short("i")
                .long("insensitive")
                .help("case insensitive")
                .takes_value(false)
        )
        .arg(
            Arg::with_name("recursive")
                .short("r")
                .long("recursive")
                .help("recursive search")
                .takes_value(false)
        )
        .arg(
            Arg::with_name("count")
                .short("c")
                .long("count")
                .help("count occurrences")
                .takes_value(false)
        )
        .arg(
            Arg::with_name("invert")
                .short("v")
                .long("invert-match")
                .help("invert match")
                .takes_value(false)
        )
        .get_matches();
    
    let pattern = matches.value_of("pattern").unwrap();
    let pattern = RegexBuilder::new(pattern)
        .case_insensitive(matches.is_present("insensitive"))
        .build()
        .map_err(|e| format!("invalid regex pattern \"{}\"", pattern))?;
    let files = matches.values_of_lossy("files").unwrap();

    Ok(Config{
        pattern,
        files,
        recursive: matches.is_present("recursive"),
        count: matches.is_present("count"),
        invert_match: matches.is_present("invert"),
    })
}

pub fn run(config: Config) -> MyResult<()> {
    let entries = find_files(&config.files, config.recursive);

    let num_files = entries.len();

    let print = |fname: &str, val: &str| {
        if num_files > 1 {
            print!("{}:{}", fname, val)
        } else {
            print!("{}", val)
        }
    };

    for entry in entries {
        match entry {
            Err(e) => eprintln!("{}", e),
            Ok(filename) => match open(&filename) {
                Err(e) => eprintln!("{}: {}", filename, e),
                Ok(file) => {
                    match find_lines(
                        file, 
                        &config.pattern, 
                        config.invert_match) {
                            Err(e) => eprintln!("{}", e),
                            Ok(res) => {
                                if config.count {
                                    print(&filename, &format!("{}\n", res.len()));
                                } else {
                                    for line in &res {
                                        print(&filename, line);
                                    }
                                }
                            }
                        }
                }
            }
        }
    }

    Ok(())
}

fn open(filename: &str) -> MyResult<Box<dyn BufRead>> {
    match filename {
        "-" => Ok(Box::new(BufReader::new(io::stdin()))),
        _ => Ok(Box::new(BufReader::new(File::open(filename)?))),
    }
}

fn find_files(paths: &[String], recursive: bool) -> Vec<MyResult<String>> {
    let mut results = vec![];

    for path in paths {
        match path.as_str() {
            "-" => results.push(Ok(path.to_string())),
            _ => match fs::metadata(path) {
                Err(e) => results.push(Err(From::from(format!("{}: {}", path, e)))),
                Ok(metadata) => {
                    if metadata.is_dir() {
                        if recursive {
                            for ref entry in walkdir::WalkDir::new(path)
                                .into_iter()
                                .flatten()
                                .filter(|e| e.file_type().is_file()) {
                                results.push(Ok(
                                    entry.path().display().to_string()
                                ))
                            }
                        } else {
                            results.push(Err(From::from(format!("{} is a directory", path))))
                        }
                    } else if metadata.is_file() {
                        results.push(Ok(path.to_string()))
                    }
                }
            }
        }
    }

    results
}

fn find_lines<T: BufRead> (mut file: T, pattern: &Regex, invert_match: bool) -> MyResult<Vec<String>> {
    let mut matches = vec![];
    let mut line = String::new();

    loop {
        let bytes = file.read_line(&mut line)?;
        if bytes == 0 {
            break; // EOF
        }

        if pattern.is_match(&line) ^ invert_match {
            matches.push(mem::take(&mut line));
        } 

        line.clear();
    }

    Ok(matches)
}