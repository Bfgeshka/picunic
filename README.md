# picunic

Searches for similar image files in given paths. Based on a mean hashing.

## Tricks
```
# Help yourself
$ picunic -h

# Will open sxiv in thumbnail mode, showing all pictures in group at once.
$ picunic -T /path -c "sxiv -t"

# Print all groups to file, one group per line. Save output log in separate file.
$ picunic -T /path -c "echo" 1>/list.txt 2>/file.log
```

## TODO
- ~~Custom application start for group of similar files~~
- ~~dHash variant (alternative or second pass)~~
- Precise second pass
- ~~Different backends aside from GraphicsMagick (Imagemagick, ccv, libvips etc)~~
- ~~Memory sanity check~~
- Content-aware files reading (i.e. do not rely on file name) via libmagic
- Multithreaded parsing
- Visualize progress

## Requirements
- POSIX (cygwin will do too)
- C compiler
- GNU Make

- Optional:
	- GraphicsMagick headers and library
