
let try_file = (path: string) => {

    let filepath = switch(Array.length(Sys.argv)) {
        | 2 => Sys.argv[1] ++ path;
        | _ => path;
    }

    switch(Sys.file_exists(filepath)) {
        | true => {
            let file = Stdlib.open_in(filepath);
            let length = Stdlib.in_channel_length(file);
            let content = Stdlib.really_input_string(file, length - 1);
            Stdlib.close_in(file);
            Some(content);
        }
        | false => None;
    }
}

let determine_file_type = path => Magic_mime.lookup(path);
