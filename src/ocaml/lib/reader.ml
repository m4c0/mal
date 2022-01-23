let int_or_sym s = 
  match int_of_string_opt s with
  | None -> Types.Symbol(s)
  | Some(i) -> Types.Integer(i)

let read_str str =
  let open Tokeniser in
  let open Seq in


  let rec read_atom ts =
    match ts() with
    | Cons(Symbol("nil"), xs) -> (Types.Nil, xs)
    | Cons(Symbol("true"), xs) -> (Types.Bool(true), xs)
    | Cons(Symbol("false"), xs) -> (Types.Bool(false), xs)
    | Cons(Symbol(x), xs) -> (int_or_sym x, xs)
    | Cons(String(x), xs) -> (Types.String(x), xs)
    | _ -> raise Tokeniser.Eof
  and read_list acc ts =
    match ts() with
    | Nil -> raise Tokeniser.Eof
    | Cons(Blank, xs) -> read_list acc xs
    | Cons(Symbol(")"), xs) -> (Types.List(acc), xs)
    | Cons(_, _) ->
        match read_form ts with (form, xs) -> read_list (acc @ [form]) xs
  and read_vector acc ts =
    match ts() with
    | Nil -> raise Tokeniser.Eof
    | Cons(Blank, xs) -> read_vector acc xs
    | Cons(Symbol("]"), xs) -> (Types.Vector(acc), xs)
    | Cons(_, _) ->
        match read_form ts with (form, xs) -> read_vector (acc @ [form]) xs
  and read_form ts =
    match ts() with
    | Nil -> raise Tokeniser.Eof
    | Cons(Blank, xs) -> read_form xs
    | Cons(Symbol("("), xs) -> read_list [] xs
    | Cons(Symbol("["), xs) -> read_vector [] xs
    | Cons(_, _) -> read_atom ts
  in

  let (form, _) = str |> tokenise |> read_form in
  form

