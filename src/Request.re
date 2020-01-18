module Status = H2.Status;

module Method = H2.Method;

module Request = {
    type t = {
        method: Method.t,
        uri: Uri.t,
        body: option(string),
        headers: list((string, string))
    }

    let body = t => t.body;
}