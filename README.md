1. Install

    $ tar -zxvf zlog-latest-stable.tar.gz
    $ cd zlog-latest-stable/
    $ make 
    $ sudo make install

or

    $ make PREFIX=/usr/local/
    $ sudo make PREFIX=/usr/local/ install

PREFIX indicates the installation destination for zlog. After installation, refresh your dynamic linker to make sure your program can find zlog library.  

    $ sudo vi /etc/ld.so.conf
    /usr/local/lib
    $ sudo ldconfig

Before running a real program, make sure libzlog.so is in the directory where the system's dynamic lib loader can find it. The command metioned above are for linux. Other systems will need a similar set of actions.


2. Introduce configure file
-------------

There are 3 important concepts in zlog: categories, formats and rules.

Categories specify different kinds of log entries. In the zlog source code, category is a (zlog_cateogory_t *) variable. In your program, different categories for the log entries will distinguish them from each other.

Formats describe log patterns, such as: with or without time stamp, source file, source line.

Rules consist of category, level, output file (or other channel) and format. In brief, if the category string in a rule in the configuration file equals the name of a category variable in the source, then they match. Still there is complex match range of category. Rule decouples variable conditions. For example, log4j must specify a level for each logger(or inherit from father logger). That's not convenient when each grade of logger has its own level for output(child logger output at the level of debug, when father logger output at the level of error)

Now create a configuration file. The function zlog_init takes the files path as its only argument.
    $ cat /etc/zlog.conf

    [formats]
    simple = "%m%n"
    [rules]
    my_cat.DEBUG    >stdout; simple

In the configuration file log messages in the category "my_cat" and a level of DEBUG or higher are output to standard output, with the format of simple(%m - usermessage %n - newline). If you want to direct out to a file and limit the files maximum size, use this configuration

    my_cat.DEBUG            "/var/log/aa.log", 1M; simple

-------------
    $ cc -c -o test_hello.o test_hello.c -I/usr/local/include
    $ cc -o test_hello test_hello.o -L/usr/local/lib -lzlog -lpthread
    $ ./test_hello
    hello, zlog


