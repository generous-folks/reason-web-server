

let defaultAddress = "0.0.0.0";
let defaultPort = 80;


let listen: (
    ~address: string,
    ~port: int,
) => string = (~address, ~port) => {
    address ++ ":" ++ string_of_int(port);
}

print_endline(listen(~address=defaultAddress, ~port=defaultPort));
