use std::mem;

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

    pub fn push(&mut self, elem: i32) {
        let new_node = Box::new(Node {
            elem: elem,
            next: mem::replace(&mut self.head, None),
        });
    }
}