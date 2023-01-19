use clap::{App, Arg};
use std::{
    error::Error,
    fs::File,
    io::{self, BufRead, BufReader, BufWriter, Write},
    os,
};

type MyResult<T> = Result<T, Box<dyn Error>>;

#[derive(Debug)]
pub struct Config {
    in_file: String,
    out_file: Option<String>,
    count: bool,
}

pub fn get_args() -> MyResult<Config> {
    let matches = App::new("uniq")
        .version("0.0.1")
        .author("lyyuna")
        .about("rust version of uniq")
        .arg(
            Arg::with_name("in_file")
                .value_name("IN_FILE")
                .help("input file")
                .default_value("-"),
        )
        .arg(
            Arg::with_name("out_file")
                .value_name("OUT_FILE")
                .help("output file"),
        )
        .arg(
            Arg::with_name("count")
                .short("c")
                .long("count")
                .help("show counts")
                .takes_value(false),
        )
        .get_matches();

    Ok(Config {
        in_file: matches.value_of_lossy("in_file").map(|v| v.into()).unwrap(),
        out_file: matches.value_of("out_file").map(String::from),
        count: matches.is_present("count"),
    })
}

pub fn run(cfg: Config) -> MyResult<()> {
    let mut file = open(&cfg.in_file).map_err(|e| format!("{}: {}", cfg.in_file, e))?;

    let mut line = String::new();
    let mut previous = String::new();
    let mut count = 0;

    let mut out_file: Box<dyn Write> = match &cfg.out_file {
        Some(out_name) => Box::new(File::create(out_name)?),
        None => Box::new(io::stdout()),
    };

    let mut print = |count: i32, content: &str| -> MyResult<()> {
        if count > 0 {
            if cfg.count {
                write!(out_file, "{}: {}", count, content)?;
            } else {
                write!(out_file, "{}", content)?;
            }
        };

        Ok(())
    };

    loop {
        let bytes = file.read_line(&mut line)?;
        if bytes == 0 {
            break;
        }

        if line.trim() != previous.trim() {
            print(count, &previous)?;
            previous = line.clone();
            count = 0;
        }

        count += 1;
        line.clear();
    }

    print(count, &previous)?;

    Ok(())
}

fn open(filename: &str) -> MyResult<Box<dyn BufRead>> {
    match filename {
        "-" => Ok(Box::new(BufReader::new(io::stdin()))),
        _ => Ok(Box::new(BufReader::new(File::open(filename)?))),
    }
}
