use clap::{App, Arg};
use regex::Regex;
use walkdir::DirEntry;
use std::{collections::btree_map::Entry, error::Error, fs::File};

type MyResult<T> = Result<T, Box<dyn Error>>;

#[derive(Debug, PartialEq, Eq)]
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
        .author("lyyyuna")
        .about("rust version find")
        .version("0.0.1")
        .arg(
            Arg::with_name("paths")
                .value_name("PATH")
                .help("search paths")
                .default_value(".")
                .multiple(true),
        )
        .arg(
            Arg::with_name("names")
                .value_name("NAME")
                .short("n")
                .long("name")
                .help("Name")
                .takes_value(true)
                .multiple(true),
        )
        .arg(
            Arg::with_name("types")
                .value_name("TYPE")
                .short("t")
                .long("type")
                .help("entry type")
                .possible_values(&["f", "d", "l"])
                .takes_value(true)
                .multiple(true),
        )
        .get_matches();

    let names = matches
        .values_of_lossy("names")
        .map(|vals| {
            vals.into_iter()
                .map(|name| Regex::new(&name).map_err(|_| format!("invalid name \"{}\"", name)))
                .collect::<Result<Vec<_>, _>>()
        })
        .transpose()?
        .unwrap_or_default();

    let entry_types = matches
        .values_of_lossy("types")
        .map(|vals| {
            vals.iter()
                .map(|val| match val.as_str() {
                    "d" => EntryType::Dir,
                    "f" => EntryType::File,
                    "l" => EntryType::Link,
                    _ => unreachable!("invalid type"),
                })
                .collect()
        })
        .unwrap_or_default();

    Ok(Config {
        paths: matches.values_of_lossy("paths").unwrap(),
        names,
        entry_types,
    })
}

pub fn run(config: Config) -> MyResult<()> {
    for path in config.paths {
        for entry in walkdir::WalkDir::new(path) {
            match entry {
                Err(e) => eprintln!("{}", e),
                Ok(entry) => {
                    if config.entry_types.is_empty() ||
                        config.entry_types.iter().any(|entry_type| {
                            match entry_type {
                                EntryType::Link => entry.file_type().is_symlink(),
                                EntryType::Dir => entry.file_type().is_dir(),
                                EntryType::File => entry.file_type().is_file(),
                            }
                        }) {
                        println!("{}", entry.path().display())
                    }
                }
            }
        }
    }

    Ok(())
}

pub fn run2(config: Config) -> MyResult<()> {
    let type_filter = |entry: &DirEntry| {
        config.entry_types.is_empty() ||
            config.entry_types
                .iter()
                .any(|entry_type| {
                    match entry_type {
                        EntryType::Link => entry.file_type().is_symlink(),
                        EntryType::Dir => entry.file_type().is_dir(),
                        EntryType::File => entry.file_type().is_file(),
                    }
                })
    };

    let name_filter = |entry: &DirEntry| {
        config.names.is_empty() ||
            config.names
                .iter()
                .any(|re| 
                    re.is_match(&entry.file_name().to_string_lossy())
                )
    };

    for path in config.paths.iter() {
        let entries = walkdir::WalkDir::new(path)
            .into_iter()
            .filter_map(|e| match e {
                Err(e) => {
                    eprintln!("{}", e);
                    None
                }
                Ok(entry) => {
                    Some(entry)
                }
            })
            .filter(type_filter)
            .filter(name_filter)
            .map(|entry| entry.path().display().to_string())
            .collect::<Vec<_>>();

        println!("{}", entries.join("\n"));
    }

    Ok(())

}
