use serde::{Serialize, Deserialize};

#[derive(Debug, Serialize, Deserialize)]
struct ServerConfig {
    worker: u64,
    ignore: bool,
    auth_server: Option<String>,
}

fn main() {
    let config = ServerConfig{
        worker: 12,
        ignore: true,
        auth_server: Some("hello".to_string())
    };

    let serialized = serde_json::to_string(&config).unwrap();
    println!("json: {}", serialized);

    let deserialized: ServerConfig = serde_json::from_str(&serialized).unwrap();
    println!("1 json: {:?}", deserialized);
    println!("Hello, world!");
}
