# Qodfisc

Qodfisc calculates the Italian fiscal code (*codice fiscale*) from surname, name,
place and date of birth, and sex.
The fields are laid out over a *Tessera Sanitaria*, the code is calculated while typing
and can be copied with a click.

The list of the municipalities comes from the [Anagrafe Nazionale](https://www.anagrafenazionale.interno.it/)
and includes also the ceased ones, as people born there keep their code,
and the foreign countries, under the EE province.
It can be updated with `tools/gencomuni.py > comuni.h`.

<p align="center"><img src="https://github.com/user-attachments/assets/eef9b548-c856-4ee2-b5b4-1ba0577272c2" alt="Qodfisc main window" width="902"></p>

## Build

Requirements: CMake 3.19 or newer and Qt 6 (Widgets).

```
cmake -B build
cmake --build build
```

## License

Written by Matteo Croce and Natale Vinto.

Copyright (C) 2006-2007 Matteo Croce

Qodfisc is released under the GNU General Public License version 3 or later, see [COPYING](COPYING).
