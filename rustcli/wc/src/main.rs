fn main() {
    if let Err(e) = wc::get_args().and_then(wc::run) {
        eprintln!("{}", e)
    }
}
