<pre><code>
libluabsd - toolbox for implementing (web-based) software for (embedded) systems
================================================================================

This library provides an easy customizable interface as extension of Lua
language against APIs those are common on unixoid Operating Systems (e. g.
the FreeBSD OS, QNX, etc.).

As an example, a database may created, as described in db(3):

    local bsd = require("bsd")

    local _fname = "example.db"
    local _flags = bit32.bor(
        bsd.fcntl.O_CREAT,
        bsd.fcntl.O_TRUNC,
        bsd.fcntl.O_RDWR
    )
    local _mode = bit32.bor(
        bsd.sys.stat.S_IRUSR,
        bsd.sys.stat.S_IWUSR,
        bsd.sys.stat.S_IRGRP,
        bsd.sys.stat.S_IWGRP,
        bsd.sys.stat.S_IROTH,
        bsd.sys.stat.S_IWOTH
    )
    local _type = bsd.db.DB_BTREE

    db = bsd.db.dbopen(_fname, _flags, _mode, _type)

Therefore

    local key = bsd.uuid.uuidgen()
    local value = "Hello world!"

    db:put(key, value, bsd.db.R_NOOVERWRITE)

    err, record = db:get(key, 0)

and so on. A callout may implemented as follows:

    fetch = true
    expired = false

    local function event()
        print("Hello world!")
        expired = true;
    end

    tv = bsd.sys.time.StructTimeSpec()
    tv:set_tv_sec(3)

    it1 = bsd.sys.time.StructItimerVal()
    it1:set_it_value(tv)

    it2 = bsd.sys.time.StructItimerVal()

    err, msg = bsd.sys.time.setitimer(bsd.sys.time.ITIMER_REAL, it1, nil, event)

    while true do -- do something
        if expired then
            break
        end

        if fetch then
            err, msg = bsd.sys.time.getitimer(bsd.sys.time.ITIMER_REAL, it2)
            fetch = false
        end
    end

    print("----------")

    print(it2)

    for k, v in pairs(it2:get()) do
        print(" ", k, "tv_sec  : " .. v:get_tv_sec())
        print(" ", k, "tv_nsec : " .. v:get_tv_nsec())
    end

</code></pre>
