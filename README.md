<pre><code>
libluabsd
=========

This library provides an easy customizable interface against on arc4random(3), 
db(3) and uuidgen(2) components. As an example, this interface may used as 
described in db(3) or dbopen(3): 


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

creates or opens a database

    local db = lib.dbopen(_fname, _flags, _mode, _type)

data are stored 

    local key = lib.uuidgen()
    local value = "Hello world!"
    
    db:put(key, value, lib.db_r_flags.R_NOOVERWRITE)
    
or retrieved by

    local record, err = db:get(key, 0)

and so on.

</code></pre>
