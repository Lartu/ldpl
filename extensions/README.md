# Important

Method declarations for the classes included in `ldpl-types.h` have been moved into the LDPL compiler
in **LDPL 4.4.2**, as having all the methods declared in this header file could potentially impede
future rewriting and optimization of these methods.

If you are writing LDPL code that depends on `ldpl-types.h` and you are getting linker errors
telling you that certain methods weren't declared, be sure to update your LDPL installation to
the latest available release.
