#! /bin/sh

rm benchmarks.txt

for f in *.lua; do
	lua $f >> benchmarks.txt
done
