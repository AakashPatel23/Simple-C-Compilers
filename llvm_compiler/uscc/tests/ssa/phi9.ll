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
  br i1 %3, label %BBX0, label %BBX1

BBX0:
  %x0 = phi i32 [ %x99, %BBX99 ], [ %x1, %BBX1 ], [ %2, %0 ]  
  switch i32 %2, label %BBX99 [ i32 1, label %BBX1] 
BBX1:
  %x1 = phi i32 [ %x0, %BBX0 ], [ %x2, %BBX2 ], [ %1, %0 ]  
  switch i32 %2, label %BBX0 [ i32 1, label %BBX2] 
BBX2:
  %x2 = phi i32 [ %x1, %BBX1 ], [ %x3, %BBX3 ] 
  switch i32 %2, label %BBX1 [ i32 1, label %BBX3] 
BBX3:
  %x3 = phi i32 [ %x2, %BBX2 ], [ %x4, %BBX4 ] 
  switch i32 %2, label %BBX2 [ i32 1, label %BBX4] 
BBX4:
  %x4 = phi i32 [ %x3, %BBX3 ], [ %x5, %BBX5 ] 
  switch i32 %2, label %BBX3 [ i32 1, label %BBX5] 
BBX5:
  %x5 = phi i32 [ %x4, %BBX4 ], [ %x6, %BBX6 ] 
  switch i32 %2, label %BBX4 [ i32 1, label %BBX6] 
BBX6:
  %x6 = phi i32 [ %x5, %BBX5 ], [ %x7, %BBX7 ] 
  switch i32 %2, label %BBX5 [ i32 1, label %BBX7] 
BBX7:
  %x7 = phi i32 [ %x6, %BBX6 ], [ %x8, %BBX8 ] 
  switch i32 %2, label %BBX6 [ i32 1, label %BBX8] 
BBX8:
  %x8 = phi i32 [ %x7, %BBX7 ], [ %x9, %BBX9 ] 
  switch i32 %2, label %BBX7 [ i32 1, label %BBX9] 
BBX9:
  %x9 = phi i32 [ %x8, %BBX8 ], [ %x10, %BBX10 ] 
  switch i32 %2, label %BBX8 [ i32 1, label %BBX10] 
BBX10:
  %x10 = phi i32 [ %x9, %BBX9 ], [ %x11, %BBX11 ] 
  switch i32 %2, label %BBX9 [ i32 1, label %BBX11] 
BBX11:
  %x11 = phi i32 [ %x10, %BBX10 ], [ %x12, %BBX12 ] 
  switch i32 %2, label %BBX10 [ i32 1, label %BBX12] 
BBX12:
  %x12 = phi i32 [ %x11, %BBX11 ], [ %x13, %BBX13 ] 
  switch i32 %2, label %BBX11 [ i32 1, label %BBX13] 
BBX13:
  %x13 = phi i32 [ %x12, %BBX12 ], [ %x14, %BBX14 ] 
  switch i32 %2, label %BBX12 [ i32 1, label %BBX14] 
BBX14:
  %x14 = phi i32 [ %x13, %BBX13 ], [ %x15, %BBX15 ] 
  switch i32 %2, label %BBX13 [ i32 1, label %BBX15] 
BBX15:
  %x15 = phi i32 [ %x14, %BBX14 ], [ %x16, %BBX16 ] 
  switch i32 %2, label %BBX14 [ i32 1, label %BBX16] 
BBX16:
  %x16 = phi i32 [ %x15, %BBX15 ], [ %x17, %BBX17 ] 
  switch i32 %2, label %BBX15 [ i32 1, label %BBX17] 
BBX17:
  %x17 = phi i32 [ %x16, %BBX16 ], [ %x18, %BBX18 ] 
  switch i32 %2, label %BBX16 [ i32 1, label %BBX18] 
BBX18:
  %x18 = phi i32 [ %x17, %BBX17 ], [ %x19, %BBX19 ] 
  switch i32 %2, label %BBX17 [ i32 1, label %BBX19] 
BBX19:
  %x19 = phi i32 [ %x18, %BBX18 ], [ %x20, %BBX20 ] 
  switch i32 %2, label %BBX18 [ i32 1, label %BBX20] 
BBX20:
  %x20 = phi i32 [ %x19, %BBX19 ], [ %x21, %BBX21 ] 
  switch i32 %2, label %BBX19 [ i32 1, label %BBX21] 
BBX21:
  %x21 = phi i32 [ %x20, %BBX20 ], [ %x22, %BBX22 ] 
  switch i32 %2, label %BBX20 [ i32 1, label %BBX22] 
BBX22:
  %x22 = phi i32 [ %x21, %BBX21 ], [ %x23, %BBX23 ] 
  switch i32 %2, label %BBX21 [ i32 1, label %BBX23] 
BBX23:
  %x23 = phi i32 [ %x22, %BBX22 ], [ %x24, %BBX24 ] 
  switch i32 %2, label %BBX22 [ i32 1, label %BBX24] 
BBX24:
  %x24 = phi i32 [ %x23, %BBX23 ], [ %x25, %BBX25 ] 
  switch i32 %2, label %BBX23 [ i32 1, label %BBX25] 
BBX25:
  %x25 = phi i32 [ %x24, %BBX24 ], [ %x26, %BBX26 ] 
  switch i32 %2, label %BBX24 [ i32 1, label %BBX26] 
BBX26:
  %x26 = phi i32 [ %x25, %BBX25 ], [ %x27, %BBX27 ] 
  switch i32 %2, label %BBX25 [ i32 1, label %BBX27] 
BBX27:
  %x27 = phi i32 [ %x26, %BBX26 ], [ %x28, %BBX28 ] 
  switch i32 %2, label %BBX26 [ i32 1, label %BBX28] 
BBX28:
  %x28 = phi i32 [ %x27, %BBX27 ], [ %x29, %BBX29 ] 
  switch i32 %2, label %BBX27 [ i32 1, label %BBX29] 
BBX29:
  %x29 = phi i32 [ %x28, %BBX28 ], [ %x30, %BBX30 ] 
  switch i32 %2, label %BBX28 [ i32 1, label %BBX30] 
BBX30:
  %x30 = phi i32 [ %x29, %BBX29 ], [ %x31, %BBX31 ] 
  switch i32 %2, label %BBX29 [ i32 1, label %BBX31] 
BBX31:
  %x31 = phi i32 [ %x30, %BBX30 ], [ %x32, %BBX32 ] 
  switch i32 %2, label %BBX30 [ i32 1, label %BBX32] 
BBX32:
  %x32 = phi i32 [ %x31, %BBX31 ], [ %x33, %BBX33 ] 
  switch i32 %2, label %BBX31 [ i32 1, label %BBX33] 
BBX33:
  %x33 = phi i32 [ %x32, %BBX32 ], [ %x34, %BBX34 ] 
  switch i32 %2, label %BBX32 [ i32 1, label %BBX34] 
BBX34:
  %x34 = phi i32 [ %x33, %BBX33 ], [ %x35, %BBX35 ] 
  switch i32 %2, label %BBX33 [ i32 1, label %BBX35] 
BBX35:
  %x35 = phi i32 [ %x34, %BBX34 ], [ %x36, %BBX36 ] 
  switch i32 %2, label %BBX34 [ i32 1, label %BBX36] 
BBX36:
  %x36 = phi i32 [ %x35, %BBX35 ], [ %x37, %BBX37 ] 
  switch i32 %2, label %BBX35 [ i32 1, label %BBX37] 
BBX37:
  %x37 = phi i32 [ %x36, %BBX36 ], [ %x38, %BBX38 ] 
  switch i32 %2, label %BBX36 [ i32 1, label %BBX38] 
BBX38:
  %x38 = phi i32 [ %x37, %BBX37 ], [ %x39, %BBX39 ] 
  switch i32 %2, label %BBX37 [ i32 1, label %BBX39] 
BBX39:
  %x39 = phi i32 [ %x38, %BBX38 ], [ %x40, %BBX40 ] 
  switch i32 %2, label %BBX38 [ i32 1, label %BBX40] 
BBX40:
  %x40 = phi i32 [ %x39, %BBX39 ], [ %x41, %BBX41 ] 
  switch i32 %2, label %BBX39 [ i32 1, label %BBX41] 
BBX41:
  %x41 = phi i32 [ %x40, %BBX40 ], [ %x42, %BBX42 ] 
  switch i32 %2, label %BBX40 [ i32 1, label %BBX42] 
BBX42:
  %x42 = phi i32 [ %x41, %BBX41 ], [ %x43, %BBX43 ] 
  switch i32 %2, label %BBX41 [ i32 1, label %BBX43] 
BBX43:
  %x43 = phi i32 [ %x42, %BBX42 ], [ %x44, %BBX44 ] 
  switch i32 %2, label %BBX42 [ i32 1, label %BBX44] 
BBX44:
  %x44 = phi i32 [ %x43, %BBX43 ], [ %x45, %BBX45 ] 
  switch i32 %2, label %BBX43 [ i32 1, label %BBX45] 
BBX45:
  %x45 = phi i32 [ %x44, %BBX44 ], [ %x46, %BBX46 ] 
  switch i32 %2, label %BBX44 [ i32 1, label %BBX46] 
BBX46:
  %x46 = phi i32 [ %x45, %BBX45 ], [ %x47, %BBX47 ] 
  switch i32 %2, label %BBX45 [ i32 1, label %BBX47] 
BBX47:
  %x47 = phi i32 [ %x46, %BBX46 ], [ %x48, %BBX48 ] 
  switch i32 %2, label %BBX46 [ i32 1, label %BBX48] 
BBX48:
  %x48 = phi i32 [ %x47, %BBX47 ], [ %x49, %BBX49 ] 
  switch i32 %2, label %BBX47 [ i32 1, label %BBX49] 
BBX49:
  %x49 = phi i32 [ %x48, %BBX48 ], [ %x50, %BBX50 ] 
  switch i32 %2, label %BBX48 [ i32 1, label %BBX50] 
BBX50:
  %x50 = phi i32 [ %x49, %BBX49 ], [ %x51, %BBX51 ] 
  switch i32 %2, label %BBX49 [ i32 1, label %BBX51] 
BBX51:
  %x51 = phi i32 [ %x50, %BBX50 ], [ %x52, %BBX52 ] 
  switch i32 %2, label %BBX50 [ i32 1, label %BBX52] 
BBX52:
  %x52 = phi i32 [ %x51, %BBX51 ], [ %x53, %BBX53 ] 
  switch i32 %2, label %BBX51 [ i32 1, label %BBX53] 
BBX53:
  %x53 = phi i32 [ %x52, %BBX52 ], [ %x54, %BBX54 ] 
  switch i32 %2, label %BBX52 [ i32 1, label %BBX54] 
BBX54:
  %x54 = phi i32 [ %x53, %BBX53 ], [ %x55, %BBX55 ] 
  switch i32 %2, label %BBX53 [ i32 1, label %BBX55] 
BBX55:
  %x55 = phi i32 [ %x54, %BBX54 ], [ %x56, %BBX56 ] 
  switch i32 %2, label %BBX54 [ i32 1, label %BBX56] 
BBX56:
  %x56 = phi i32 [ %x55, %BBX55 ], [ %x57, %BBX57 ] 
  switch i32 %2, label %BBX55 [ i32 1, label %BBX57] 
BBX57:
  %x57 = phi i32 [ %x56, %BBX56 ], [ %x58, %BBX58 ] 
  switch i32 %2, label %BBX56 [ i32 1, label %BBX58] 
BBX58:
  %x58 = phi i32 [ %x57, %BBX57 ], [ %x59, %BBX59 ] 
  switch i32 %2, label %BBX57 [ i32 1, label %BBX59] 
BBX59:
  %x59 = phi i32 [ %x58, %BBX58 ], [ %x60, %BBX60 ] 
  switch i32 %2, label %BBX58 [ i32 1, label %BBX60] 
BBX60:
  %x60 = phi i32 [ %x59, %BBX59 ], [ %x61, %BBX61 ] 
  switch i32 %2, label %BBX59 [ i32 1, label %BBX61] 
BBX61:
  %x61 = phi i32 [ %x60, %BBX60 ], [ %x62, %BBX62 ] 
  switch i32 %2, label %BBX60 [ i32 1, label %BBX62] 
BBX62:
  %x62 = phi i32 [ %x61, %BBX61 ], [ %x63, %BBX63 ] 
  switch i32 %2, label %BBX61 [ i32 1, label %BBX63] 
BBX63:
  %x63 = phi i32 [ %x62, %BBX62 ], [ %x64, %BBX64 ] 
  switch i32 %2, label %BBX62 [ i32 1, label %BBX64] 
BBX64:
  %x64 = phi i32 [ %x63, %BBX63 ], [ %x65, %BBX65 ] 
  switch i32 %2, label %BBX63 [ i32 1, label %BBX65] 
BBX65:
  %x65 = phi i32 [ %x64, %BBX64 ], [ %x66, %BBX66 ] 
  switch i32 %2, label %BBX64 [ i32 1, label %BBX66] 
BBX66:
  %x66 = phi i32 [ %x65, %BBX65 ], [ %x67, %BBX67 ] 
  switch i32 %2, label %BBX65 [ i32 1, label %BBX67] 
BBX67:
  %x67 = phi i32 [ %x66, %BBX66 ], [ %x68, %BBX68 ] 
  switch i32 %2, label %BBX66 [ i32 1, label %BBX68] 
BBX68:
  %x68 = phi i32 [ %x67, %BBX67 ], [ %x69, %BBX69 ] 
  switch i32 %2, label %BBX67 [ i32 1, label %BBX69] 
BBX69:
  %x69 = phi i32 [ %x68, %BBX68 ], [ %x70, %BBX70 ] 
  switch i32 %2, label %BBX68 [ i32 1, label %BBX70] 
BBX70:
  %x70 = phi i32 [ %x69, %BBX69 ], [ %x71, %BBX71 ] 
  switch i32 %2, label %BBX69 [ i32 1, label %BBX71] 
BBX71:
  %x71 = phi i32 [ %x70, %BBX70 ], [ %x72, %BBX72 ] 
  switch i32 %2, label %BBX70 [ i32 1, label %BBX72] 
BBX72:
  %x72 = phi i32 [ %x71, %BBX71 ], [ %x73, %BBX73 ] 
  switch i32 %2, label %BBX71 [ i32 1, label %BBX73] 
BBX73:
  %x73 = phi i32 [ %x72, %BBX72 ], [ %x74, %BBX74 ] 
  switch i32 %2, label %BBX72 [ i32 1, label %BBX74] 
BBX74:
  %x74 = phi i32 [ %x73, %BBX73 ], [ %x75, %BBX75 ] 
  switch i32 %2, label %BBX73 [ i32 1, label %BBX75] 
BBX75:
  %x75 = phi i32 [ %x74, %BBX74 ], [ %x76, %BBX76 ] 
  switch i32 %2, label %BBX74 [ i32 1, label %BBX76] 
BBX76:
  %x76 = phi i32 [ %x75, %BBX75 ], [ %x77, %BBX77 ] 
  switch i32 %2, label %BBX75 [ i32 1, label %BBX77] 
BBX77:
  %x77 = phi i32 [ %x76, %BBX76 ], [ %x78, %BBX78 ] 
  switch i32 %2, label %BBX76 [ i32 1, label %BBX78] 
BBX78:
  %x78 = phi i32 [ %x77, %BBX77 ], [ %x79, %BBX79 ] 
  switch i32 %2, label %BBX77 [ i32 1, label %BBX79] 
BBX79:
  %x79 = phi i32 [ %x78, %BBX78 ], [ %x80, %BBX80 ] 
  switch i32 %2, label %BBX78 [ i32 1, label %BBX80] 
BBX80:
  %x80 = phi i32 [ %x79, %BBX79 ], [ %x81, %BBX81 ] 
  switch i32 %2, label %BBX79 [ i32 1, label %BBX81] 
BBX81:
  %x81 = phi i32 [ %x80, %BBX80 ], [ %x82, %BBX82 ] 
  switch i32 %2, label %BBX80 [ i32 1, label %BBX82] 
BBX82:
  %x82 = phi i32 [ %x81, %BBX81 ], [ %x83, %BBX83 ] 
  switch i32 %2, label %BBX81 [ i32 1, label %BBX83] 
BBX83:
  %x83 = phi i32 [ %x82, %BBX82 ], [ %x84, %BBX84 ] 
  switch i32 %2, label %BBX82 [ i32 1, label %BBX84] 
BBX84:
  %x84 = phi i32 [ %x83, %BBX83 ], [ %x85, %BBX85 ] 
  switch i32 %2, label %BBX83 [ i32 1, label %BBX85] 
BBX85:
  %x85 = phi i32 [ %x84, %BBX84 ], [ %x86, %BBX86 ] 
  switch i32 %2, label %BBX84 [ i32 1, label %BBX86] 
BBX86:
  %x86 = phi i32 [ %x85, %BBX85 ], [ %x87, %BBX87 ] 
  switch i32 %2, label %BBX85 [ i32 1, label %BBX87] 
BBX87:
  %x87 = phi i32 [ %x86, %BBX86 ], [ %x88, %BBX88 ] 
  switch i32 %2, label %BBX86 [ i32 1, label %BBX88] 
BBX88:
  %x88 = phi i32 [ %x87, %BBX87 ], [ %x89, %BBX89 ] 
  switch i32 %2, label %BBX87 [ i32 1, label %BBX89] 
BBX89:
  %x89 = phi i32 [ %x88, %BBX88 ], [ %x90, %BBX90 ] 
  switch i32 %2, label %BBX88 [ i32 1, label %BBX90] 
BBX90:
  %x90 = phi i32 [ %x89, %BBX89 ], [ %x91, %BBX91 ] 
  switch i32 %2, label %BBX89 [ i32 1, label %BBX91] 
BBX91:
  %x91 = phi i32 [ %x90, %BBX90 ], [ %x92, %BBX92 ] 
  switch i32 %2, label %BBX90 [ i32 1, label %BBX92] 
BBX92:
  %x92 = phi i32 [ %x91, %BBX91 ], [ %x93, %BBX93 ] 
  switch i32 %2, label %BBX91 [ i32 1, label %BBX93] 
BBX93:
  %x93 = phi i32 [ %x92, %BBX92 ], [ %x94, %BBX94 ] 
  switch i32 %2, label %BBX92 [ i32 1, label %BBX94] 
BBX94:
  %x94 = phi i32 [ %x93, %BBX93 ], [ %x95, %BBX95 ] 
  switch i32 %2, label %BBX93 [ i32 1, label %BBX95] 
BBX95:
  %x95 = phi i32 [ %x94, %BBX94 ], [ %x96, %BBX96 ] 
  switch i32 %2, label %BBX94 [ i32 1, label %BBX96] 
BBX96:
  %x96 = phi i32 [ %x95, %BBX95 ], [ %x97, %BBX97 ] 
  switch i32 %2, label %BBX95 [ i32 1, label %BBX97] 
BBX97:
  %x97 = phi i32 [ %x96, %BBX96 ], [ %x98, %BBX98 ] 
  switch i32 %2, label %BBX96 [ i32 1, label %BBX98] 
BBX98:
  %x98 = phi i32 [ %x97, %BBX97 ], [ %x99, %BBX99 ] 
  switch i32 %2, label %BBX97 [ i32 1, label %BBX99] 
BBX99:
  %x99 = phi i32 [ %x98, %BBX98 ], [ %x0, %BBX0 ] 
  switch i32 %2, label %BBX98 [ i32 1, label %BBX0  i32 2, label %E1 ] 

E1:
  ret i32 %x99
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
