swizzle_1<3, 0> x, r;
swizzle_1<3, 1> y, g;
swizzle_1<3, 2> z, b;

swizzle_2<3, 0, 0> xx, rr;
swizzle_2<3, 0, 1> xy, rg;
swizzle_2<3, 0, 2> xz, rb;
swizzle_2<3, 1, 0> yx, gr;
swizzle_2<3, 1, 1> yy, gg;
swizzle_2<3, 1, 2> yz, gb;
swizzle_2<3, 2, 0> zx, br;
swizzle_2<3, 2, 1> zy, bg;
swizzle_2<3, 2, 2> zz, bb;

swizzle_3<3, 0, 0, 0> xxx, rrr;
swizzle_3<3, 0, 0, 1> xxy, rrg;
swizzle_3<3, 0, 0, 2> xxz, rrb;
swizzle_3<3, 0, 1, 0> xyx, rgr;
swizzle_3<3, 0, 1, 1> xyy, rgg;
swizzle_3<3, 0, 1, 2> xyz, rgb;
swizzle_3<3, 0, 2, 0> xzx, rbr;
swizzle_3<3, 0, 2, 1> xzy, rbg;
swizzle_3<3, 0, 2, 2> xzz, rbb;
swizzle_3<3, 1, 0, 0> yxx, grr;
swizzle_3<3, 1, 0, 1> yxy, grg;
swizzle_3<3, 1, 0, 2> yxz, grb;
swizzle_3<3, 1, 1, 0> yyx, ggr;
swizzle_3<3, 1, 1, 1> yyy, ggg;
swizzle_3<3, 1, 1, 2> yyz, ggb;
swizzle_3<3, 1, 2, 0> yzx, gbr;
swizzle_3<3, 1, 2, 1> yzy, gbg;
swizzle_3<3, 1, 2, 2> yzz, gbb;
swizzle_3<3, 2, 0, 0> zxx, brr;
swizzle_3<3, 2, 0, 1> zxy, brg;
swizzle_3<3, 2, 0, 2> zxz, brb;
swizzle_3<3, 2, 1, 0> zyx, bgr;
swizzle_3<3, 2, 1, 1> zyy, bgg;
swizzle_3<3, 2, 1, 2> zyz, bgb;
swizzle_3<3, 2, 2, 0> zzx, bbr;
swizzle_3<3, 2, 2, 1> zzy, bbg;
swizzle_3<3, 2, 2, 2> zzz, bbb;

swizzle_4<3, 0, 0, 0, 0> xxxx, rrrr;
swizzle_4<3, 0, 0, 0, 1> xxxy, rrrg;
swizzle_4<3, 0, 0, 0, 2> xxxz, rrrb;
swizzle_4<3, 0, 0, 1, 0> xxyx, rrgr;
swizzle_4<3, 0, 0, 1, 1> xxyy, rrgg;
swizzle_4<3, 0, 0, 1, 2> xxyz, rrgb;
swizzle_4<3, 0, 0, 2, 0> xxzx, rrbr;
swizzle_4<3, 0, 0, 2, 1> xxzy, rrbg;
swizzle_4<3, 0, 0, 2, 2> xxzz, rrbb;
swizzle_4<3, 0, 1, 0, 0> xyxx, rgrr;
swizzle_4<3, 0, 1, 0, 1> xyxy, rgrg;
swizzle_4<3, 0, 1, 0, 2> xyxz, rgrb;
swizzle_4<3, 0, 1, 1, 0> xyyx, rggr;
swizzle_4<3, 0, 1, 1, 1> xyyy, rggg;
swizzle_4<3, 0, 1, 1, 2> xyyz, rggb;
swizzle_4<3, 0, 1, 2, 0> xyzx, rgbr;
swizzle_4<3, 0, 1, 2, 1> xyzy, rgbg;
swizzle_4<3, 0, 1, 2, 2> xyzz, rgbb;
swizzle_4<3, 0, 2, 0, 0> xzxx, rbrr;
swizzle_4<3, 0, 2, 0, 1> xzxy, rbrg;
swizzle_4<3, 0, 2, 0, 2> xzxz, rbrb;
swizzle_4<3, 0, 2, 1, 0> xzyx, rbgr;
swizzle_4<3, 0, 2, 1, 1> xzyy, rbgg;
swizzle_4<3, 0, 2, 1, 2> xzyz, rbgb;
swizzle_4<3, 0, 2, 2, 0> xzzx, rbbr;
swizzle_4<3, 0, 2, 2, 1> xzzy, rbbg;
swizzle_4<3, 0, 2, 2, 2> xzzz, rbbb;
swizzle_4<3, 1, 0, 0, 0> yxxx, grrr;
swizzle_4<3, 1, 0, 0, 1> yxxy, grrg;
swizzle_4<3, 1, 0, 0, 2> yxxz, grrb;
swizzle_4<3, 1, 0, 1, 0> yxyx, grgr;
swizzle_4<3, 1, 0, 1, 1> yxyy, grgg;
swizzle_4<3, 1, 0, 1, 2> yxyz, grgb;
swizzle_4<3, 1, 0, 2, 0> yxzx, grbr;
swizzle_4<3, 1, 0, 2, 1> yxzy, grbg;
swizzle_4<3, 1, 0, 2, 2> yxzz, grbb;
swizzle_4<3, 1, 1, 0, 0> yyxx, ggrr;
swizzle_4<3, 1, 1, 0, 1> yyxy, ggrg;
swizzle_4<3, 1, 1, 0, 2> yyxz, ggrb;
swizzle_4<3, 1, 1, 1, 0> yyyx, gggr;
swizzle_4<3, 1, 1, 1, 1> yyyy, gggg;
swizzle_4<3, 1, 1, 1, 2> yyyz, gggb;
swizzle_4<3, 1, 1, 2, 0> yyzx, ggbr;
swizzle_4<3, 1, 1, 2, 1> yyzy, ggbg;
swizzle_4<3, 1, 1, 2, 2> yyzz, ggbb;
swizzle_4<3, 1, 2, 0, 0> yzxx, gbrr;
swizzle_4<3, 1, 2, 0, 1> yzxy, gbrg;
swizzle_4<3, 1, 2, 0, 2> yzxz, gbrb;
swizzle_4<3, 1, 2, 1, 0> yzyx, gbgr;
swizzle_4<3, 1, 2, 1, 1> yzyy, gbgg;
swizzle_4<3, 1, 2, 1, 2> yzyz, gbgb;
swizzle_4<3, 1, 2, 2, 0> yzzx, gbbr;
swizzle_4<3, 1, 2, 2, 1> yzzy, gbbg;
swizzle_4<3, 1, 2, 2, 2> yzzz, gbbb;
swizzle_4<3, 2, 0, 0, 0> zxxx, brrr;
swizzle_4<3, 2, 0, 0, 1> zxxy, brrg;
swizzle_4<3, 2, 0, 0, 2> zxxz, brrb;
swizzle_4<3, 2, 0, 1, 0> zxyx, brgr;
swizzle_4<3, 2, 0, 1, 1> zxyy, brgg;
swizzle_4<3, 2, 0, 1, 2> zxyz, brgb;
swizzle_4<3, 2, 0, 2, 0> zxzx, brbr;
swizzle_4<3, 2, 0, 2, 1> zxzy, brbg;
swizzle_4<3, 2, 0, 2, 2> zxzz, brbb;
swizzle_4<3, 2, 1, 0, 0> zyxx, bgrr;
swizzle_4<3, 2, 1, 0, 1> zyxy, bgrg;
swizzle_4<3, 2, 1, 0, 2> zyxz, bgrb;
swizzle_4<3, 2, 1, 1, 0> zyyx, bggr;
swizzle_4<3, 2, 1, 1, 1> zyyy, bggg;
swizzle_4<3, 2, 1, 1, 2> zyyz, bggb;
swizzle_4<3, 2, 1, 2, 0> zyzx, bgbr;
swizzle_4<3, 2, 1, 2, 1> zyzy, bgbg;
swizzle_4<3, 2, 1, 2, 2> zyzz, bgbb;
swizzle_4<3, 2, 2, 0, 0> zzxx, bbrr;
swizzle_4<3, 2, 2, 0, 1> zzxy, bbrg;
swizzle_4<3, 2, 2, 0, 2> zzxz, bbrb;
swizzle_4<3, 2, 2, 1, 0> zzyx, bbgr;
swizzle_4<3, 2, 2, 1, 1> zzyy, bbgg;
swizzle_4<3, 2, 2, 1, 2> zzyz, bbgb;
swizzle_4<3, 2, 2, 2, 0> zzzx, bbbr;
swizzle_4<3, 2, 2, 2, 1> zzzy, bbbg;
swizzle_4<3, 2, 2, 2, 2> zzzz, bbbb;