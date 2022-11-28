use std::{error::Error, io::{BufRead, BufReader}, fs::File};
use clap::{App, Arg};
use std::io;

type MyResult<T> = Result<T, Box<dyn Error>>;

#[derive(Debug)]
pub struct Config {
    files: Vec<String>,
    number_lines: bool,
    number_nonblank_lines: bool,
}

pub fn get_args() -> MyResult<Config> {
    let matches = App::new("cat")
        .version("0.0.1")
        .author("lyyyuna")
        .about("rust version of cat")
        .arg(
            Arg::with_name("files")
                .value_name("FILE")
                .help("Input file(s)")
                .multiple(true)
                .default_value(".")
        )
        .arg(
            Arg::with_name("number")
                .short("n")
                .long("number")
                .help("Number lines")
                .takes_value(false)
                .conflicts_with("number_nonblank")
        )
        .arg(
            Arg::with_name("number_nonblank")
                .short("b")
                .long("number-nonblank")
                .help("Number nonblank lines")
                .takes_value(false)
        )
        .get_matches();

    Ok(Config { 
        files: matches.values_of_lossy("files").unwrap(),
        number_lines: matches.is_present("number"),
        number_nonblank_lines: matches.is_present("number_nonblank")    
    })
}

pub fn run(cfg: Config) -> MyResult<()> {

    for filename in cfg.files {
        match open(&filename) {
            Err(e) => eprintln!("{}: {}", filename, e),
            Ok(file) => {
                let mut cnt = 0;
                for (index, line) in file.lines().enumerate() {
                    if cfg.number_lines {
                        println!("{:>6}\t{}", index+1, line?)
                    } else if cfg.number_nonblank_lines {
                        let line_str = line?;
                        if !line_str.is_empty() {
                            cnt += 1;
                            println!("{:>6}\t{}", cnt, line_str)
                        }
                    } else {
                        println!("{}", line?)
                    }
                }
            }
        }
    }

    Ok(())
}

fn open(filename: &str) -> MyResult<Box <dyn BufRead>> {
    match filename {
        "-" => Ok(Box::new(BufReader::new(io::stdin()))),
        _ => Ok(Box::new(BufReader::new(File::open(filename)?))),
    }
}