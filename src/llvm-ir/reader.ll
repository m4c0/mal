; Will be a pointer inside the line returned by readline
; Assumes caller manages the original pointer
@token_current = private global i8* null
define i8* @token_peek() {
  %res = load i8*, i8** @token_current
  ret i8* %res
}

define private i8 @peek() {
  %str = load i8*, i8** @token_current
  %c = load i8, i8* %str
  ret i8 %c
}
define private void @adv() {
  %ptr = load i8*, i8** @token_current
  %new = getelementptr i8, i8* %ptr, i64 1
  store i8* %new, i8** @token_current
  ret void
}

define private void @skip_spaces() {
  br label %loop

loop:
  %c = call i8 @peek()
  switch i8 %c, label %exit [
    i8 9, label %skip  ; tab
    i8 10, label %skip ; LF
    i8 13, label %skip ; CR
    i8 32, label %skip ; space
    i8 44, label %skip ; comma
  ]

skip:
  call void @adv()
  br label %loop

exit:
  ret void
}
define private void @find_next_token() {
  call void @skip_spaces()
  ret void
}

define i8* @token_next() {
  call void @find_next_token()
  %res = call i8* @token_peek()
  ret i8* %res
}

define void @tokenise(i8* %in) {
  store i8* %in, i8** @token_current
  call void @find_next_token()
  ret void
}

