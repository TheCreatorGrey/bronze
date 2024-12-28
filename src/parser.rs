use std::fs;

pub struct Storage {
    // The lookup table:
    // A table which stores vectors of three i32s
    // in the following format:
    // [
    //      Numerical representation of a datatype (0-4),
    //      Start index in raw table of data,
    //      End index in raw table of data
    // ]
    pub lookup: Vec<Vec<i32>>,

    // Stores any referenced value in the form
    // of binary as unsigned 8-bit integers.
    // When a value is stored, it will be converted
    // to binary and added here byte by byte. The
    // start and end index of this chunk of data
    // will then be appended to the lookup table.
    // This data will be decoded if it needs to
    // be used.
    pub raw: Vec<u8>
}



fn raise(line: i32, message: &str) {
    eprintln!("\x1b[31m\n[Bronze] \x1b[91mError at line {}: {}\n\x1b[0m", line, message);
    std::process::exit(1);
}

fn warn(line: i32, message: &str) {
    eprintln!("\x1b[31m\n[Bronze] \x1b[93mWarning at line {}: {}\n\x1b[0m", line, message);
}


// Takes a file name and returns a refined 2D vector which
// will be easier and faster to execute later.
pub fn parse(file: &str) -> (Vec<Vec<i32>>, Storage) {
    let file = fs::read_to_string(file)
        .expect("log (Could not read file);");

    let mut this_storage = Storage {
        lookup: Vec::new(),
        raw: Vec::new()
    };

    let mut parsed: Vec<Vec<i32>> = Vec::new();
    let mut line: Vec<i32> = Vec::new();
    let mut chunk = String::new();
    let mut chunk_type = 0;

    let mut ignore_delimiters: bool = false;
    let mut comment_ignore: bool = false;

    let mut line_started: bool = true;

    let mut line_count = 1;
    line.push(1);

    let charmap = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";

    // These are unrelated variables which are
    // referenced and changed throughout the loop
    let mut raw_start = 0;

    let mut parsed_integer = 0;
    // ==========================================

    for c in file.chars() {
        // Ignore newlines
        if c == 0x0A as char {
            line_count += 1;
            continue;
        }

        // Toggle comment_ignore if ` is present
        if c == 0x0060 as char {
            comment_ignore = !comment_ignore;
            continue;
        }
        if comment_ignore {
            continue;
        }

        // Enable/disable ignore_delimiters if parentheses are present
        if c == 0x028 as char {
            ignore_delimiters = true;
            continue;
        } else if c == 0x029 as char {
            ignore_delimiters = false;
            continue;
        }
        
        // Ignore whitespaces at the beginning of a line
        if c.is_whitespace() {
            if line_started {
                continue;
            }
        } else {
            line_started = false;
        }


        if !ignore_delimiters {
            // Colons separate a type declaration from
            // the actual value. The syntax of a given
            // value to a command should be written as
            // the type followed by a colon and then the
            // value (e.g. int:4). If there is no type
            // declaration, the value is interpreted
            // as a string by default.
            if c == 0x003A as char {
                // Since the "chunk" variable is built up
                // character by character, if a colon is found
                // the chunk variable must contain the type name.

                match chunk.as_str() {
                    "str" => { chunk_type = 0 }, // String (0) (declaration optional)
                    "int8" => { chunk_type = 1 }, // 8-bit integer (1)
                    "int16" => { chunk_type = 2 }, // 16-bit integer (2)
                    "int32" => { chunk_type = 3 }, // 32-bit integer (3)
                    "int64" => { chunk_type = 4 }, // 64-bit integer (4)
                    "int128" => { chunk_type = 5 }, // Gigantic 128-bit integer (5)
                    "float" => { chunk_type = 6 }, // Float (6)
                    "bool" => { chunk_type = 7 }, // Boolean (7)
                    _ => { chunk_type = 0 }, // Default to string (0)
                }

                // Since the chunk is built char by char, Clearing the chunk
                // at this point will make it so that when the delimiter code
                // below is triggered, the chunk will not contain the declaration
                // but the raw value only.
                chunk = String::new();

                continue;
            }

            // Delimiters are spaces and semicolons
            // A semicolon represents the end of a line
            // A space represents the end of a chunk unless between parentheses
            // This checks if the char is one of these delimiters: 
            if
                c == 0x003B as char || // semicolon
                c == 0x20 as char // space
            {
                // If a delimiter has been found, it means
                // the current chunk of the line should be processed
                // and added to the line vector.

                println!("{chunk_type} {chunk}");

                // Add value to storage
                raw_start = this_storage.raw.len() as i32;
                match chunk_type {
                    0 => { // Type is a string
                        // Iterate each char in the string
                        for s in chunk.chars() {
                            // Push byte of char to raw storage
                            this_storage.raw.push(
                                match charmap.find(s) {
                                    Some(value) => {
                                        value as u8
                                    }

                                    None => 0
                                }
                            )
                        }
                    },

                    // Type is in the integer range of types (1-5, 8-bit through 128-bit)
                    1..5 => {
                        // Parse to largest possible size
                        parsed_integer = match chunk.parse::<u128>() {
                            Ok(i) => i,
                            Err(e) => {
                                raise(line_count, format!(
                                    "Could not interpret \"{}\" as an integer. The given value is either too large or cannot be represented by an integer.", chunk
                                ).as_str());
                                0
                            }
                        };

                        parsed_integer = match chunk_type {
                            1 => parsed_integer << 120, // Shift down to 8
                            2 => parsed_integer << 112, // Shift down to 16
                            3 => parsed_integer << 96, // Shift down to 32
                            4 => parsed_integer << 64, // Shift down to 64
                            5 => parsed_integer, // Already 128 bits (don't shift)
                            _ => parsed_integer
                        };

                        for (count, b) in parsed_integer.to_be_bytes().iter().enumerate() {
                            // Cut off unused bytes
                            match chunk_type {
                                1 => {if 1 < count {break}}, // Cut off at 1 byte (8 bits)
                                2 => {if 2 < count {break}}, // Cut off at 2 bytes (16 bits)
                                3 => {if 4 < count {break}}, // Cut off at 4 bytes (32 bits)
                                4 => {if 8 < count {break}}, // Cut off at 8 bytes (64 bits)
                                5 => {if 16 < count {break}}, // Cut off at 16 bytes (128 bits)
                                _ => {}
                            };

                            // Push byte chunk to storage
                            this_storage.raw.push(*b)
                        }
                    },

                    _ => {}
                }

                // Add indices to lookup table
                this_storage.lookup.push(
                    vec![
                        chunk_type, // Numerical representation of datatype
                        raw_start, // Start index in raw storage
                        this_storage.raw.len() as i32 - 1 // End index in raw storage
                    ]
                );
                
                // Assign new string to hold next chunk
                chunk = String::new();
                chunk_type = 0;
    
                // Append line to parsed if
                // the delimiter is a semicolon
                if c == 0x003B as char {
                    parsed.push(line);
                    line = Vec::new();
                    // Add line number for debugging purposes
                    line.push(line_count);

                    line_started = true;
                }
    
                continue;
            }
        }

        chunk.push(c);
    }

    return (parsed, this_storage);
}