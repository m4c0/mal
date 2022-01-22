exception Eof

let rec skip_comment s =
  match s with
  | [] -> []
  | '\n' :: xs -> xs
  | _ :: xs -> skip_comment xs

let rec next s =
  let rec read_string c s =
    match s with
    | [] -> raise Eof
    | '"' :: xs -> (c ^ "\"") :: next xs
    | '\\' :: x :: xs -> read_string (c ^ "\\" ^ String.make 1 x) xs
    | x :: xs -> read_string (c ^ String.make 1 x) xs
  in
  let rec read_seq c s =
    match s with
    | [] -> c :: []
    | x :: xs ->
        match x with
        | '~' | '@'
        | ' ' | ',' | '\012' | '\n' | '\r' | '\t' 
        | '[' | ']' | '{' | '}' | '(' | ')'
        | '"' 
        | ';'  -> c :: next s
        | x -> read_seq (c ^ String.make 1 x) xs
  in

  match s with
  | [] -> []
  | '~' :: '@' :: xs -> "~@" :: next xs
  | x :: xs ->
      match x with
      | ' ' | ',' | '\012' | '\n' | '\r' | '\t' -> next xs
      | '[' | ']' | '{' | '}' | '(' | ')' | '~' | '@' -> (String.make 1 x) :: next xs
      | '"' -> read_string "\"" xs
      | ';' -> xs |> skip_comment |> next
      | x -> read_seq (String.make 1 x) xs
