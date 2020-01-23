module Status = H2.Status;

module Method = H2.Method;


type t = {
    meth: Method.t,
    uri: Uri.t,
    body: option(string),
    headers: list((string, string))
}

let body = t => t.body;

let meth = t => t.meth;

let transform = (~body, ~uri, req) => {
    let headers = req.H2.Request.headers |> H2.Headers.to_list;
    let meth = req.meth;
    { meth, uri, body, headers };
};