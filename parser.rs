use std::fs;
#[path = "encoder.rs"]
mod encoder;

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



fn inform(message: String) {
    eprintln!("\x1b[31m[Bronze] \x1b[92m{}\x1b[0m", message.as_str());
}

fn raise(line: u8, message: String) {
    eprintln!("\x1b[31m\n[Bronze] \x1b[91mError at line {}: {}\n\x1b[0m", line, message.as_str());
    std::process::exit(1);
}

fn warn(line: u8, message: String) {
    eprintln!("\x1b[31m\n[Bronze] \x1b[93mWarning at line {}: {}\n\x1b[0m", line, message.as_str());
}


// Takes a script file and parses it into an array of binary which will be easier to process later
pub fn parse(file: &str) -> (Vec<Vec<Vec<u8>>>, Storage) {
    let file = match fs::read_to_string(file) {
        Ok(r) => {
            inform(
                format!("Read script file \"{}\" successfully", file)
            ); r
        },
        
        Err(_e) => {
            raise(
                0 as u8, 
                format!("Could not read \"{}\"", file)
            ); 
            String::from("No content")
        }
    };

    // Explicit storage
    let mut expl_storage = Storage {
        lookup: Vec::new(),
        raw: Vec::new()
    };

    let mut parsed: Vec<Vec<Vec<u8>>> = Vec::new();
    let mut line: Vec<Vec<u8>> = Vec::new();
    let mut expl_binary: Vec<u8> = Vec::new();
    let mut chunk = String::new();
    let mut chunk_type = 0;

    let mut ignore_delimiters: bool = false;
    let mut comment_ignore: bool = false;

    let mut line_started: bool = true;

    let mut line_count: u8 = 1;
    //line.push(1);

    // These are unrelated variables which are
    // referenced and changed throughout the loop
    let mut raw_start = 0;

    let mut parsed_integer = 0;
    let mut leading_zero: bool = false;
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

                chunk_type = match chunk.as_str() {
                    "str" => 0, // String (0) (declaration optional)
                    "int" => 1, // Integer (1)
                    "float" => 2, // Float (2)
                    "bool" => 3, // Boolean (3)
                    _ => 4, // Default to string (0)
                };

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
                raw_start = expl_binary.len() as i32;
                expl_binary.push(chunk_type);
                match chunk_type {
                    0 => {encoder::enc_ascii(&mut expl_binary, chunk)}, // String (ASCII)

                    // Type is an integer
                    1 => {
                        // Parse to largest possible size
                        parsed_integer = match chunk.parse::<i128>() {
                            Ok(i) => i,
                            Err(e) => {
                                raise(
                                    line_count, format!(
                                        "The given value \"{}\" cannot be interpreted as a 128-bit integer.", chunk
                                    )
                                ); 0
                            }
                        };

                        encoder::enc_int(&mut expl_binary, parsed_integer)
                    },

                    _ => {}
                }

                line.push(expl_binary);
                expl_binary = Vec::new();

                // Add indices to lookup table
                //expl_storage.lookup.push(
                //    vec![
                //        chunk_type, // Numerical representation of datatype
                //        raw_start, // Start index in raw storage
                //        expl_binary.len() as i32 - 1 // End index in raw storage
                //    ]
                //);
                
                // Assign new string to hold next chunk
                chunk = String::new();
                chunk_type = 0;
    
                // Append line to parsed if
                // the delimiter is a semicolon
                if c == 0x003B as char {
                    parsed.push(line);
                    line = Vec::new();
                    // Add line number for debugging purposes
                    //line.push(line_count);

                    line_started = true;
                }
    
                continue;
            }
        }

        chunk.push(c);
    }

    return (parsed, expl_storage);
}