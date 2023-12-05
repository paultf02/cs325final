; ModuleID = './cosine.c'
source_filename = "./cosine.c"

declare float @print_float(float)

define float @cosine(float %x) {
entry:
  %x1 = alloca float, align 4
  store float %x, ptr %x1, align 4
  %cos = alloca float, align 4
  %n = alloca float, align 4
  %term = alloca float, align 4
  %eps = alloca float, align 4
  %alt = alloca float, align 4
  store float 0x3EB0C6F7A0000000, ptr %eps, align 4
  store float 1.000000e+00, ptr %n, align 4
  store float 1.000000e+00, ptr %cos, align 4
  store float 1.000000e+00, ptr %term, align 4
  store float -1.000000e+00, ptr %alt, align 4
  br label %whilecondition

whilecondition:                                   ; preds = %endif, %entry
  %term2 = load float, ptr %term, align 4
  %eps3 = load float, ptr %eps, align 4
  %float_ogt = fcmp ogt float %term2, %eps3
  %whilecond = icmp ne i1 %float_ogt, false
  br i1 %whilecond, label %whilebody, label %endwhile

whilebody:                                        ; preds = %whilecondition
  %term4 = load float, ptr %term, align 4
  %x5 = load float, ptr %x1, align 4
  %float_mul = fmul float %term4, %x5
  %x6 = load float, ptr %x1, align 4
  %float_mul7 = fmul float %float_mul, %x6
  %n8 = load float, ptr %n, align 4
  %float_div = fdiv float %float_mul7, %n8
  %n9 = load float, ptr %n, align 4
  %float_add = fadd float %n9, 1.000000e+00
  %float_div10 = fdiv float %float_div, %float_add
  store float %float_div10, ptr %term, align 4
  %cos11 = load float, ptr %cos, align 4
  %alt12 = load float, ptr %alt, align 4
  %term13 = load float, ptr %term, align 4
  %float_mul14 = fmul float %alt12, %term13
  %float_add15 = fadd float %cos11, %float_mul14
  store float %float_add15, ptr %cos, align 4
  %alt16 = load float, ptr %alt, align 4
  %0 = fneg float %alt16
  store float %0, ptr %alt, align 4
  %n17 = load float, ptr %n, align 4
  %float_add18 = fadd float %n17, 2.000000e+00
  store float %float_add18, ptr %n, align 4
  %n19 = load float, ptr %n, align 4
  %float_olt = fcmp olt float %n19, 2.000000e+05
  %ifcond = icmp ne i1 %float_olt, false
  br i1 %ifcond, label %then, label %endif

endwhile:                                         ; preds = %whilecondition
  %cos24 = load float, ptr %cos, align 4
  ret float %cos24

then:                                             ; preds = %whilebody
  %term20 = load float, ptr %term, align 4
  %calltmp = call float @print_float(float %term20)
  %cos21 = load float, ptr %cos, align 4
  %calltmp22 = call float @print_float(float %cos21)
  %calltmp23 = call float @print_float(float 4.200000e+01)
  br label %endif

endif:                                            ; preds = %then, %whilebody
  br label %whilecondition
}
