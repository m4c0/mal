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
  | Bool of bool
  | Hashmap of t TMap.t
  | Integer of int
  | Keyword of string
  | List of t list
  | Nil
  | String of string
  | Symbol of string
  | Vector of t list
