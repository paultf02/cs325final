; ModuleID = './pi.c'
source_filename = "./pi.c"

define float @pi() {
entry:
  %flag = alloca i1, align 1
  %PI = alloca float, align 4
  %i = alloca i32, align 4
  store i1 true, ptr %flag, align 1
  store float 3.000000e+00, ptr %PI, align 4
  store i32 2, ptr %i, align 4
  br label %whilecondition

whilecondition:                                   ; preds = %endif, %entry
  %i1 = load i32, ptr %i, align 4
  %int_slt = icmp slt i32 %i1, 100
  %whilecond = icmp ne i1 %int_slt, false
  br i1 %whilecond, label %whilebody, label %endwhile

whilebody:                                        ; preds = %whilecondition
  %flag2 = load i1, ptr %flag, align 1
  %ifcond = icmp ne i1 %flag2, false
  br i1 %ifcond, label %then, label %else

endwhile:                                         ; preds = %whilecondition
  %PI22 = load float, ptr %PI, align 4
  ret float %PI22

then:                                             ; preds = %whilebody
  %PI3 = load float, ptr %PI, align 4
  %i4 = load i32, ptr %i, align 4
  %i5 = load i32, ptr %i, align 4
  %int_add = add i32 %i5, 1
  %int_mul = mul i32 %i4, %int_add
  %i6 = load i32, ptr %i, align 4
  %int_add7 = add i32 %i6, 2
  %int_mul8 = mul i32 %int_mul, %int_add7
  %int_float_cast = sitofp i32 %int_mul8 to float
  %float_div = fdiv float 4.000000e+00, %int_float_cast
  %float_add = fadd float %PI3, %float_div
  store float %float_add, ptr %PI, align 4
  br label %endif

else:                                             ; preds = %whilebody
  %PI9 = load float, ptr %PI, align 4
  %i10 = load i32, ptr %i, align 4
  %i11 = load i32, ptr %i, align 4
  %int_add12 = add i32 %i11, 1
  %int_mul13 = mul i32 %i10, %int_add12
  %i14 = load i32, ptr %i, align 4
  %int_add15 = add i32 %i14, 2
  %int_mul16 = mul i32 %int_mul13, %int_add15
  %int_float_cast17 = sitofp i32 %int_mul16 to float
  %float_div18 = fdiv float 4.000000e+00, %int_float_cast17
  %float_sub = fsub float %PI9, %float_div18
  store float %float_sub, ptr %PI, align 4
  br label %endif

endif:                                            ; preds = %else, %then
  %flag19 = load i1, ptr %flag, align 1
  %not = icmp ne i1 %flag19, true
  store i1 %not, ptr %flag, align 1
  %i20 = load i32, ptr %i, align 4
  %int_add21 = add i32 %i20, 2
  store i32 %int_add21, ptr %i, align 4
  br label %whilecondition
}
