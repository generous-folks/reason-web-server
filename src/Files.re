
let file_exists = path => Sys.file_exists(path) && !Sys.is_directory(path);

let is_index = path => Str.string_match(Str.regexp(".*index.html$"), path, 0);

let get_base_path = switch(Array.length(Sys.argv)) {
    | 2 => Sys.argv[1];
    | _ => {
      print_endline("Missing first argument 'directory', defaulting to 'assets'");
      "assets";
    };
}

let determine_file_type = path => Magic_mime.lookup(path);

let rec try_file = (base_path: string, path: string) => {

    let path = base_path ++ path;

    switch((is_index(path), file_exists(path))) {
        | (_, true) => {
            let file = Stdlib.open_in(path);
            let length = Stdlib.in_channel_length(file);
            let content = Stdlib.really_input_string(file, length - 1);
            Stdlib.close_in(file);
            (Some(content), determine_file_type(path));
        }
        | (false, false) => try_file("", path ++ "index.html");
        | (_, false) => (None, "");
    }
}


