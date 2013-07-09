#! /bin/sh

rm benchmarks.txt

for f in *.lua; do
	lua $f >> benchmarks.txt 2>&1
done
