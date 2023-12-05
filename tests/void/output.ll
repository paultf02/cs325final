; ModuleID = './void.c'
source_filename = "./void.c"

declare i32 @print_int(i32)

define void @Void() {
entry:
  %result = alloca i32, align 4
  store i32 0, ptr %result, align 4
  %result1 = load i32, ptr %result, align 4
  %calltmp = call i32 @print_int(i32 %result1)
  br label %whilecondition

whilecondition:                                   ; preds = %whilebody, %entry
  %result2 = load i32, ptr %result, align 4
  %int_slt = icmp slt i32 %result2, 10
  %whilecond = icmp ne i1 %int_slt, false
  br i1 %whilecond, label %whilebody, label %endwhile

whilebody:                                        ; preds = %whilecondition
  %result3 = load i32, ptr %result, align 4
  %int_add = add i32 %result3, 1
  store i32 %int_add, ptr %result, align 4
  %result4 = load i32, ptr %result, align 4
  %calltmp5 = call i32 @print_int(i32 %result4)
  br label %whilecondition

endwhile:                                         ; preds = %whilecondition
  ret void
}
