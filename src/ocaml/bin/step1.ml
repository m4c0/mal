open Lib

let read str = Reader.read_str str
let eval t = t
let print t = Printer.pr_str t

let rep str : string = 
  try str |> read |> eval |> print
  with Tokeniser.Eof -> "EOF"
;;

try
  while true do
    print_string "user> ";
    read_line () |> rep |> print_string;
    print_newline ();
  done;
with
  End_of_file -> ()
