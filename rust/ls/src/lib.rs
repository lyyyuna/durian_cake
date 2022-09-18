mod owner;

use chrono::{DateTime, Local};
use clap::{Arg, App};
use tabular::{Table, Row};
use std::{error::Error, path::PathBuf, fs, os::unix::prelude::MetadataExt};
use owner::Owner;
use users::{get_user_by_uid, get_group_by_gid};

type MyResult<T> = Result<T, Box<dyn Error>>;

#[derive(Debug)]
pub struct Config {
    paths: Vec<String>,
    long: bool,
    show_hidden: bool,
}

pub fn get_args() -> MyResult<Config> {
    let matches = App::new("ls")
            .version("0.0.1")
            .author("lyyyuna")
            .about("rust version of ls")
            .arg(
                Arg::with_name("paths")
                    .value_name("PATH")
                    .help("files or directories")
                    .default_value(".")
                    .multiple(true)
            )
            .arg(
                Arg::with_name("long")
                    .takes_value(false)
                    .help("long listing")
                    .short("l")
                    .long("long")
            )
            .arg(
                Arg::with_name("all")
                .takes_value(false)
                    .help("show all files")
                    .short("a")
                    .long("all")
            )
            .get_matches();

    Ok(Config{ 
        paths: matches.values_of_lossy("paths").unwrap(),
        long: matches.is_present("long"),
        show_hidden: matches.is_present("all"),
    })
}

pub fn run(config: Config) -> MyResult<()> {
    let paths = find_files(&config.paths, config.show_hidden)?;

    if config.long {
        println!("{}", format_output(&paths)?);
    } else {
        for path in paths {
            println!("{}", path.display());
        }
    }

    Ok(())
}

fn find_files(paths: &[String], show_hidden: bool) -> MyResult<Vec<PathBuf>> {
    let mut results = vec![];

    for name in paths {
        match fs::metadata(name) {
            Err(e) => eprintln!("{}: {}", name, e),
            Ok(meta) => {
                if meta.is_dir() {
                    for entry in fs::read_dir(name)? {
                        let entry = entry?;
                        let path = entry.path();
                        let is_hidden = path.file_name()
                            .map_or(false, |filename| {
                                filename.to_string_lossy().starts_with(".")
                            });

                        if !is_hidden || show_hidden {
                            results.push(entry.path());
                        }
                    }
                } else {
                    results.push(name.into())
                }
            },
        }
    }

    Ok(results)
}

fn format_output(paths: &[PathBuf]) -> MyResult<String> {
    let fmt = "{:<}{:<}  {:>}  {:<}  {:<}  {:>}  {:<}  {:<}";
    let mut table = Table::new(fmt);

    for path in paths {
        let metadata = fs::metadata(path)?;

        let uid = metadata.uid();
        let gid = metadata.gid();

        let user = get_user_by_uid(uid)
                .map(|u| u.name().to_string_lossy().into_owned())
                .unwrap_or_else(|| uid.to_string());
        let group = get_group_by_gid(gid)
                .map(|g| g.name().to_string_lossy().into_owned())
                .unwrap_or_else(|| gid.to_string());

        let file_type = if metadata.is_dir() { "d" } else { "-" };

        let perms = format_mode(metadata.mode());
        let modified: DateTime<Local> = DateTime::from(metadata.modified()?);

        table.add_row(
            Row::new()
                .with_cell(file_type)
                .with_cell(perms)
                .with_cell(metadata.nlink())
                .with_cell(user)
                .with_cell(group)
                .with_cell(metadata.len())
                .with_cell(modified.format("%b %d %y %H:%M"))
                .with_cell(path.display())
        );
    }

    Ok(format!("{}", table))
}

fn format_mode(mode: u32) -> String {
    format!("{}{}{}",
        mk_triple(mode, Owner::User),
        mk_triple(mode, Owner::Group),
        mk_triple(mode, Owner::Other),
    )
}

pub fn mk_triple(mode: u32, owner: Owner) -> String {
    let [read, write, execute] = owner.masks();

    format!("{}{}{}",
        if mode & read == 0 {"-"} else {"r"},
        if mode & write == 0 {"-"} else {"w"},
        if mode & execute == 0 {"-"} else {"x"},
    )
}