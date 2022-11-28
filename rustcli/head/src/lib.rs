use clap::{App, Arg};
use std::{error::Error, io::{BufRead, BufReader, self}, fs::File};
use std::io::Read;

type MyResult<T> = Result<T, Box<dyn Error>>;

#[derive(Debug)]
pub struct Config {
    files: Vec<String>,
    lines: usize,
    bytes: Option<usize>
}

pub fn get_args() -> MyResult<Config> {
    let matches = App::new("head")
        .version("0.0.1")
        .about("rust version head")
        .author("lyy")
        .arg(
            Arg::with_name("lines")
                .short("n")
                .long("lines")
                .value_name("LINES")
                .help("number of lines")
                .default_value("10")
        )
        .arg(
            Arg::with_name("bytes")
                .short("b")
                .long("bytes")
                .takes_value(true)
                .conflicts_with("lines")
                .help("number of bytes")
        )
        .arg(
            Arg::with_name("files")
                .value_name("FILES")
                .help("input files")
                .multiple(true)
                .default_value("-")
        )
        .get_matches();

    let lines = matches
        .value_of("lines")
        .map(parse_positive_int)
        .transpose()
        .map_err(|e| format!("illegal line count -- {}", e))?;

    let bytes = matches
        .value_of("bytes")
        .map(parse_positive_int)
        .transpose()
        .map_err(|e| format!("illegal bytes count -- {}", e))?;

    Ok(Config { 
        files: matches.values_of_lossy("files").unwrap(), 
        lines: lines.unwrap(), 
        bytes
    })
}

pub fn run(cfg: Config) -> MyResult<()> {
    let num_files = cfg.files.len();

    for (file_num, filename) in cfg.files.iter().enumerate() {
        match open(&filename) {
            Err(e) => eprintln!("{}: {}", filename, e),
            Ok(mut file) => {
                if num_files > 1 {
                    println!("{}==> {} <==", if file_num > 0 { "\n" } else { "" }, filename);
                }

                if let Some(num_bytes) = cfg.bytes {
                    let mut handle = file.take(num_bytes as u64);
                    let mut buffer = vec![0; num_bytes];
                    let bytes_read = handle.read(&mut buffer)?;

                    print!("{}", String::from_utf8_lossy(&buffer[..bytes_read]));
                } else {
                    let mut line = String::new();
                    for _ in 0..cfg.lines {
                        let bytes = file.read_line(&mut line)?;
                        if bytes == 0 {
                            break;
                        }

                        print!("{}", line);
                        line.clear();
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
        _ => Ok(Box::new(BufReader::new(File::open(filename)?)))
    }
}

fn parse_positive_int(val: &str) -> MyResult<usize> {
    match val.parse::<usize>() {
        Ok(n) if n > 0 => Ok(n),
        _ => Err(val.into())//Err(From::from(val))
    }
}

#[test]
fn test_parse_positive_int() {
    let res = parse_positive_int("3");
    assert!(res.is_ok());
    assert_eq!(res.unwrap(), 3);

    let res = parse_positive_int("hoo");
    assert!(res.is_err());
    assert_eq!(res.unwrap_err().to_string(), "hoo".to_string());

    let res = parse_positive_int("0");
    assert!(res.is_err());
    assert_eq!(res.unwrap_err().to_string(), "0".to_string());
}