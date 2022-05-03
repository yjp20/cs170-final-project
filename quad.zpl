# param N := read "dat" as "1n" comment "#";
# param D := read "dat" as "1n" skip 1 comment "#";
# param Rs := read "dat" as "1n" skip 2 comment "#";
# param Rp := read "dat" as "1n" skip 3 comment "#";

param N := 24;
param D := 30;
param Rs := 3;
param Rp := 8;

set X := { 0 .. D-1 };
set AXIS := X * X;
set GRID := { read "inputs/small/196.in" as "<1n,2n>" skip 4 comment "#"};
set NEIGHBORS[<i,j> in AXIS] := { <a,b> in AXIS with (i-a)*(i-a) + (j-b)*(j-b) <= Rp * Rp };

var sol[AXIS] binary;

minimize cost: sum <i,j> in AXIS : (141.1 * sol[i,j] + 28.9 * sol[i,j] * (sum <a,b> in NEIGHBORS[i,j] : sol[a,b]));

subto cover: forall <i,j> in GRID do
        (sum <a,b> in AXIS with (i-a) ^ 2 + (j-b) ^ 2 <= Rs * Rs : sol[a,b]) >= 1;
