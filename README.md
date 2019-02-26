# picunic

Searches for similar image files in given paths. Based on a mean hashing.

## TODO
- ~~Custom application start for group of similar files~~
- dHash variant (alternative or second pass)
- pHash (second pass)
- Different backends aside from GraphicsMagick (Imagemagick, ccv, libvips etc)
- Memory sanity check
- Content-aware files reading (i.e. do not rely on file name) via libmagic

## Requirements
- POSIX (cygwin will do too)
- GraphicsMagick headers and library
- C89 compiler
- GNU Make
