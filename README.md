<pre><code>
libluabsd - toolbox for implementing (web-based) software for (embedded) systems
================================================================================

This library provides an easy customizable interface as extension of the Lua 
(5.2.4) language against APIs those are common on Unix-like Operating Systems,
e. g. *BSD, GNU/Linux, Minix, QNX, etc.

    local bsd = require("bsd")

As an example, a database may creat,

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

as described in db(3):

    db = bsd.db.dbopen(_fname, _flags, _mode, _type)

A key / value pair 

    local key = bsd.core.uuidgen()
    local value = "Hello world!"

may created by utilizing

    local buf_key = bsd.sys.uio.StructIOVec(#key)
    local buf_value = bsd.sys.uio.StructIOVec(#value)

    buf_key:copy_in(key)
    buf_value:copy_in(value)

buffer maps to

    local dbt_key = bsd.db.StructDBT(buf_key)
    local dbt_value = bsd.db.StructDBT(buf_value)

a set of data base thang

    local dbt_result = bsd.db.StructDBT()

those implements proxy pattern for operations on db(3):

    _flags = bsd.db.R_NOOVERWRITE

    ret, err, msg = db:put(dbt_key, dbt_value, _flags)

Therefore, a callout may implemented as follows:

    fetch = true
    expired = false

    local function event()
    
        ret, err, msg = db:get(dbt_key, dbt_result, 0)

        buf_result = bsd.sys.uio.StructIOVec(dbt_result:get_size())
        dbt_result:get_data(buf_result)

        print("event:", buf_result:copy_out())

        expired = true;
    end

by utilizing setitimer(2) API:

    local timer = bsd.sys.time.ITIMER_REAL
    
    local tv = bsd.sys.time.StructTimeSpec()
    local it_callout = bsd.sys.time.StructItimerVal()

    tv:set_tv_sec(3)

    it_callout:set_it_value(tv)

    ret, err, msg = bsd.sys.time.setitimer(timer, it_callout, nil, event)

where

    local it_probe = bsd.sys.time.StructItimerVal()

inspects during

    while true do -- do something
        if expired then
            break
        end

        if fetch then
            ret, err, msg = bsd.sys.time.getitimer(timer, it_probe)
            fetch = false
        end
    end

It is obvious,  

    local mt = getmetatable(it_probe);

    table.sort(mt)

    for k, v in pairs(mt) do
        print("", k, v)
    end

parametrical information are encapsulated by instances of LUA_TUSERDATA.
Therefore, its properties are accessible through get / set routines:

    tv = it_probe:get_it_value()

    print(" -> ", it_probe, tv, "tv_sec: " .. tv:get_tv_sec())

But on the other hand, data (e. g. maps to stat{}, see sys/stat.h)

    local sb = bsd.sys.stat.StructStat()
    local fd = db:fd()

    ret, err, msg = bsd.sys.stat.fstat(fd, sb)

    ret, err, msg = db:sync(0)
    ret, err, msg = db:close()

is accessible by 

    function print_udata(i, j, pfx)

        print(pfx, i, j)

        pfx = pfx .. "\t"

        if j.get ~= nil then
            local t = j:get()

            table.sort(t)

            for k, v in pairs(t) do
                if type(v) == "userdata" then
                    print_udata(k, v, pfx)
                    print("")
                else
                    print(pfx, k, v)
                end
            end
        end
    end

utilizing

    print_udata("struct", sb, "")

tables.
</code></pre>
