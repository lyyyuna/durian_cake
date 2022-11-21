use assert_cmd::Command;
use predicates::prelude::*;
use std::fs;

type TestResult = Result<(), Box<dyn std::error::Error>>;

#[test]
fn dies_no_args() -> TestResult {
    let mut cmd = Command::cargo_bin("echo").unwrap();
    cmd.assert()
        .failure()
        .stderr(predicate::str::contains("USAGE"));

    Ok(())
}

#[test]
fn hello1() -> TestResult {
    let outfile = "tests/expected/hello1.txt";
    let expected = fs::read_to_string(outfile).unwrap();
    let mut cmd = Command::cargo_bin("echo").unwrap();

    cmd.arg("Hello there").assert().success().stdout(expected);

    Ok(())
}
