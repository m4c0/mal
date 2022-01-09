declare i8* @readline(i8* %prompt)
declare i32 @puts(i8* %str)
declare void @free(i8* %ptr)

@.prompt = constant [7 x i8] c"user> \00"

define i8* @READ(i8* %in) {
  ret i8* %in
}
define i8* @EVAL(i8* %in) {
  ret i8* %in
}
define i8* @PRINT(i8* %in) {
  ret i8* %in
}

define i8* @rep(i8* %in) {
  %r = call i8* @READ(i8* %in)
  %e = call i8* @EVAL(i8* %r)
  %p = call i8* @PRINT(i8* %e)
  ret i8* %p
}

define i32 @main(i32 %argc, i8** %argv) {
  br label %loop

loop:
  %line = call i8* @readline(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @.prompt, i64 0, i64 0))
  %is-null = icmp eq i8* %line, null
  br i1 %is-null, label %exit, label %rep

rep:
  %res = call i8* @rep(i8* %line)
  call i32 @puts(i8* %res)
  call void @free(i8* %line)
  br label %loop
  
exit:
  ret i32 0
}
