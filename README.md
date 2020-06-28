<pre><code>
libluabsd - toolbox for implementing web-based software for embedded systems
============================================================================

This library provides an easy customizable interface as extension of Lua
language against APIs those are common at the FreeBSD Operating System.

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

    local db = lib.db.dbopen(_fname, _flags, _mode, _type)

Therefore

    local key = lib.uuid.uuidgen()
    local value = "Hello world!"

    db:put(key, value, lib.db_r_flags.R_NOOVERWRITE)

    local err, record = db:get(key, 0)

and so on. A callout may implemented as follows:

    local expired = false

    local function callback ()
        expired = true
        print("Hello world!")
    end

    lib.time.setitimer(lib.i_timer.ITIMER_REAL, 3, callback)

    while true do -- yes, this is bad style ^^
        if expired then
            break
        end
    end

</code></pre>
