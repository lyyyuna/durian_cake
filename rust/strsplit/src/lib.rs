#![warn(missing_debug_implementations, rust_2018_idioms)]

pub struct StrSplit<'a> {
    remainder: &'a str,
    delimiter: &'a str,
}

impl StrSplit<'_> {
    pub fn new(haystack: &str, delimiter: &str) -> Self {
        Self {
            remainder: haystack,
            delimiter,
        }
    }
}

impl Iterator for StrSplit<'_> {
    type Item = &str;
    fn next(&mut self) -> Option<Self::Item> {
        if let Some(next_delim) = self.remainder.find(self.delimiter) {
            let until_delim = &self.remainder[..next_delim];
            self.remainder = &self.remainder[(next_delim + self.delimiter.len())..];
            Some(until_delim)
        } else if self.remainder.is_empty() {
            None
        } else {
            let rest = self.remainder;
            self.remainder = &[];
            Some(rest)
        }
    }
}

#[test]
fn it_works() {
    let haystack = "a b c d e f";
    let letters =  StrSplit::new(haystack, " ");
    assert_eq!(letters, vec!["a", "b", "c", "d", "e"].into_iter());
}