F.A.Q.
======

What is not included?
---------------------

### A.I.

No intelligence included. There is currently no single A.I. that can control 
any type of character in any type of situation.  Y.A.R.S. deos not even try 
to solve this problem.

### Dialog

Dialog Systems are varied and the one the works best depends on the type of 
game.

### Pathfinding

A character moving from Point A to Point B is solely based on the game world.  
Y.A.R.S. is completely separated from all game worlds, so that it can work in 
any game world.

### Quests

A quest is something a character does and is not a part of a character.


Why not just use a JSON library?
--------------------------------

All the `to_string()` methods return a JSON formated string.  The reason for 
creating these strings manually instead of using a JSON library is:

1. It would add a dependency, this project is supposed to be dependency free.
2. JSON Strings are easy to read and all JSON libraries can convert them if 
   there is that need.
3. Most importantly, which one?  There are many, each with different pros and 
   cons.  Plus, none are API compatable, so picking one removes the possibility 
   of using the others.  However, one area where all JSON libraries agree is 
   JSON formatted strings.


What are all the "// {{{" and "// }}}" in the code?
---------------------------------------------------

These are Vim Markers and they make folding very easy in addition to being a 
nice visual indicator for grouping code.  If you are using Vim, the command 
`:set fdm=marker` to enabled this style of folding.  Now when you are on a 
collapsed fold or in an expanded fold, use `za` in Command-Mode to toggle the 
open/close state of the fold.  Using this technique allows for another method 
of quick movement in a file.

This is not Object-Oriented.  This is a mess!
-------------------------------------------------

There is an old phrase, "Ugly is in the eye of the beholder". Or something 
like that.

But to address the why _OO Design Principles_ were not used, the intention 
was to be more of a _Data Oriented Design_ and use C++ classes and namespaces 
to control data access. The data is grouped into "how it is accessed" 
containers.  The Yars class manages what goes into the data containers and 
what is removed.  The Yars class also provides direct access to the data 
containers and methods that make updating the data consistant and easy.

All data is available for external changes to allow customization of \yars 
without modifications to the Yars class.
