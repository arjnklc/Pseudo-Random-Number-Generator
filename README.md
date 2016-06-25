# Pseudo-Random-Number-Generator

This program generates pseudo-random number with a high entropy with a seed. It uses Yarrow function of libtomcrypt crypto library. As a seed, it gets random bytes from /dev/urandom and clock cycles since processor was powered on.

# Usage

Since it uses /dev/urandom , it only works on linux for now. Also it needs libtomcrypt to be compiled. So first, you have to install libtomcrypt. It also uses some features of C++11. So when you compile it, you have to tell to the compiler. After compiling, you can run the program without arguments.

Installing libtomcrypt library
<pre>
$ sudo apt-get install libtomcrypt-dev
</pre>

Compiling random.cpp file
<pre>
$ g++ random.cpp -std=c++11 -ltomcrypt
</pre>


