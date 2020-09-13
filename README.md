<pre><code>
libluabsd - toolbox for implementing (web-based) software for (embedded) systems
================================================================================

This library provides an easy customizable interface as extension of the Lua
(5.2.4) language against APIs those are common on Unix-like Operating Systems,
e. g. *BSD, GNU/Linux, Minix, QNX, etc.

    local bsd = require("bsd")

As an example, a database may be created as described in db(3) [1]:

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

    local db = bsd.db.dbopen(_fname, _flags, _mode, _type)

A key/value pair

    local key = bsd.core.uuid()
    local value = "Hello world!"

may be created by utilizing

    local buf_key = bsd.sys.uio.iovec_create(#key)
    local buf_value = bsd.sys.uio.iovec_create(#value)

    buf_key:copy_in(key)
    buf_value:copy_in(value)

buffer maps to a set of data base thang

    local dbt_key = bsd.db.dbt_create(buf_key)
    local dbt_value = bsd.db.dbt_create(buf_value)
    local dbt_result = bsd.db.dbt_create()

still implements an interface for operations as described in db(3):

    _flags = bsd.db.R_NOOVERWRITE

    local ret, err, msg = db:put(dbt_key, dbt_value, _flags)

Therefore, a callout may be implemented e. g.

    local fetch = true
    local expired = false

    local function event()

        ret, err, msg = db:get(dbt_key, dbt_result, 0)

        buf_result = bsd.sys.uio.iovec_create(dbt_result:get_size())
        dbt_result:get_data(buf_result)

        local k, v = buf_key:copy_out(), buf_result:copy_out()

        print("event:", "(k,v) =", "(" .. k .. "," .. v .. ")\n")

        expired = true;
    end

by utilizing setitimer(2) or interval timer [2]:

    local tv = bsd.sys.time.timeval_create()
    tv:set_tv_sec(3)

    local it_callout = bsd.sys.time.itimerval_create()
    it_callout:set_it_value(tv)

    local it_probe = bsd.sys.time.itimerval_create()
    local timer = bsd.sys.time.ITIMER_REAL

    ret, err, msg = bsd.sys.time.setitimer(timer, it_callout, nil, event)

    while true do -- do something
        if expired then
            break
        end

        if fetch then
            ret, err, msg = bsd.sys.time.getitimer(timer, it_probe)
            fetch = false
        end
    end

It is obvious, C structures are encapsulated by instances of LUA_TUSERDATA:

    local mt = getmetatable(it_probe);

    table.sort(mt)

    for k, v in pairs(mt) do
        print("", k, v)
    end
    print("")

Those are accessible through get/set routines:

    tv = it_probe:get_it_value()

    print(" -> ", it_probe, tv, "tv_sec: " .. tv:get_tv_sec(), "\n")

As mentioned before, C Structures are accessible

    function print_udata(i, j, pfx)

        local k = nil

        if j.get ~= nil then
            k = j:get()
        end

        print(pfx, i, j)
        pfx = pfx .. "\t"

        if type(k) == "table" then
            print_table(k, pfx)
        end
    end

by

    function print_table(t, pfx)

        table.sort(t)

        for k, v in pairs(t) do
            if type(v) == "userdata" then
                print_udata(k, v, pfx)
            else
                print(pfx, k, v)

                if type(v) == "table" then
                    for p, q in pairs(v) do
                        print_udata(pfx .. p, q, pfx)
                    end
                end
            end
        end
        print("")
    end

utilizing instaces of LUA_TTABLES:

    local sb = bsd.sys.stat.stat_create()
    local fd = db:fd()

    ret, err, msg = bsd.sys.stat.fstat(fd, sb)

    ret, err, msg = db:sync(0)
    ret, err, msg = db:close()

    print_udata("struct", sb, "")

    _fname = "example.file"
    _flags = bit32.bor(
        bsd.fcntl.O_CREAT,
        bsd.fcntl.O_TRUNC,
        bsd.fcntl.O_RDWR
    )
    fd, err, msg = bsd.fcntl.open(_fname, _flags, _mode)

    print(" open() ", fd, err, msg, "\n")

    local tv = sb:get_st_atim()
    print_udata("struct", tv, "")

    local tv_buf = tv:dump()

    print_udata("tv_buf", tv_buf, "")

    print(" write(2) :", bsd.unistd.write(fd, tv_buf, tv_buf:len()))
    print(" close(2) :", bsd.unistd.close(fd), "\n")

    _flags = bit32.bor(
        bsd.fcntl.O_RDWR
    )
    fd, err, msg = bsd.fcntl.open(_fname, _flags, _mode)

    local data_buf = bsd.sys.uio.iovec_create(tv_buf:len())

    print(" read(2) :", bsd.unistd.read(fd, data_buf, tv_buf:len()))
    print(" close(2) :", bsd.unistd.close(fd), "\n")

    print_udata("data_buf", data_buf, "")

    local tv_new = bsd.sys.time.timespec_create(data_buf)

    print_udata("struct", tv_new, "")

References
----------

 [1] FreeBSD Library Functions Manual,
      "dbopen -- database access methods",
      https://www.freebsd.org/cgi/man.cgi?db(3)

 [2] FreeBSD System Calls Manual,
      "getitimer, setitimer -- get/set value of interval timer",
      https://www.freebsd.org/cgi/man.cgi?setitimer(2)

</code></pre>
