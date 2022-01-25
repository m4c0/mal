open Types

exception Invalid_args

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
let is_list = Lambda (fun args ->
  match args with
  | [List(_)] -> Bool(true)
  | _ -> Bool(false)
)
let is_empty = Lambda (fun args ->
  match args with
  | [List([])] -> Bool(true)
  | [Vector([])] -> Bool(true)
  | _ -> Bool(false)
)
let count = Lambda (fun args ->
  match args with
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
