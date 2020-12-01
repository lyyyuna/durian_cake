// List
#[derive(Debug)]
struct Node {
    elem: i32,
    next: Link,
}

type Link = Option<Box<Node>>;

#[derive(Debug)]
struct List {
    head: Link
}

impl List {
    pub fn new() -> Self {
        List { head: None }
    }
}