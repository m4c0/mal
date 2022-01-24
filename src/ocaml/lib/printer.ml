let rec pr_str t =
  let open Types in

  let tmap_fold k d i =
    let kt = match k with
    | HashKey.String(ks) -> Types.String ks
    | HashKey.Keyword(ks) -> Types.Keyword ks
    in i @ [(pr_str kt) ^ " " ^ (pr_str d)]
  in

  match t with
  | Nil -> "nil"
  | Bool(b) -> string_of_bool b
  | Integer(i) -> string_of_int i 
  | Keyword(s) -> s
  | String(s) -> s 
  | Symbol(s) -> s 
  | List(l) ->
      let ls = List.map pr_str l in
      "(" ^ (String.concat " " ls) ^ ")" 
  | Vector(l) ->
      let ls = List.map pr_str l in
      "[" ^ (String.concat " " ls) ^ "]" 
  | Hashmap(h) ->
      let hs = Types.TMap.fold tmap_fold h [] in
      "{" ^ (String.concat " " hs) ^ "}"
