#!/bin/bash
assert() {
    expected="$1"
    input="$2"

    ./9cc "$input" > tmp.s
    cc -o tmp tmp.s
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

assert 0 "0;"
assert 21 "5+20-4;"
assert 123 "100+20+3;"
assert 41 " 12 + 34 - 5 ;"
assert 65 "'A';"
assert 66 "'A' + 1;"
assert 131 "'A' + 'B';"
assert 12 "2 * 6;"
assert 47 '5+6*7;'
assert 15 '5*(9-6);'
assert 4 '(3+5)/2;'
assert 90 "'A' * 'B' - 4200;"
assert 0 "0==1;"
assert 10 "-20 + 30;"
assert 1 "1==1;"
assert 1 "1*2*50==100;"
assert 1 "100>99;"
assert 0 "99>100;"
assert 1 "99<100;"
assert 0 "100<99;"
assert 1 "100>=100;"
assert 1 "100<=100;"
assert 1 "65=='A';"
assert 65 "a='A';a;"
assert 120 "a_aa=3; bbbb=80; return a_aa * bbbb/2; "
assert 120 "a=7;b=6;if(a>b) return 120; return 0;"
assert 0 "a=5;b=6;if(a>b) return 120; return 0;"
assert 120 "a=7;b=6;if(a>b) return 120; return 0;"
assert 0 "a=5;b=6;if(a>b) return 120; return 0;"
assert 120 "a=7;b=6;if(a>b) return 120; else return 0;"
assert 0 "a=5;b=6;if(a>b) return 120;else return 0;"
assert 50 "a=5;b=6;if(a>b) return 120;if(a<b) return 50;"
assert 128 "a=b=2;while(100>a)a=a*b;return a;"
assert 32 "a=2;for(i=0;4>i;i=i+1)a=a*2;return a;"
assert 10 "a=0;for(i=0;i<10;i=i+1)a=a+1;return a;"
assert 55 "a=0;b=1;for(i=0;i<10;i=i+1){c=a+b;a=b;b=c;}return a;"
assert 144 "a=0;b=1;for(i=0;i<12;i=i+1){c=a+b;a=b;b=c;}return a;"

echo OK