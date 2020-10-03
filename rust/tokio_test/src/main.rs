use tokio::net::TcpListener;
use tokio::prelude::*;

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    let t1 = tokio::spawn(async {
        loop {
            println!("hello");
            tokio::time::delay_for(tokio::time::Duration::from_secs(1)).await;
        }

    });

    loop {
        tokio::time::delay_for(tokio::time::Duration::from_secs(1)).await;
    }
}