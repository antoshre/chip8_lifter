# chip8_lifter
Static lifter for Chip8 assembly into LLVM IR

Most definitely early-alpha.  Doesn't support any kind of branching, yet.

All the fun IR manipulation is in [lib/src/IREmitter.cpp](https://github.com/antoshre/chip8_lifter/blob/master/lib/src/IREmitter.cpp).

## Example Output
`draw_space_invader.ch8`:
```
6200: SETI	| V2 = 000
6300: SETI	| V3 = 000
A20A: ISETI	| I = 0x20a
D236: DRAW	| draw(V2, V3, 0x6)
0000: //indicate end of program
BA7C: //sprite data
D6FE
54AA
```
`V2` and `V3` are set to `0` to indicate the sprite should be drawn at `(0,0)`.

Chip8 programs are traditionally mapped starting at `0x200`, so `0x20A` corresponds to the location of the sprite data.  In this case the data is `6` bytes long.
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
The resulting LLVM IR is functionally identical to the following C++ program:
```
#include <cstdint>
#include <cstring>

extern "C" bool draw(std::uint8_t, std::uint8_t, std::uint8_t*, std::uint8_t);

void f(std::uint8_t* MEM) {
    std::uint8_t x,y;
    x = 0;
    y = 0;
    std::uint16_t I;
    I = 0x0206;
    std::uint8_t sprite[6];
    std::memcpy(sprite, &MEM[I], 6);
    draw(x, y, sprite, 6);
}
```
which produces the following (cleaned-up) (`-O3`) LLVM IR:
```
define dso_local void @_Z1fPh(i8* nocapture readonly %0) local_unnamed_addr #0 !dbg !185 {
  %2 = alloca [6 x i8], align 1
  %3 = getelementptr inbounds [6 x i8], [6 x i8]* %2, i64 0, i64 0, !dbg !200
  %4 = getelementptr inbounds i8, i8* %0, i64 522, !dbg !202
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* nonnull align 1 dereferenceable(6) %3, i8* nonnull align 1 dereferenceable(6) %4, i64 6, i1 false), !dbg !203
  %5 = call zeroext i1 @draw(i8 zeroext 0, i8 zeroext 0, i8* nonnull %3, i8 zeroext 6), !dbg !204
  ret void, !dbg !205
}
```