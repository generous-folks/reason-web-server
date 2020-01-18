

let defaultAddress: string = "0.0.0.0";

let listen:
    (~address: string) => string = (~address) => {
    address;
}

print_endline(listen(~address=defaultAddress));
