# bd
Binary dump utility that respects your `CHAR_BIT` value and your bit numbering.
Written in a strict C99, `bd` will work on any architecture and platform.

## Introduction
The standard `od` utility does not support binary output (just like `printf`
does not have `%b`), yet it would be great to use it once in a while. The `bd`
utility fills this hole and aims at maximum flexibility and portability of the
source code.

## Usage
The `bd` utility groups bits into groups and puts a certain number of groups on
one line. Both of these values can be set by options `-g N` and `-n N`
respectively. Default values amount to `CHAR_BIT` for the group size and `8`
for the groups-per-line setting.

In case you want to skip a certain number of bits (e.g. a file format header),
use the option `-s N`.

It is possible to explicitly specify the bit numbering by using the `-l` and
`-m` options that force the *l*east and *m*ost significant bit.

## Example
### Random data
```
$ head -c 6 /dev/random | bd -g 4 -n 4
0111 0010 1101 1001
1001 1101 0101 1101
0001 1001 0000 1100
```

### Least significant 'A'
```
$ echo -n 'A' | bd -n 1 -l
10000010
```

### Most significant 'A'
```
$ echo -n 'A' | bd -n 1 -m
01000001
```

## Build & install
```
$ make
$ sudo mv ./bd /usr/bin
```

## License
2-clause BSD license. For more information please consult the
[LICENSE](LICENSE.md) file. In the case that you need a different license, feel
free to contact me.

## Author
Daniel Lovasko (daniel.lovasko@gmail.com)

