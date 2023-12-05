; ModuleID = './unary.c'
source_filename = "./unary.c"

declare i32 @print_int(i32)

declare float @print_float(float)

define float @unary(i32 %n, float %m) {
entry:
  %m2 = alloca float, align 4
  %n1 = alloca i32, align 4
  store i32 %n, ptr %n1, align 4
  store float %m, ptr %m2, align 4
  %result = alloca float, align 4
  %sum = alloca float, align 4
  store float 0.000000e+00, ptr %sum, align 4
  %n3 = load i32, ptr %n1, align 4
  %m4 = load float, ptr %m2, align 4
  %int_float_cast = sitofp i32 %n3 to float
  %float_add = fadd float %int_float_cast, %m4
  store float %float_add, ptr %result, align 4
  %result5 = load float, ptr %result, align 4
  %calltmp = call float @print_float(float %result5)
  %sum6 = load float, ptr %sum, align 4
  %result7 = load float, ptr %result, align 4
  %float_add8 = fadd float %sum6, %result7
  store float %float_add8, ptr %sum, align 4
  %n9 = load i32, ptr %n1, align 4
  %m10 = load float, ptr %m2, align 4
  %0 = fneg float %m10
  %int_float_cast11 = sitofp i32 %n9 to float
  %float_add12 = fadd float %int_float_cast11, %0
  store float %float_add12, ptr %result, align 4
  %result13 = load float, ptr %result, align 4
  %calltmp14 = call float @print_float(float %result13)
  %sum15 = load float, ptr %sum, align 4
  %result16 = load float, ptr %result, align 4
  %float_add17 = fadd float %sum15, %result16
  store float %float_add17, ptr %sum, align 4
  %n18 = load i32, ptr %n1, align 4
  %m19 = load float, ptr %m2, align 4
  %1 = fneg float %m19
  %2 = fneg float %1
  %int_float_cast20 = sitofp i32 %n18 to float
  %float_add21 = fadd float %int_float_cast20, %2
  store float %float_add21, ptr %result, align 4
  %result22 = load float, ptr %result, align 4
  %calltmp23 = call float @print_float(float %result22)
  %sum24 = load float, ptr %sum, align 4
  %result25 = load float, ptr %result, align 4
  %float_add26 = fadd float %sum24, %result25
  store float %float_add26, ptr %sum, align 4
  %n27 = load i32, ptr %n1, align 4
  %3 = sub i32 0, %n27
  %m28 = load float, ptr %m2, align 4
  %4 = fneg float %m28
  %int_float_cast29 = sitofp i32 %3 to float
  %float_add30 = fadd float %int_float_cast29, %4
  store float %float_add30, ptr %result, align 4
  %result31 = load float, ptr %result, align 4
  %calltmp32 = call float @print_float(float %result31)
  %sum33 = load float, ptr %sum, align 4
  %result34 = load float, ptr %result, align 4
  %float_add35 = fadd float %sum33, %result34
  store float %float_add35, ptr %sum, align 4
  %sum36 = load float, ptr %sum, align 4
  ret float %sum36
}
