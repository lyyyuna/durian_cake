use clap::{App, Arg};
use regex::Regex;
use std::error::Error;
use walkdir::{WalkDir, DirEntry};

type MyResult<T> = Result<T, Box<dyn Error>>;

#[derive(Debug, Eq, PartialEq)]
enum EntryType {
    Dir,
    File,
    Link,
}

#[derive(Debug)]
pub struct Config {
    paths: Vec<String>,
    names: Vec<Regex>,
    entry_types: Vec<EntryType>,
}

pub fn get_args() -> MyResult<Config> {
    let matches = App::new("find")
        .version("0.0.1")
        .author("lyyyuna")
        .about("rust version of find")
        .arg(
            Arg::with_name("paths")
                .value_name("PATH")
                .help("search paths")
                .default_value(".")
                .multiple(true)
        )
        .arg(
            Arg::with_name("names")
                .value_name("NAME")
                .short("n")
                .long("name")
                .help("search names")
                .takes_value(true)
                .multiple(true)
        )
        .arg(
            Arg::with_name("types")
                .value_name("TYPE")
                .short("t")
                .long("type")
                .help("entry types")
                .possible_values(&["f", "d", "l"])
                .takes_value(true)
                .multiple(true)
        )
        .get_matches();

    let names = matches
            .values_of_lossy("names")
            .map(|vals| {
                vals.into_iter()
                    .map(|name| {
                        Regex::new(&name)
                            .map_err(|_| format!("invalid name: {}", name))
                    })
                    .collect()
            })
            .transpose()?
            .unwrap_or_default();

    println!("{:?}", names);

    let entry_types = matches
            .values_of_lossy("types")
            .map(|vals| {
                vals.iter()
                    .map(|val| match val.as_str() {
                        "d" => EntryType::Dir,
                        "f" => EntryType::File,
                        "l" => EntryType::Link,
                        _ => unreachable!("invalid type")
                    })
                    .collect()
            })
            .unwrap_or_default();

    let paths = matches.values_of_lossy("paths").unwrap();

    Ok(Config { 
        paths: paths, 
        names: names, 
        entry_types: entry_types
    })
}

pub fn run(cfg: Config) -> MyResult<()> {
    let type_filter = |entry: &DirEntry| {
        cfg.entry_types.is_empty()
            || cfg.entry_types
                .iter()
                .any(|entry_type| match entry_type {
                    EntryType::Dir => entry.file_type().is_dir(),
                    EntryType::File => entry.file_type().is_file(),
                    EntryType::Link => entry.file_type().is_symlink(),
                })
    };

    let name_filter = |entry: &DirEntry| {
        cfg.names.is_empty()
            || cfg.names.iter()
                .any(|re| re.is_match(&entry.file_name().to_string_lossy()))
    };

    for path in cfg.paths {
        let entries = WalkDir::new(path)
            .into_iter()
            .filter_map(|e| match e {
                Err(e) => {
                    eprintln!("{}", e);
                    None
                }
                Ok(entry) => Some(entry)
            })
            .filter(type_filter)
            .filter(name_filter)
            .map(|entry| entry.path().display().to_string())
            .collect::<Vec<String>>();

        println!("{}", entries.join("\n"));
    }

    Ok(())
}