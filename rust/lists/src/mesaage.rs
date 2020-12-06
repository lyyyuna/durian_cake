#![allow(dead_code)]

pub trait Messenger {
    fn send(&self, msg: &str);
}

pub struct LimitTacker<'a, T: Messenger> {
    messenger: &'a T,
    value: usize,
    max: usize,
}

impl<'a, T> LimitTacker<'a, T>
    where T: Messenger {
    pub fn new(messenger: &'a T, max: usize) -> LimitTacker<T> {
        LimitTacker {
            messenger,
            value: 0,
            max,
        }
    }

    pub fn set_value(&mut self, value: usize) {
        self.value = value;

        let percentage_of_max = self.value as f64 / self.max as f64;

        if percentage_of_max >= 1.0 {
            self.messenger.send("error, you are over your quota!");
        } else if percentage_of_max >= 0.9 {
            self.messenger.send("0.5");
        } else if percentage_of_max >= 0.75 {
            self.messenger.send("0.75");
        }
    }
}

#[cfg(test)]
mod tests {
    use std::cell::RefCell;

    use super::*;

    struct MockMessenger {
        sent_messages: RefCell<Vec<String>>,
    }

    impl MockMessenger {
        fn new() -> MockMessenger {
            MockMessenger {
                sent_messages: RefCell::new(vec![])
            }
        }
    }

    impl Messenger for MockMessenger {
        fn send(&self, message: &str) {
            self.sent_messages.borrow_mut().push(String::from(message));
        }
    }

    #[test]
    fn it_sends_an_over_75_percent() {
        let mock_messenger = MockMessenger::new();
        let mut limit_tracker = LimitTacker::new(&mock_messenger, 100);

        limit_tracker.set_value(80);

        assert_eq!(mock_messenger.sent_messages.borrow().len(), 1);
    }
}