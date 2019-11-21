; RUN: not test -f %llvmshlibdir/libBye%shlibext || opt %s -load=%llvmshlibdir/libBye%shlibext -goodbye -wave-goodbye \
; RUN:   -disable-output 2>&1 | grep Bye
; REQUIRES: plugins

@junk = global i32 0

define i32* @somefunk() {
  ret i32* @junk
}
