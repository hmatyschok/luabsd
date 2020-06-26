<pre><code>
libluabsd - toolbox for implementing web-based software for embedded systems
============================================================================

This library provides an easy customizable interface as extension of Lua
language against APIs those are common at the FreeBSD Operating System:

  o arc4random(3),
  o db(3),
  o fork(2),
  o getpid(2),
  o getppid(2),
  o getpgid(2),
  o getpgrp(2),
  o getsid(2),
  o setpgid(2),
  o setpgrp(2),
  o setsid(2).,
  o setitimer(2),
  o uuidgen(2).

As an example, a database may created, as described in db(3):

    local lib = require("bsd")

    local _fname = "example.db"
    local _flags = bit32.bor(
        lib.f_o_flags.O_CREAT,
        lib.f_o_flags.O_TRUNC,
        lib.f_o_flags.O_RDWR
    )
    local _mode = bit32.bor(
        lib.f_s_flags.S_IRUSR,
        lib.f_s_flags.S_IWUSR,
        lib.f_s_flags.S_IRGRP,
        lib.f_s_flags.S_IWGRP,
        lib.f_s_flags.S_IROTH,
        lib.f_s_flags.S_IWOTH
    )
    local _type = lib.db_type.DB_BTREE

    local db = lib.dbopen(_fname, _flags, _mode, _type)

Therefore

    local key = lib.uuidgen()
    local value = "Hello world!"

    db:put(key, value, lib.db_r_flags.R_NOOVERWRITE)

    local err, record = db:get(key, 0)

and so on. A callout may implemented as follows:

    local expired = false

    local function callback ()
        expired = true
        print("Hello world!")
    end

    lib.setitimer(lib.i_timer.ITIMER_REAL, 3, callback)

    while true do -- yes, this is bad style ^^
        if expired then
            break
        end
    end

</code></pre>
