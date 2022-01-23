let read_atom tokens = failwith "TODO"

let read_list tokens = Types.x

let read_form tokens =
  match tokens with
  | [] -> raise Tokeniser.Eof
  | "(" :: xs -> read_list xs
  | _ -> read_atom tokens 

