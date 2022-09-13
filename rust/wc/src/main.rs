fn main() {
    if let Err(err) = wc::get_args().and_then(wc::run) {
        eprintln!("{}", err);
        std::process::exit(1);
    }
}
