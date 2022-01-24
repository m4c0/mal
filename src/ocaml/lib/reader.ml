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
    | Cons(Keyword(x), xs) -> (Types.Keyword(x), xs)
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
  and read_hashmap acc ts =
    match ts() with
    | Cons(Blank, xs) -> read_hashmap acc xs
    | Cons(Symbol("}"), xs) -> (Types.Hashmap(acc), xs)
    | Cons(Keyword(key), vs) ->
        let open Types.TMap in
        let (value, xs) = read_form vs in
        let new_acc = add (Keyword key) value acc in
        read_hashmap new_acc xs
    | Cons(String(key), vs) ->
        let open Types.TMap in
        let (value, xs) = read_form vs in
        let new_acc = add (String key) value acc in
        read_hashmap new_acc xs
    | _ -> raise Tokeniser.Eof
  and read_macro sym ts =
    let tsym = Types.Symbol sym in
    let (form, xs) = read_form ts in
    let tlst = Types.List [tsym; form] in
    (tlst, xs)
  and read_withmeta ts =
    let (meta, ms) = read_form ts in
    let (data, ds) = read_form ms in
    let tsym = Types.Symbol "with-meta" in
    let tlst = Types.List [tsym; data; meta] in
    (tlst, ds)
  and read_form ts =
    match ts() with
    | Nil -> raise Tokeniser.Eof
    | Cons(Blank, xs) -> read_form xs
    | Cons(Symbol("("), xs) -> read_list [] xs
    | Cons(Symbol("["), xs) -> read_vector [] xs
    | Cons(Symbol("{"), xs) -> read_hashmap Types.TMap.empty xs
    | Cons(Symbol("'"), xs) -> read_macro "quote" xs
    | Cons(Symbol("`"), xs) -> read_macro "quasiquote" xs
    | Cons(Symbol("~"), xs) -> read_macro "unquote" xs
    | Cons(Symbol("~@"), xs) -> read_macro "splice-unquote" xs
    | Cons(Symbol("@"), xs) -> read_macro "deref" xs
    | Cons(Symbol("^"), xs) -> read_withmeta xs
    | Cons(_, _) -> read_atom ts
  in

  let (form, _) = str |> tokenise |> read_form in
  form

