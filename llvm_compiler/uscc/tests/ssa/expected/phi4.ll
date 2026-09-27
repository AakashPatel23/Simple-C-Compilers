; ModuleID = 'phi4.ll'
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
  br i1 %3, label %BB4, label %BB5

BB4:                                              ; preds = %BB6, %0
  %4 = phi i32 [ %1, %0 ], [ %6, %BB6 ]
  br i1 %3, label %BB6, label %BB5

BB5:                                              ; preds = %BB4, %0
  %5 = phi i32 [ %2, %0 ], [ %4, %BB4 ]
  br label %BB6

BB6:                                              ; preds = %BB9, %BB7, %BB6.1, %BB5, %BB4
  %6 = phi i32 [ %6, %BB6.1 ], [ %5, %BB5 ], [ %4, %BB4 ], [ %6, %BB7 ], [ %6, %BB9 ]
  br i1 %3, label %BB6.1, label %BB4

BB6.1:                                            ; preds = %BB6
  br i1 %3, label %BB6.2, label %BB6

BB6.2:                                            ; preds = %BB6.1
  br i1 %3, label %BB6.3, label %BB7

BB6.3:                                            ; preds = %BB6.2
  br i1 %3, label %BB6.4, label %BB8

BB6.4:                                            ; preds = %BB6.3
  br i1 %3, label %BB6.5, label %BB10

BB6.5:                                            ; preds = %BB6.4
  br i1 %3, label %BB9, label %End

BB7:                                              ; preds = %BB8, %BB6.2
  br i1 %3, label %BB6, label %BB8

BB8:                                              ; preds = %BB7, %BB6.3
  br label %BB7

BB9:                                              ; preds = %BB10, %BB6.5
  br i1 %3, label %BB6, label %BB10

BB10:                                             ; preds = %BB9, %BB6.4
  br label %BB9

End:                                              ; preds = %BB6.5
  ret i32 %6
}

; Function Attrs: nounwind
declare i32 @__isoc99_scanf(i8* nocapture readonly, ...) #1

; Function Attrs: nounwind
declare i32 @printf(i8* nocapture readonly, ...) #1

attributes #0 = { nounwind uwtable "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { nounwind }

!llvm.ident = !{!0}

!0 = metadata !{metadata !"clang version 3.5.0 (tags/RELEASE_350/final)"}
!1 = metadata !{metadata !2, metadata !2, i64 0}
!2 = metadata !{metadata !"int", metadata !3, i64 0}
!3 = metadata !{metadata !"omnipotent char", metadata !4, i64 0}
!4 = metadata !{metadata !"Simple C/C++ TBAA"}
