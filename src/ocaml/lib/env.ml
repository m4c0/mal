let step2 =
  [ ("+", Core.sum);
    ("-", Core.sub);
    ("*", Core.mult);
    ("/", Core.div);
  ] |> List.to_seq |> Eval.Env.of_seq
