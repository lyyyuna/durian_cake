use assert_cmd::Command;
use predicates::prelude::predicate; 
use std::error::Error;

#[test]
fn dies_no_args() -> Result<(), Box<dyn Error>> {
    let mut cmd = Command::cargo_bin("echo")?;
    cmd.assert()
        .failure()
        .stderr(predicate::str::contains("USAGE"));
    Ok(())
}

#[test]
fn runs成功() {
    let mut cmd = Command::cargo_bin("echo").unwrap();
    cmd.arg("hello")
        .assert()
        .success();
}