let read str : string =
  let explode s = List.init (String.length s) (String.get s) in
  try str |> explode |> Tokeniser.next |> String.concat " "
  with Tokeniser.Eof -> "EOF"

let eval str : string = str
let print str : string = str

let rep str : string = 
  str |> read |> eval |> print
;;

try
  while true do
    print_string "user> ";
    read_line () |> rep |> print_string;
    print_newline ();
  done;
with
  End_of_file -> ()
