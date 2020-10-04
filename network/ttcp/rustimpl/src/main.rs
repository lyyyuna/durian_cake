use tokio::prelude::*;
use tokio::net::{TcpListener, TcpStream};
use tokio::stream::StreamExt;
use tokio::io::{AsyncReadExt};
use structopt::StructOpt;

#[derive(Debug, StructOpt)]
#[structopt(name = "ttcp", about = "ttcp")]
struct Opt {
    #[structopt(long)]
    server: bool,

    #[structopt(long = "number", default_value = "1000")]
    number: i32,

    #[structopt(long = "length", default_value = "4096")]
    length: i32,

    #[structopt(long = "ipport", default_value = "127.0.0.1:5555")]
    ipport: String,
}

#[tokio::main]
async fn main() {
    let opt = Opt::from_args();

    if opt.server == true {
        let mut listener = TcpListener::bind(opt.ipport).await.unwrap();
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
    } else {
        let mut stream = TcpStream::connect(opt.ipport).await.unwrap();

        // send session
        stream.write_i32(opt.number).await.unwrap();
        stream.write_i32(opt.length).await.unwrap();

        let buf: Vec<u8> = vec![1; opt.length as usize];
        // send data
        for _i in 0..opt.number {
            stream.write_i32(opt.length).await.unwrap();
            stream.write_all(&buf).await.unwrap();

            let recv_length = stream.read_i32().await.unwrap();
            if recv_length != opt.length {
                eprintln!("length not compatible")
            }
        }
    }


}
