let pr_str readably t = 
  let quote s =
    let b = s |> String.length |> Buffer.create in
    let rec uc s =
      match s() with
      | Seq.Nil -> Buffer.contents b
      | Seq.Cons('"', xs) -> Buffer.add_string b "\\\""; uc xs
      | Seq.Cons('\\', xs) -> Buffer.add_string b "\\\\"; uc xs
      | Seq.Cons('\n', xs) -> Buffer.add_string b "\\n"; uc xs
      | Seq.Cons(c, xs) -> Buffer.add_char b c; uc xs
    in s |> String.to_seq |> uc |> Printf.sprintf "\"%s\""
  in
  let rec tmap_fold k d i =
    let kt = match k with
    | Types.HashKey.String(ks) -> Types.String ks
    | Types.HashKey.Keyword(ks) -> Types.Keyword ks
    in i @ [(pr kt) ^ " " ^ (pr d)]
  and pr = function
  | Types.Nil -> "nil"
  | Types.Atom(a) -> Printf.sprintf "(atom %s)" (pr !a)
  | Types.Bool(b) -> string_of_bool b
  | Types.Integer(i) -> string_of_int i 
  | Types.Keyword(s) -> s
  | Types.String(s) -> if readably then quote s else s
  | Types.Symbol(s) -> s 
  | Types.Lambda(_) -> "#<function>"
  | Types.List(l) ->
      let ls = List.map pr l in
      "(" ^ (String.concat " " ls) ^ ")" 
  | Types.Vector(l) ->
      let ls = List.map pr l in
      "[" ^ (String.concat " " ls) ^ "]" 
  | Types.Hashmap(h) ->
      let hs = Types.TMap.fold tmap_fold h [] in
      "{" ^ (String.concat " " hs) ^ "}"
  in pr t

let pr_str_r = pr_str true
let pr_str_nr = pr_str false
