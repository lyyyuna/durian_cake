use tokio::prelude::*;
use tokio::net::TcpListener;
use tokio::stream::StreamExt;
use tokio::io::{AsyncReadExt};

#[tokio::main]
async fn main() {
    let mut listener = TcpListener::bind("127.0.0.1:5555").await.unwrap();
    let mut incoming = listener.incoming();

    while let Some(stream) = incoming.next().await {
        match stream {
            Ok(mut stream) => {
                let session_number = stream.read_i32().await.unwrap();
                let session_length = stream.read_i32().await.unwrap();
                println!("session number {}", session_number);
                println!("session length {}", session_length);

                for _i in 0..session_number {
                    let length = stream.read_i32().await.unwrap();
                    // println!("length {}", session_length);

                    if length != session_length {
                        eprintln!("length not compatible")
                    }

                    let mut buf = vec![0; length as usize];
                    stream.read_exact(&mut buf).await.unwrap();
                    stream.write_i32(length).await.unwrap();
                }
            },
            Err(e) => {
                println!("{}", e);
            }
        }
    }
}
