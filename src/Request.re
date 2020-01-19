module Status = Httpaf.Status;

module Method = Httpaf.Method;


type t = {
    meth: Method.t,
    uri: Uri.t,
    body: option(string),
    headers: list((string, string))
}

let body = t => t.body;

let meth = t => t.meth;

let transform = (~body, ~uri, req) => {
    let headers = req.Httpaf.Request.headers |> Httpaf.Headers.to_list;
    let meth = req.meth;
    { meth, uri, body, headers };
};