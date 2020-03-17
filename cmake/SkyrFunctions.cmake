function(remove_extension file_name basename)
    string(REGEX REPLACE "\\.[^.]*$" "" _basename ${file_name})
    set(${basename} ${_basename} PARENT_SCOPE)
endfunction()
