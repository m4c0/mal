exception Eof

type token = 
  | Blank
  | String of string
  | Symbol of string

let rec take_comment s =
  let open Seq in
  match s() with
  | Nil -> (Blank, s)
  | Cons('\n', xs) -> (Blank, xs)
  | Cons(_, xs) -> take_comment xs 

let rec take_spaces s =
  let open Seq in
  match s() with
  | Cons((' ' | ',' | '\t' | '\r' | '\n'), xs) -> take_spaces xs
  | _ -> (Blank, s)

let rec take_string acc s =
  let open Seq in
  match s() with
  | Nil -> raise Eof
  | Cons('\n', _) -> raise Eof
  | Cons('"', xs) -> (String(acc ^ "\""), xs)
  | Cons('\\', xs) -> (
      match xs() with
      | Nil -> raise Eof
      | Cons(x, xx) -> take_string (acc ^ "\\" ^ (String.make 1 x)) xx
  )
  | Cons(x, xs) -> take_string (acc ^ (String.make 1 x)) xs

let rec take_symbol acc s =
  let open Seq in
  match s() with
  | Nil
  | Cons((' ' | ',' | '\t' | '\r' | '\n'), _)
  | Cons(('[' | ']' | '{' | '}' | '(' | ')' | '\'' | '`' | '^'), _) -> (Symbol(acc), s)
  | Cons(x, xs) -> take_symbol (acc ^ (String.make 1 x)) xs

let take_unquote s =
  let open Seq in
  match s() with
  | Cons('@', xs) -> (Symbol("~@"), xs)
  | _ -> (Symbol("~"), s)

let token_fn s =
  let open Seq in
  match s() with
  | Nil -> None
  | Cons(';', xs) -> Some(take_comment xs)
  | Cons('~', xs) -> Some(take_unquote xs)
  | Cons('"', xs) -> Some(take_string "\"" xs)
  | Cons((' ' | ',' | '\t' | '\r' | '\n'), xs) -> Some(take_spaces xs)
  | Cons(('[' | ']' | '{' | '}' | '(' | ')' | '\'' | '`' | '^') as x, xs)
    -> Some(Symbol(String.make 1 x), xs)
  | Cons(x, xs) -> Some(take_symbol (String.make 1 x) xs)

let tokenise str =
  str
  |> String.to_seq
  |> Seq.unfold token_fn
