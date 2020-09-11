# chip8_lifter
Static lifter for Chip8 assembly into LLVM IR

Most definitely early-alpha.  Doesn't support any kind of branching, yet.

All the fun IR manipulation is in [lib/src/IREmitter.cpp](https://github.com/antoshre/chip8_lifter/blob/master/lib/src/IREmitter.cpp).

## Example Output
```
examples/print_ir ../../roms/draw_space_invader.ch8
Listing:
6200: SETI	| V2 = 000
6300: SETI	| V3 = 000
A20A: ISETI	| I = 0x20a
D236: DRAW	| draw(V2, V3, 0x6)

; ModuleID = 'module'
source_filename = "module"

define void @f(i8* nocapture readonly %MEM) local_unnamed_addr {
entry:
  %0 = alloca [6 x i8], align 1
  %.sub = getelementptr inbounds [6 x i8], [6 x i8]* %0, i64 0, i64 0
  %"[I]" = getelementptr i8, i8* %MEM, i64 522
  call void @llvm.memcpy.p0i8.p0i8.i8(i8* nonnull align 1 dereferenceable(6) %.sub, i8* nonnull align 1 dereferenceable(6) %"[I]", i8 6, i1 false)
  %1 = call i1 @draw(i8 0, i8 0, i8* nonnull %.sub, i8 6)
  ret void
}

declare i1 @draw(i8, i8, i8*, i8) local_unnamed_addr

; Function Attrs: argmemonly nounwind willreturn
declare void @llvm.memcpy.p0i8.p0i8.i8(i8* noalias nocapture writeonly, i8* noalias nocapture readonly, i8, i1 immarg) #0

attributes #0 = { argmemonly nounwind willreturn }

Process finished with exit code 0
```

Resulting object file can be successfully linked against anything that provides a compatible `draw` function.
That code has to provide a copy of the original `.ch8` file as it almost always contains sprite data.