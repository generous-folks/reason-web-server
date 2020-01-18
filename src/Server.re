
module Server = {
  type replier = (~headers: list((string, string))=?, int, string) => unit;

  type closer = unit => unit;

  type handler = (Request.t, replier, closer) => unit;
};