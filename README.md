# bwuhos

## Introduction

This is a small hobbyist x86_64 operating system project. Nothing complex, and
actually it won't even be usable for general purpose computing. It will be have
a basic filesystem, some utilities, a shell, a kernel, and everything will run
in kernel space.

Nothing is implemented yet.

## Dependencies

Software / system dependencies are:

* GCC with x86_64-elf target installed at /opt/cross/bin (I have successfully
  used 11.2.0 and 11.4.0)
* GNU binutils with x86_64-elf target installed at /opt/cross/bin (I have
  successfully used 2.38)
* mincbuild (for build)
* QEmu for execution with `run.sh`
* GDB for debugging when using `run.sh`

## Management

* To build bwuhos, run `./build.sh`

This will generate an ISO image which you can use the same way as any other.

## Usage

Do not run bwuhos on real hardware, it probably won't even work. And if it does,
I make no promises about how safe it is to run it. After building the project,
you should run the `run.sh` script as follows:

```
$ ./run.sh &
```

... which will spawn a process while retaining your ability to use the shell
while it runs. This is important because the script assumes that you want to use
GDB. Then, run:

```
$ gdb
```

... and continue execution with:

```
(gdb) c
```

... which will run bwuhos.

## Contributing

Do not contribute. You may use the bwuhos source as a reference when
implementing your own operating system, or for other educational purposes.
