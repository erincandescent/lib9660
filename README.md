# lib9660 - a simple implementation of the ISO 9660 file system

lib9660 is a simple implementation of the ISO 9660 file system, as used on CD.
It does not perform any dynamic memory allocations or depend upon any 
complicated standard library routines (actually all it requires is memcmp and
memcpy). It provides a reasonable facsimilie of the standard POSIX file 
descriptor I/O API. 

lib9660 is re-entrant; it is safe to use it from multiple threads, provided you
do not access the same file from multiple threads (note that for these purposes
the openat and opendirat functions count as accessing the passed parent file)

## Usage notes
l9660_dir is simply a wrapper for l9660_file. You may use them interchangably if
necessary to save memory (note that this will require casting). The distinct
types are provided purely for type checking.

You can pass the same file/directory pointer for both child and parent 
parameters of l9660_open(dir)at. This can be useful in memory constrained 
environments.

l9660_read will not perform a read from the underlying device as long as the
request can be answered from its' internal buffer. Be prepared for short reads.

## Tuning parameters
### L9660_SINGLEBUFFER
By default, lib9660 maintains a buffer per open file. This means each file (and 
directory) consumes just over 2kB of RAM for the buffer and state information.

If you are memory constrained, you can build with L9660_SINGLEBUFFER (make sure
it is also defined in any files which include lib9660.h), which causes lib9660
to instead use a single common buffer. lib9660 will now consume a constant
2kB of RAM for this buffer, instead of 2kB per file.

The cost of this is slightly reduced performance; if you alternate between 
reading multiple files, then each time you change file that file's buffer must
be reloaded. Additionally, this makes lib9660 no longer thread safe.

### L9660_HAVE_STDIO
If your system has stdio.h and it defines SEEK_[SET,CUR,END], then lib9660 will
use those definitions for l9660_seek. Otherwise, it will use its' own 
definitions of L9660_SEEK_[SET,CUR,END].

### DEBUG
If you build l9660.c with DEBUG defined, then this will enable some library 
debugging features. Most notably, if L9660_DEBUG is set in the environment,
then all directory entries processed (by the open[dir]at functions) will be
dumped to stdout.

This requires a functioning printf and getenv. You probably don't need it