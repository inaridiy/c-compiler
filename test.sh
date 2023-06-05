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

echo OK