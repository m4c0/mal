let read_atom _ = failwith "TODO"

let read_list _ = failwith "TODO"

let read_form tokens : Types.t =
  match tokens with
  | [] -> raise Tokeniser.Eof
  | "(" :: xs -> read_list xs
  | _ -> read_atom tokens 

