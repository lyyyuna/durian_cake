fn main() {
    if let Err(e) = cat::get_args().map(|c| cat::run(c)) {
        eprintln!("{}", e);
        std::process::exit(1);
    }
}
