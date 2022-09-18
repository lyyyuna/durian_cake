use clap::{App, Arg};
use std::{error::Error, str::FromStr};
use chrono::NaiveDate;

type MyResult<T> = Result<T, Box<dyn Error>>;

#[derive(Debug)]
pub struct Config {
    month: Option<u32>,
    year: i32,
    today: NaiveDate,
}

pub fn get_args() -> MyResult<Config> {
    let matches = App::new("cal")
            .version("0.0.1")
            .author("lyyyuna")
            .about("rust version of cal")
            .get_matches();
    
    unimplemented!();
}

pub fn run(config: Config) -> MyResult<()> {
    Ok(())
}

fn parse_int<T: FromStr>(val: &str) -> MyResult<T> {
    val.parse()
        .map_err(|_| format!("invalid integer \"{}\"", val)
        .into())
}

fn parse_year(year: &str) -> MyResult<i32> {
    parse_int(year)
        .and_then(|num| {
            if (1..=9999).contains(&num) {
                Ok(num)
            } else {
                Err(format!("{} not in range 1-9999", year).into())
            }
        })
}

const MONTH_NAMES: [&str; 12] = [
    "January",
    "February",
    "March",
    "April",
    "May",
    "June",
    "July",
    "August",
    "September",
    "October",
    "November",
    "December",
];

fn parse_month(month: &str) -> MyResult<u32> {

    unimplemented!();
}