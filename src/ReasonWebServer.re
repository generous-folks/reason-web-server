
open Lwt.Infix;

let defaultAddress = "0.0.0.0";
let defaultPort = 80;


let read_body = reqd => {
  let (next, awake) = Lwt.wait();

  Lwt.async(() => {
    let body = reqd |> Httpaf.Reqd.request_body;
    let body_str = ref("");
    let on_eof = () => Lwt.wakeup_later(awake, Some(body_str^));
    let rec on_read = (request_data, ~off, ~len) => {
      let read = Bigstringaf.substring(~off, ~len, request_data);
      body_str := body_str^ ++ read;
      Httpaf.Body.schedule_read(body, ~on_read, ~on_eof);
    };
    Httpaf.Body.schedule_read(body, ~on_read, ~on_eof);
    Lwt.return_unit;
  });

  next;
};

let make_request_handler: (
    ~uri: Uri.t,
    ~handler: Server.handler,
    ~closer: unit => unit,
    Unix.sockaddr
) => Httpaf.Server_connection.request_handler= (~uri, ~handler, ~closer, _client, reqd) => {

    let req = reqd |> Httpaf.Reqd.request;

    let respond = (~headers=?, status, content) => {
        let httpHeaders =
            (
              switch (headers) {
              | None => []
              | Some(hs) => hs
              }
            )
            @ [("content-length", content |> String.length |> string_of_int)]
            @ [("x-powered-by", "reason-web-server")]
            |> Httpaf.Headers.of_list;
        let res = Httpaf.Response.create(status |> Httpaf.Status.of_code, ~headers=httpHeaders);
        Httpaf.Reqd.respond_with_string(reqd, res, content);
    };
    read_body(reqd)
    >|= (
       body => {
         let uri = Uri.with_path(uri, req.target);
         let req = req;
         let () = handler(Request.transform(~uri, ~body, req), respond, closer);
         ();
       }
     )|> ignore;
};

let error_handler = (_client, ~request as _=?, _err, _get) => {

  ();
};


let listen: (
    ~address: [ | `Loopback | `Any | `Of_string(string)]=?,
    ~port: int,
    ~handler: Server.handler,
) => Lwt.t(unit) = (~address=`Any, ~port, ~handler) => {

    let (forever, awaker) = Lwt.wait();

    let host =
      switch (address) {
      | `Loopback => "127.0.0.1"
      | `Any => "0.0.0.0"
      | `Of_string(str) => str
      };

    let address =
        switch (address) {
        | `Loopback => Unix.inet_addr_loopback
        | `Any => Unix.inet_addr_any
        | `Of_string(str) => Unix.inet_addr_of_string(str)
        };

    let uri = Uri.make(~scheme="http", ~host, ~port, ());

    let closer = () => Lwt.wakeup_later(awaker, ());


    let connection_handler =
      Httpaf_lwt_unix.Server.create_connection_handler(
        ~config=Httpaf.Config.default,
        ~request_handler=make_request_handler(~uri, ~handler, ~closer),
        ~error_handler,
      );


    let listening_address = Unix.(ADDR_INET(address, port));

    Lwt_io.establish_server_with_client_socket(
        listening_address,
        connection_handler,
    ) |> ignore;
    print_endline("Running on " ++ host ++ ":" ++ string_of_int(port));

    forever;
};



let handler: Server.handler = (req, reply, _kill_server) => {

    let content = Files.try_file(req.uri |> Uri.path);
    let content_type = Files.determine_file_type(req.uri |> Uri.path);

    switch((content, req.meth)) {
    | (Some(content), _) => reply(200, content, ~headers=[("content-type", content_type)]);
    | (None, _) => reply(404, "Not found");
    }
};

listen(~address=`Any, ~port=defaultPort, ~handler) |> Lwt_main.run;
