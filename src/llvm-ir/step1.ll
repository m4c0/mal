; externals
declare i8* @readline(i8* %prompt)
declare i32 @printf(i8* %fmt, ...)
declare void @free(i8* %ptr)

; internals
%st.str = type { i8*, i32 }
declare void @tokenise(i8* %in)
declare void @token_next()
declare %st.str @token_peek()

@.prompt = constant [7 x i8] c"user> \00"
@.printf-fmt = constant [6 x i8] c"%.*s\0A\00"

define %st.str @READ(i8* %in) {
  call void @tokenise(i8* %in)
  %res = call %st.str @token_peek()
  ret %st.str %res
}
define %st.str @EVAL(%st.str %in) {
  ret %st.str %in
}
define %st.str @PRINT(%st.str %in) {
  ret %st.str %in
}

define %st.str @rep(i8* %in) {
  %r = call %st.str @READ(i8* %in)
  %e = call %st.str @EVAL(%st.str %r)
  %p = call %st.str @PRINT(%st.str %e)
  ret %st.str %p
}

define i32 @main(i32 %argc, i8** %argv) {
  br label %loop

loop:
  %line = call i8* @readline(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @.prompt, i64 0, i64 0))
  %is-null = icmp eq i8* %line, null
  br i1 %is-null, label %exit, label %rep

rep:
  %res = call %st.str @rep(i8* %line)
  %res.str = extractvalue %st.str %res, 0
  %res.len = extractvalue %st.str %res, 1
  call i32(i8*, ...) @printf(i8* getelementptr ([6 x i8], [6 x i8]* @.printf-fmt, i64 0, i64 0), i32 %res.len, i8* %res.str)
  call void @free(i8* %line)
  br label %loop
  
exit:
  ret i32 0
}
