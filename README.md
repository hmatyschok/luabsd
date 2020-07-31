<pre><code>
libluabsd - toolbox for implementing (web-based) software for (embedded) systems
================================================================================

This library provides an easy customizable interface as extension of the Lua 
(5.2.4) language  against APIs those are common Unix-like Operating Systems 
(e. g. *BSD, GNU/Linux, Minix, QNX, etc.).

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

    buf_key = bsd.sys.uio.StructIOVec(#key)
    buf_key:copy_in(key)

    buf_value = bsd.sys.uio.StructIOVec(#value)
    buf_value:copy_in(value)

    dbt_key = bsd.db.StructDBT()
    dbt_key:set_data(buf_key)

    dbt_value = bsd.db.StructDBT()
    dbt_value:set_data(buf_value)

    err, msg = db:put(dbt_key, dbt_value, bsd.db.R_NOOVERWRITE)

a callout may implemented as follows:

    fetch = true
    expired = false

    local function event()
        dbt_result = bsd.db.StructDBT()        
        db:get(dbt_key, dbt_result, 0)

        buf_result = bsd.sys.uio.StructIOVec(dbt_result:get_size())

        dbt_result:get_data(buf_result)

        db:sync(0)
        db:close()
        
        print("event:", buf_result:copy_out())

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
    
    tv = it2:get_it_value()

    print(" -> ", it2, tv, "tv_sec: " .. tv:get_tv_sec())

</code></pre>
