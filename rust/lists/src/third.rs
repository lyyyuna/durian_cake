#![allow(dead_code)]
use std::rc::Rc;

// List
#[derive(Debug)]
struct Node<T> {
    elem: T,
    next: Link<T>,
}

type Link<T> = Option<Rc<Node<T>>>;

#[derive(Debug)]
pub struct List<T> {
    head: Link<T>
}

impl<T> List<T> {
    pub fn new() -> Self {
        List { head: None }
    }
    
    pub fn append(&mut self, elem: T) -> List<T> {
        List {
            head: Some(Rc::new(Node {
                elem: elem,
                next: self.head.clone(),
            }))
        }
    }

    pub fn tail(&self) -> List<T> {
        List {
            head: self.head.as_ref().and_then(|node| {
                node.next.clone()
            })
        }
    }

    pub fn head(&self) -> Option<&T> {
        self.head.as_ref().map(|node| {
            &node.elem
        })
    }
}

pub struct Iter<'a, T> {
    next: Option<&'a Node<T>>
}

impl<T> List<T> {
    pub fn iter(&self) -> Iter<T> {
        Iter {
            next: self.head.as_ref().map(|node| {
                & **node
            })
        }
    }
}

impl<'a, T> Iterator for Iter<'a, T> {
    type Item = &'a T;

    fn next(&mut self) -> Option<Self::Item> {
        self.next.map(|node| {
            self.next = node.next.as_ref().map(|node| {
                & **node
            });
            &node.elem
        })
    }
}

#[cfg(test)]
mod test {
    use super::List;

    #[test]
    fn basics() {
        let mut list = List::new();

        assert_eq!(list.head(), None);

        list = list.append(1).append(2).append(3);
        assert_eq!(list.head(), Some(&3));

        list = list.tail();
        assert_eq!(list.head(), Some(&2));

        list = list.tail();
        assert_eq!(list.head(), Some(&1));

        list = list.tail();
        assert_eq!(list.head(), None);

        list = list.tail();
        assert_eq!(list.head(), None);
    }

    #[test]
    fn iter() {
        let list = List::new().append(1).append(2).append(3);

        let mut iter = list.iter();
        assert_eq!(iter.next(), Some(&3));
        assert_eq!(iter.next(), Some(&2));
        assert_eq!(iter.next(), Some(&1));
    }
}