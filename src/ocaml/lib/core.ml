open Types

exception Invalid_args
exception Out_of_bounds
exception Unreadable_file

let two_int_fn fn =
  let l args =
    match args with 
    | [Integer(a); Integer(b)] -> Integer(fn a b)
    | _ -> raise Invalid_args
  in Lambda(l)

let cmp_fn fn =
  let l args =
    match args with 
    | Integer(a) :: Integer(b) :: _ -> Bool(fn a b)
    | _ -> raise Invalid_args
  in Lambda(l)

let sum = two_int_fn (fun a b -> a + b)
let sub = two_int_fn (fun a b -> a - b)
let mult = two_int_fn (fun a b -> a * b)
let div = two_int_fn (fun a b -> a / b)

let pr_str = Lambda (fun args -> args |> List.map Printer.pr_str_r |> String.concat " " |> Types.of_string)
let str = Lambda (fun args -> args |> List.map Printer.pr_str_nr |> String.concat "" |> Types.of_string)
let prn = Lambda (fun args -> args |> List.map Printer.pr_str_r |> String.concat " " |> print_endline; Nil)
let println = Lambda (fun args -> args |> List.map Printer.pr_str_nr |> String.concat " " |> print_endline; Nil)
let list_ = Lambda (fun args -> List(args))
let is_list = Lambda (function
  | [List(_)] -> Bool(true)
  | _ -> Bool(false)
)
let is_empty = Lambda (function
  | [List([])] -> Bool(true)
  | [Vector([])] -> Bool(true)
  | _ -> Bool(false)
)
let count = Lambda (function
  | [List(l)] -> Integer(List.length l)
  | [Vector(l)] -> Integer(List.length l)
  | _ -> Integer(0)
)

let equal = Lambda (fun args ->
  let rec shallow_equals args =
    match args with
    | List(a) :: Vector(b) :: _ -> deep_equals a b
    | Vector(a) :: List(b) :: _ -> deep_equals a b
    | a :: b :: _ -> a = b
    | _ -> false
  and rec_not_equals (a, b) = shallow_equals [a; b] |> not
  and deep_equals a b =
    try List.combine a b |> List.find_opt rec_not_equals |> Option.is_none
    with Invalid_argument(_) -> false
  in
  Bool(shallow_equals args)
)
let lt = cmp_fn (fun a b -> a < b)
let lte = cmp_fn (fun a b -> a <= b)
let gt = cmp_fn (fun a b -> a > b)
let gte = cmp_fn (fun a b -> a >= b)

let read_string = Lambda (function
  | [String(a)] -> Reader.read_str a
  | _ -> raise Invalid_args
)
let slurp = Lambda (fun args ->
  let read_file f = 
    try 
      let ch = open_in_bin f in
      let s = really_input_string ch (in_channel_length ch) in
      close_in ch;
      s
    with _ -> raise Unreadable_file
  in
  match args with
  | [String(f)] -> String(read_file f)
  | _ -> raise Invalid_args
)

let atom = Lambda (function
  | [a] -> Atom(ref a)
  | _ -> raise Invalid_args
)
let is_atom = Lambda (fun args ->
  match args with
  | [Atom(_)] -> Bool(true)
  | _ -> Bool(false)
)
let deref = Lambda (function
  | [Atom(x)] -> !x
  | _ -> raise Invalid_args
)
let reset = Lambda (function
  | [Atom(x); v] -> x := v; v
  | _ -> raise Invalid_args
)
let swap = Lambda (function
  | Atom(x) :: Lambda(fn) :: a -> x := (fn (!x :: a)); !x
  | _ -> raise Invalid_args
)

let cons = Lambda (function
  | [x; List(l)] -> List(x :: l)
  | [x; Vector(l)] -> List(x :: l)
  | _ -> raise Invalid_args
)
let concat = Lambda (fun args ->
  args |> List.map Types.list_of_iter |> List.concat |> Types.of_list
)

let vec = Lambda (function
  | [List(x)] -> Vector(x)
  | [Vector(_) as x] -> x
  | _ -> raise Invalid_args
)

let nth = Lambda (function
  | [(List(l) | Vector(l)); Integer(i)] -> (
      try List.nth l i
      with _ -> raise Out_of_bounds
  )
  | _ -> raise Invalid_args
)
let first = Lambda (function
  | [(List([]) | Vector([]))] -> Nil 
  | [(List(x :: _) | Vector(x :: _))] -> x
  | [Nil] -> Nil
  | _ -> raise Invalid_args
)
let rest = Lambda (function
  | [(List([]) | Vector([]))] -> List([])
  | [(List(_ :: xs) | Vector(_ :: xs))] -> List(xs)
  | [Nil] -> List([])
  | _ -> raise Invalid_args
)

let throw = Lambda (function
  | [x] -> raise (Types.Application_exception x)
  | _ -> raise Invalid_args
)

let apply = Lambda (fun args ->
  let rec merge_args = function
    | [List(l) | Vector(l)] -> l
    | x :: xs -> x :: merge_args xs
    | _ -> raise Invalid_args
  in
  match args with
  | Lambda(fn) :: xs -> xs |> merge_args |> fn 
  | _ -> raise Invalid_args
)
let map = Lambda (function
  | [Lambda(fn); (List(l) | Vector(l))] -> List(List.map (fun ls -> fn [ls]) l)
  | _ -> raise Invalid_args
)

let is_nil = Lambda (function [Nil] -> Bool(true) | _ -> Bool(false))
let is_true = Lambda (function [Bool(true)] -> Bool(true) | _ -> Bool(false))
let is_false = Lambda (function [Bool(false)] -> Bool(true) | _ -> Bool(false))
let is_symbol = Lambda (function [Symbol(_)] -> Bool(true) | _ -> Bool(false))
let is_keyword = Lambda (function [Keyword(_)] -> Bool(true) | _ -> Bool(false))
let is_vector = Lambda (function [Vector(_)] -> Bool(true) | _ -> Bool(false))
let is_map = Lambda (function [Hashmap(_)] -> Bool(true) | _ -> Bool(false))

let is_sequential = Lambda (function
  | [List(_)] -> Bool(true)
  | [Vector(_)] -> Bool(true)
  | _ -> Bool(false))

let vector = Lambda (fun args -> Vector(args))
let symbol = Lambda(function [String(x)] -> Symbol(x) | _ -> raise Invalid_args)
let keyword = Lambda(function
  | [String(x)] -> Keyword(x) 
  | [Keyword(_) as x] -> x
  | _ -> raise Invalid_args
)

let hashmap = Lambda(fun args -> Hashmap(Types.assoc TMap.empty args))
let assoc = Lambda(function
  | Hashmap(m) :: xs -> Hashmap(Types.assoc m xs)
  | _ -> raise Invalid_args
)
let dissoc = Lambda(function
  | Hashmap(m) :: xs -> 
      let rec fn m = function
        | [] -> m
        | Keyword(x) :: xx -> fn (TMap.remove (Keyword x) m) xx
        | String(x) :: xx -> fn (TMap.remove (String x) m) xx
        | _ :: xx -> fn m xx
      in Hashmap(fn m xs)
  | _ -> raise Invalid_args
)

let map_find_opt = function
  | [Hashmap(m); String(s)] -> TMap.find_opt (String s) m
  | [Hashmap(m); Keyword(s)] -> TMap.find_opt (Keyword s) m
  | [Nil; _] -> None
  | _ -> raise Invalid_args

let get = Lambda(fun args ->
  match map_find_opt args with
  | None -> Nil
  | Some(x) -> x
)
let contains = Lambda(fun args ->
  match map_find_opt args with
  | None -> Bool(false)
  | Some(_) -> Bool(true)
)

let keys = Lambda(function
  | [Hashmap(x)] ->
      let f (k, _) = match k with
      | HashKey.Keyword(k) -> Types.Keyword(k)
      | HashKey.String(s) -> Types.String(s)
      in TMap.bindings x |> List.map f |> Types.of_list
  | _ -> raise Invalid_args
)
let vals = Lambda(function
  | [Hashmap(x)] -> let f (_, v) = v in TMap.bindings x |> List.map f |> Types.of_list
  | _ -> raise Invalid_args
)
