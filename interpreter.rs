//use std::env;
mod parser;

fn main() {
    let script = parser::parse("main.bze");

    println!("{:?}", script);
}