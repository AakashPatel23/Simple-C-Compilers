target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@.str = private unnamed_addr constant [3 x i8] c"%d\00", align 1
@.str1 = private unnamed_addr constant [3 x i8] c"%d\00", align 1

; Function Attrs: nounwind uwtable
define i32 @main() #0 {
  %n = alloca i32, align 4
  %1 = call i32 (i8*, ...)* @__isoc99_scanf(i8* getelementptr inbounds ([3 x i8]* @.str, i64 0, i64 0), i32* %n) #2
  %2 = load i32* %n, align 4, !tbaa !1
  %3 = icmp slt i32 %2, 3
  br i1 %3, label %BB5, label %BB5_1

BB5:
  %x5 = phi i32 [ %x5, %BB5 ], [ %x6, %BB6 ], [ %x7, %BB7 ], [ %x8, %BB8 ], [ %2, %0 ]  

  switch i32 %2, label %BB5 [ i32 1, label %BB6 i32 2, label %BB7 i32 3, label %BB8 ] 

BB6:
  %x6 = phi i32 [ %x5, %BB5 ], [ %x6, %BB6 ], [ %x7, %BB7 ], [ %x8, %BB8 ] 

  switch i32 %2, label %BB5 [ i32 1, label %BB6 i32 2, label %BB7 i32 3, label %BB8 ] 
  
BB7:
  %x7 = phi i32 [ %x5, %BB5 ], [ %x6, %BB6 ], [ %x7, %BB7 ], [ %x8, %BB8 ] 

  switch i32 %2, label %BB5 [ i32 1, label %BB6 i32 2, label %BB7 i32 3, label %BB8 ] 
  
BB8:
  %x8 = phi i32 [ %x5, %BB5 ], [ %x6, %BB6 ], [ %x7, %BB7 ], [ %x8, %BB8 ] 

  switch i32 %2, label %BB5 [ i32 1, label %BB6 i32 2, label %BB7 i32 3, label %BB8  i32 4, label %BB6_1 ] 
  

BB5_1:
  %y5 = phi i32 [ %y5, %BB5_1 ], [ %y6, %BB6_1 ], [ %y7, %BB7_1 ], [ %y8, %BB8_1 ], [ %2, %0 ]

  switch i32 %2, label %BB5_1 [ i32 1, label %BB6_1 i32 2, label %BB7_1 i32 3, label %BB8_1 ] 

BB6_1:
  %y6 = phi i32 [ %y5, %BB5_1 ], [ %y6, %BB6_1 ], [ %y7, %BB7_1 ], [ %y8, %BB8_1 ], [ %x8, %BB8 ]

  switch i32 %2, label %BB5_1 [ i32 1, label %BB6_1 i32 2, label %BB7_1 i32 3, label %BB8_1 ] 
  
BB7_1:
  %y7 = phi i32 [ %y5, %BB5_1 ], [ %y6, %BB6_1 ], [ %y7, %BB7_1 ], [ %y8, %BB8_1 ], [ %z5, %BB5_2 ]

  switch i32 %2, label %BB5_1 [ i32 1, label %BB6_1 i32 2, label %BB7_1 i32 3, label %BB8_1 i32 4, label %BB5_2 ] 
  
BB8_1:
  %y8 = phi i32 [ %y5, %BB5_1 ], [ %y6, %BB6_1 ], [ %y7, %BB7_1 ], [ %y8, %BB8_1 ]

  switch i32 %2, label %BB5_1 [ i32 1, label %BB6_1 i32 2, label %BB7_1 i32 3, label %BB8_1 ] 


BB5_2:
  %z5 = phi i32 [ %x5, %BB5_2 ], [ %x6, %BB6_2 ], [ %x7, %BB7_2 ], [ %x8, %BB8_2 ], [ %x8, %BB7_1 ] 

  switch i32 %2, label %BB5_2 [ i32 1, label %BB6_2 i32 2, label %BB7_2 i32 3, label %BB8_2 i32 4, label %BB7_1 ] 

BB6_2:
  %z6 = phi i32 [ %x5, %BB5_2 ], [ %x6, %BB6_2 ], [ %x7, %BB7_2 ], [ %x8, %BB8_2 ] 

  switch i32 %2, label %BB5_2 [ i32 1, label %BB6_2 i32 2, label %BB7_2 i32 3, label %BB8_2 ] 
  
BB7_2:
  %z7 = phi i32 [ %x5, %BB5_2 ], [ %x6, %BB6_2 ], [ %x7, %BB7_2 ], [ %x8, %BB8_2 ] 

  switch i32 %2, label %BB5_2 [ i32 1, label %BB6_2 i32 2, label %BB7_2 i32 3, label %BB8_2 ] 
  
BB8_2:
  %z8 = phi i32 [ %x5, %BB5_2 ], [ %x6, %BB6_2 ], [ %x7, %BB7_2 ], [ %x8, %BB8_2 ] 

  switch i32 %2, label %BB5_2 [ i32 1, label %BB6_2 i32 2, label %BB7_2 i32 3, label %BB8_2 i32 4, label %E1 ] 



E1:
  ret i32 %z8
}

; Function Attrs: nounwind
declare i32 @__isoc99_scanf(i8* nocapture readonly, ...) #1

attributes #0 = { nounwind uwtable "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { nounwind }

!llvm.ident = !{!0}

!0 = metadata !{metadata !"clang version 3.5.0 (tags/RELEASE_350/final)"}
!1 = metadata !{metadata !2, metadata !2, i64 0}
!2 = metadata !{metadata !"int", metadata !3, i64 0}
!3 = metadata !{metadata !"omnipotent char", metadata !4, i64 0}
!4 = metadata !{metadata !"Simple C/C++ TBAA"}
