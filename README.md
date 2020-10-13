# chip8_lifter

[![Build Status](https://travis-ci.com/antoshre/chip8_lifter.svg?branch=master)](https://travis-ci.com/antoshre/chip8_lifter)

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
The `V2` and `V3` registers are set to `0` to indicate the sprite should be drawn at `(0,0)`.

Chip8 programs are traditionally mapped starting at `0x200`, so `0x20A` corresponds to the location of the sprite data.  In this case the data is `6` bytes long.
```
examples/print_ir ../../roms/draw_space_invader.ch8
Listing:
6200: SETI	| V2 = 0x00
6300: SETI	| V3 = 0x00
A20A: ISETI	| I = 0x20a
D236: DRAW	| draw(V2, V3, 6)

; ModuleID = 'module'
source_filename = "module"

define void @f(i8* %MEM) local_unnamed_addr {
entry:
  %"&MEM[I]" = getelementptr i8, i8* %MEM, i64 522
  %0 = tail call i1 @draw(i8 0, i8 0, i8* %"&MEM[I]", i8 6)
  ret void
}

declare i1 @draw(i8, i8, i8*, i8) local_unnamed_addr

```
The resulting LLVM IR is functionally identical to the following C++ program:
```
#include <cstdint>
#include <cstring>

extern "C" bool draw(std::uint8_t, std::uint8_t, std::uint8_t*, std::uint8_t);

void f(std::uint8_t* MEM) {
    std::uint8_t x=0,y=0;
    std::uint16_t I = 0x20A;
    std::uint8_t* offset = &MEM[I];
    draw(x, y, offset, 6);
}
```
which produces the following (cleaned-up) (`-O3`) LLVM IR:
```
define dso_local void @_Z1fPh(i8* %0) local_unnamed_addr #0 !dbg !185 {
  %2 = getelementptr inbounds i8, i8* %0, i64 522, !dbg !197
  %3 = tail call zeroext i1 @draw(i8 zeroext 0, i8 zeroext 0, i8* nonnull %2, i8 zeroext 6), !dbg !198
  ret void, !dbg !199
}
```