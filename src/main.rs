//use std::env;
mod parser;

// Upon parsing, instead of inserting strings of each chunk,
// each chunk should be assigned a slot in a memory/storage table
// and an integer should be inserted. The first digit corresponding
// to the datatype and the following corresponding to the index
// of the value in the vector for that datatype


// Consider voiding rust's datatypes and store everything in binary

fn main() {
    let (script, mut storage) = parser::parse("main.bze");

    //storage.strings.push(String::from("BAHH"));

    println!("{:?}", script);
    println!("{:?}", storage.lookup);
    println!("{:?}", storage.raw);
    //println!("{valtest}");
}