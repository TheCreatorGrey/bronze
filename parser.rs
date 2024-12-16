use std::fs;

pub fn parse(file: &str) -> Vec<Vec<String>> {
    let file = fs::read_to_string(file)
        .expect("log (Could not read file);");

    let mut parsed: Vec<Vec<String>> = Vec::new();
    let mut line: Vec<String> = Vec::new();
    let mut chunk = String::new();

    let mut ignore_delimiters: bool = false;
    let mut comment_ignore: bool = false;

    let mut line_started: bool = true;

    for c in file.chars() {
        // Ignore newlines
        if c == 0x0A as char {
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

        // A semicolon represents the end of a line
        // A space represents the end of a chunk unless between parentheses
        if !ignore_delimiters {
            // Check if the char is a delimiter 
            // (semicolon or space)
            if
                c == 0x003B as char ||
                c == 0x20 as char
            {
                // Append chunk to line
                line.push(chunk);
                // Assign new string to hold next chunk
                chunk = String::new();
    
                // Append line to parsed if
                // the delimiter is a semicolon
                if c == 0x003B as char {
                    parsed.push(line);
                    line = Vec::new();

                    line_started = true;
                }
    
                continue;
            }
        }

        chunk.push(c);
    }

    return parsed;
}