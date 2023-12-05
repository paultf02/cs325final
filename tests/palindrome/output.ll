; ModuleID = './palindrome.c'
source_filename = "./palindrome.c"

define i1 @palindrome(i32 %number) {
entry:
  %number1 = alloca i32, align 4
  store i32 %number, ptr %number1, align 4
  %t = alloca i32, align 4
  %rev = alloca i32, align 4
  %rmndr = alloca i32, align 4
  %result = alloca i1, align 1
  store i32 0, ptr %rev, align 4
  store i1 false, ptr %result, align 1
  %number2 = load i32, ptr %number1, align 4
  store i32 %number2, ptr %t, align 4
  br label %whilecondition

whilecondition:                                   ; preds = %whilebody, %entry
  %number3 = load i32, ptr %number1, align 4
  %int_sgt = icmp sgt i32 %number3, 0
  %whilecond = icmp ne i1 %int_sgt, false
  br i1 %whilecond, label %whilebody, label %endwhile

whilebody:                                        ; preds = %whilecondition
  %number4 = load i32, ptr %number1, align 4
  %int_srem = srem i32 %number4, 10
  store i32 %int_srem, ptr %rmndr, align 4
  %rev5 = load i32, ptr %rev, align 4
  %int_mul = mul i32 %rev5, 10
  %rmndr6 = load i32, ptr %rmndr, align 4
  %int_add = add i32 %int_mul, %rmndr6
  store i32 %int_add, ptr %rev, align 4
  %number7 = load i32, ptr %number1, align 4
  %int_sdiv = sdiv i32 %number7, 10
  store i32 %int_sdiv, ptr %number1, align 4
  br label %whilecondition

endwhile:                                         ; preds = %whilecondition
  %t8 = load i32, ptr %t, align 4
  %rev9 = load i32, ptr %rev, align 4
  %int_eq = icmp eq i32 %t8, %rev9
  %ifcond = icmp ne i1 %int_eq, false
  br i1 %ifcond, label %then, label %else

then:                                             ; preds = %endwhile
  store i1 true, ptr %result, align 1
  br label %endif

else:                                             ; preds = %endwhile
  store i1 false, ptr %result, align 1
  br label %endif

endif:                                            ; preds = %else, %then
  %result10 = load i1, ptr %result, align 1
  ret i1 %result10
}
