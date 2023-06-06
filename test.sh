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

assert 0 "0"
assert 21 "5+20-4"
assert 123 "100+20+3"
assert 41 " 12 + 34 - 5 "
assert 65 "'A'"
assert 66 "'A' + 1"
assert 131 "'A' + 'B'"
assert 12 "2 * 6"
assert 47 '5+6*7'
assert 15 '5*(9-6)'
assert 4 '(3+5)/2'
assert 90 "'A' * 'B' - 4200"
assert 0 "0==1"
assert 10 "-20 + 30"
assert 1 "1==1"
assert 1 "1*2*50==100"
assert 1 "100>99"
assert 0 "99>100"
assert 1 "99<100"
assert 0 "100<99"
assert 1 "100>=100"
assert 1 "100<=100"
assert 1 "65=='A'"

echo OK