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

# Common part number used in all experiments
proc com_set_part {} {
    set_part  {xczu7ev-ffvf1517-3-e}
}
