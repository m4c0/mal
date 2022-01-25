let pr_str readably t = 
  let unquote s =
    s
  in
  let rec tmap_fold k d i =
    let kt = match k with
    | Types.HashKey.String(ks) -> Types.String ks
    | Types.HashKey.Keyword(ks) -> Types.Keyword ks
    in i @ [(pr kt) ^ " " ^ (pr d)]
  and pr = function
  | Types.Nil -> "nil"
  | Types.Bool(b) -> string_of_bool b
  | Types.Integer(i) -> string_of_int i 
  | Types.Keyword(s) -> s
  | Types.String(s) -> if readably then s else unquote s
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
