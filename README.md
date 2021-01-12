<pre><code>
libluabsd - interface for implementing (e. g. web-based) software or rc-scripting
=================================================================================

This library provides an easy customizable interface as extension for the Lua
(5.2.4) language against APIs those are common on Unix-like Operating Systems,
e. g. *BSD, GNU/Linux, Minix, QNX, etc.

    local bsd = require("bsd")

As an example, a database may be created as described in db(3) [1]:

    local fd, ret, err, msg
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

    local db, err, msg = bsd.db.dbopen(_fname, _flags, _mode, _type)
    print(string.format(" dbopen(%s,%d,%d,%d)",
        _fname, _flags, _mode, _type), err, msg, "\n")

A key/value pair

    local key = bsd.core.uuid()
    local value = "Hello world!"

may be created by utilizing

    local buf_key = bsd.sys.uio.create_iovec(#key)
    local buf_value = bsd.sys.uio.create_iovec(#value)

    buf_key:copy_in(key)
    buf_value:copy_in(value)

buffer maps to a set of (so called) data base thang

    local dbt_key = bsd.db.create_dbt(buf_key)
    local dbt_value = bsd.db.create_dbt(buf_value)
    local dbt_result = bsd.db.create_dbt()

still implements an interface for operations as described in db(3):

    _flags = bsd.db.R_NOOVERWRITE

    ret, err, msg = db:put(dbt_key, dbt_value, _flags)

Therefore, a callout may be implemented e. g.

    local fetch = true
    local expired = false

    local function event()

        ret, err, msg = db:get(dbt_key, dbt_result, 0)

        buf_result = bsd.sys.uio.create_iovec(dbt_result:get_size())
        dbt_result:get_data(buf_result)

        local k, v = buf_key:copy_out(), buf_result:copy_out()

        print(" event:", string.format("(k,v) = (%s,%s)\n", k, v))

        expired = true;
    end

by utilizing setitimer(2) or interval timer API [2]:

    local tv = bsd.sys.time.create_itimerval()
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

    print(" -> ", it_probe, tv, string.format("tv_sec: %s\n", tv:get_tv_sec()))

As mentioned before, C Structures are e. g. accessible

    function print_udata(i, j, pfx)

        local function _print_table(_t, _pfx)

            table.sort(_t)

            for k, v in pairs(_t) do
                if type(v) == "userdata" then
                    print_udata(k, v, _pfx)
                else
                    print(_pfx, k, v)

                    if type(v) == "table" then
                        for p, q in pairs(v) do
                            print_udata(_pfx .. p, q, _pfx)
                        end
                    end
                end
            end
            print("")
        end
        local k = nil

        if type(j) == "userdata" then
            k = j:get_table()
        else
            k = j
        end

        print(pfx, i, j)
        pfx = pfx .. "\t"

        if type(k) == "table" then
            _print_table(k, pfx)
        end
    end

by utilizing instances of LUA_TTABLES:

    local sb = bsd.sys.stat.create_stat()
    fd, err, msg = db:fd()
    print(" db:fd()", fd, err, msg, "\n")

    ret, err, msg = bsd.sys.stat.fstat(fd, sb)
    print(string.format(" fstat(%d)", fd), ret, err, msg, "\n")

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
    print(string.format(" open(%s,%d,%d)", _fname, _flags, _mode),
        fd, err, msg, "\n")

    local tv = sb:get_st_atim()
    print_udata("struct", tv, "")

    local tv_buf = tv:dump()
    print_udata("struct", tv_buf, "")

    local off = tv_buf:get_len()
    ret, err, msg = bsd.unistd.write(fd, tv_buf, off)
    print(string.format(" write(%d,%d)", fd, off), ret, err, msg, "\n")

    local whence = bsd.sys.unistd.SEEK_CUR
    ret, err, msg = bsd.unistd.lseek(fd, -off, whence)
    print(string.format(" lseek(%d,%d,%d)", fd, -off, whence), ret, err, msg, "\n")

    local data_buf = bsd.sys.uio.create_iovec(off)
    ret, err, msg = data_buf:read(fd)
    print(string.format(" data_buf:read(%d)", fd), ret, err, msg, "\n")

    ret, err, msg = bsd.unistd.close(fd)
    print(string.format(" close(%d)", fd), ret, err, msg, "\n")

    print_udata("struct", data_buf, "")

    local tv_new = bsd.sys.time.create_timespec(data_buf)

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
