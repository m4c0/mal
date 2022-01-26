module HashKey = struct
  type t =
    | Keyword of string
    | String of string
  let compare a b =
    match a with
    | String(x) -> (
        match b with
        | String(y) -> String.compare x y
        | _ -> 1
    )
    | Keyword(x) -> (
        match b with
        | Keyword(y) -> String.compare x y
        | _ -> -1
    )
end
module TMap = Map.Make(HashKey)

type t =
  | Atom of t ref
  | Bool of bool
  | Hashmap of t TMap.t
  | Integer of int
  | Keyword of string
  | Lambda of (t list -> t)
  | List of t list
  | Macro of (t list -> t)
  | Nil
  | String of string
  | Symbol of string
  | Vector of t list

exception Application_exception of t

exception Expecting_symbol
let string_of_symbol tt =
  match tt with
  | Symbol(s) -> s
  | _ -> raise Expecting_symbol

let list_of_iter tt =
  match tt with
  | List(s) -> s
  | Vector(s) -> s
  | _ -> raise Expecting_symbol

let of_list x = List x
let of_string x = String x

exception Unbalanced_hashmap
let rec assoc map = function
  | [] -> map
  | String(k) :: v :: xs -> TMap.add (HashKey.String k) v (assoc map xs)
  | Keyword(k) :: v :: xs -> TMap.add (HashKey.Keyword k) v (assoc map xs)
  | _ -> raise Unbalanced_hashmap 

