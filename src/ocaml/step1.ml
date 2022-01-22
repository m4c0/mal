let read str : string = str
let eval str : string = str
let print str : string = str

let rep str : string = 
  str |> read |> eval |> print
;;

try
  while true do
    print_string Reader.prompt;
    read_line () |> rep |> print_string;
    print_newline ();
  done;
with
  End_of_file -> ()
