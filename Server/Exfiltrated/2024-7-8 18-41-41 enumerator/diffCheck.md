## Enumerator "Demo":
`Enumerator` is a step up from `ls_read` in that it can read directories recursively, and you can target a specific directory (granted that you have the necessary permissions to do so).
<br>
The directory structure we tried to emulate was: <br>
make.ps1 <br>
enumerator.c <br>
.gitignore <br>
Boo
  |------------------   Foo
                          | - oi.txt
  | - hi.txt
  | - sample.txt
  |------------------   Zoo
                          | - choo.txt

<br >

The fact that the Boo, Foo, and Zoo directories don't show up on Github (thanks to `.gitignore`) but they do on the `Enumerator` is pretty good proof things are working as they should.