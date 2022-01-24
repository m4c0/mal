open Types

exception Invalid_args

let two_int_fn fn args =
  match args with 
  | (Integer(a), Integer(b)) -> Integer(fn a b)
  | _ -> raise Invalid_args

let sum = two_int_fn (fun a b -> a + b)
let sub = two_int_fn (fun a b -> a - b)
let mult = two_int_fn (fun a b -> a * b)
let div = two_int_fn (fun a b -> a / b)
