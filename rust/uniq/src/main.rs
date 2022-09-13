fn main() {
    if let Err(err) = uniq::get_args().and_then(uniq::run) {
        eprintln!("{}", err);

        std::process::exit(1);
    }
}
