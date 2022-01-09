%st.str = type { i8*, i32 };
@token_current = private global %st.str zeroinitializer

define %st.str @token_peek() {
  %res = load %st.str, %st.str* @token_current
  ret %st.str %res
}

define private void @skip_spaces() {
  %orig = load i8*, i8** getelementptr (%st.str, %st.str* @token_current, i64 0, i32 0)
  br label %loop

loop:
  %ptr = phi i8* [%orig, %0], [%new, %skip]
  %c = load i8, i8* %ptr
  switch i8 %c, label %exit [
    i8 9, label %skip  ; tab
    i8 10, label %skip ; LF
    i8 13, label %skip ; CR
    i8 32, label %skip ; space
    i8 44, label %skip ; comma
  ]

skip:
  %new = getelementptr i8, i8* %ptr, i64 1
  br label %loop

exit:
  store i8* %ptr, i8** getelementptr (%st.str, %st.str* @token_current, i64 0, i32 0)
  ret void
}

define private void @find_token_end() {
  %orig = load i8*, i8** getelementptr (%st.str, %st.str* @token_current, i64 0, i32 0)
  %c = load i8, i8* %orig
  %new = getelementptr i8, i8* %orig, i64 1
  switch i8 %c, label %other [
    i8 0, label %eof
    i8 39, label %single ; '
    i8 40, label %single ; (
    i8 41, label %single ; )
    i8 91, label %single ; [
    i8 93, label %single ; ]
    i8 94, label %single ; ^
    i8 96, label %single ; `
    i8 123, label %single ; {
    i8 125, label %single ; }
    i8 126, label %tilde
  ]

eof:
  store i32 0, i32* getelementptr (%st.str, %st.str* @token_current, i64 0, i32 1)
  br label %exit

tilde:
  %next_c = load i8, i8* %new
  %is_at = icmp eq i8 %next_c, 64
  %len = select i1 %is_at, i32 2, i32 1
  store i32 %len, i32* getelementptr (%st.str, %st.str* @token_current, i64 0, i32 1)
  br label %exit

single:
  store i32 1, i32* getelementptr (%st.str, %st.str* @token_current, i64 0, i32 1)
  br label %exit

other:
  br label %exit

exit:
  ret void
}

define private void @find_next_token(i8* %new_start) {
  store i8* %new_start, i8** getelementptr (%st.str, %st.str* @token_current, i64 0, i32 0)

  call void @skip_spaces()
  call void @find_token_end()
  ret void
}

define void @token_next() {
  %orig = load i8*, i8** getelementptr (%st.str, %st.str* @token_current, i64 0, i32 0)
  %len = load i32, i32* getelementptr (%st.str, %st.str* @token_current, i64 0, i32 1)
  %ptr = getelementptr i8, i8* %orig, i32 %len
  call void @find_next_token(i8* %ptr)
  ret void
}

define void @tokenise(i8* %in) {
  call void @find_next_token(i8* %in)
  ret void
}

