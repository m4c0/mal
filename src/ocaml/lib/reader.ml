let read_str str =
  let open Tokeniser in
  let open Seq in

  let rec read_atom ts =
    match ts() with
    | Cons(Symbol(x), xs) -> (Types.Symbol(x), xs)
    | Cons(String(x), xs) -> (Types.Symbol(x), xs)
    | _ -> raise Tokeniser.Eof
  and read_list acc ts =
    match ts() with
    | Nil -> raise Tokeniser.Eof
    | Cons(Blank, xs) -> read_list acc xs
    | Cons(Symbol(")"), xs) -> (Types.List(acc), xs)
    | Cons(_, _) ->
        match read_form ts with (form, xs) -> read_list (acc @ [form]) xs
  and read_form ts =
    match ts() with
    | Nil -> raise Tokeniser.Eof
    | Cons(Blank, xs) -> read_form xs
    | Cons(Symbol("("), xs) -> read_list [] xs
    | Cons(_, _) -> read_atom ts
  in

  str |> tokenise |> read_form

