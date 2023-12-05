; ModuleID = './while.c'
source_filename = "./while.c"

@test = common global i32 0
@f = common global float 0.000000e+00
@b = common global i1 false

declare i32 @print_int(i32)

define i32 @While(i32 %n) {
entry:
  %n1 = alloca i32, align 4
  store i32 %n, ptr %n1, align 4
  %result = alloca i32, align 4
  store i32 12, ptr @test, align 4
  store i32 0, ptr %result, align 4
  %test = load i32, ptr @test, align 4
  %calltmp = call i32 @print_int(i32 %test)
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
  br label %whilecondition

endwhile:                                         ; preds = %whilecondition
  %result4 = load i32, ptr %result, align 4
  ret i32 %result4
}
