The Pulsars / Les Pulsar.
=========================

History
-------

This little programming client/server game was created for the French programming contest Prologin, for the 2001 Final.
The game is made of a map with some terrain surface, and a certain number of robots on that terrain. There are two type
of robots, that can be controlled by each players. Each type of robot have different capacity, and each player have to
write the IA that will control his own robots.


Goal of this fork
-----------------

I making this new version for a couple of reason:
 - This game is actually quite fun to play, and don't need lots of development experience to make a simple IA
 - It could be used to make coding tournament
 - I want to make my old bot to work again!


Things going to change
----------------------

The original version allowed player to use different languages like C, C++, Java, CAML, Pascal. Support for CAML and 
Java will be dropped because of being too buggy, Pascal version is unsure to stay.

But I'm planning to add support for new language like Lua and Python, which I think are well suited for the task

The original code use GTK1 for the GUI, I will replace it to something more modern, maybe Qt5, but could also be 
web-based.

Also the autotools will be replaced by CMake.

The current server use MySQL as backend, I do think about replacing it by SQLite3, it is not necessarily more efficient,
but makes the requirement lower as installing/running the server would not need an external tool.

Will probably go too on refactoring part of the code, especially the "meta-client" and "server" as part of what should
be the on the server side is on the client side and other interesting things.

And, well, translate the game in English to allow it to be a (bit) more international!



The original repository can be find here: https://sourceforge.net/projects/prologin/ 