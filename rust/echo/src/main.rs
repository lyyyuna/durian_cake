use clap::{App, Arg};

fn main() {
    let _matches = App::new("echo")
        .version("0.1.0")
        .author("lyyyuna")
        .about("rust echo")
        .arg(
            Arg::with_name("text")
            .value_name("TEXT")
            .help("input text")
            .required(true)
            .min_values(1)
        )
        .arg(
            Arg::with_name("omit_newline")
            .short("n")
            .help("do not print newline")
            .takes_value(false)
        )
        .get_matches();
    

    let text = _matches.values_of_lossy("text").unwrap();
    let omit_newline = _matches.is_present("omit_newline");

    let mut ending = "\n";
    if omit_newline {
        ending = "";
    }

    print!("{}{}", text.join(" "), ending)
}
