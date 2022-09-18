fn main() {
    if let Err(e) = cal::get_args().and_then(cal::run) {
        eprintln!("{}", e);
        std::process::exit(1);
    }
}
