proc com_assert_in {name names {msg ""}} {
    if {[lsearch -exact $names $name] == -1} {
        if {$msg != ""} {
            puts "$msg"
        } else {
            puts "ERROR: \"$name\" not in {$names}"
        }
        exit 1
    }
}
