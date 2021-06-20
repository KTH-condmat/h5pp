cmake_minimum_required(VERSION 3.15)

function(expand_target_libs target_names expanded_list)
    unset(target_expanded)
    unset(target_names_expanded)
    foreach(lib ${target_names})
        #        message("Checking if ${lib} is a target")
        if(TARGET ${lib})
            unset(interface_libs)
            unset(imported_lib)
            #            message("Expanding target ${lib}")
            get_target_property(lib_type  ${lib} TYPE)
            if(NOT "${lib_type}" MATCHES "INTERFACE")
                get_target_property(imported_lib  ${lib} LOCATION)
            endif()
            get_target_property(interface_libs  ${lib} INTERFACE_LINK_LIBRARIES)
            # Now we have all the libs in the target "lib".
            # The imported lib is obviously not a target, but interface_libs may contain
            # many other targets. That's ok, for now we only do one level of expansion.
            if(imported_lib)
                list(APPEND target_expanded_${lib}  ${imported_lib})
            endif()
            if(interface_libs)
                list(APPEND target_expanded_${lib}  ${interface_libs})
            endif()
            list(APPEND target_expanded ${lib})
            #            message("Expanded target ${lib} into ${target_expanded_${lib}}")
        endif()
    endforeach()
    if(target_expanded)
        list(REMOVE_DUPLICATES target_expanded)
    endif()
    #    message(STATUS "target names : ${target_names}")
    #    foreach(lib ${target_expanded})
    #        message(STATUS "target expanded [${lib}]: ${target_expanded_${lib}}")
    #    endforeach()
    foreach(tgt ${target_names})
        set(repl ${tgt})
        foreach(lib ${target_expanded}) # Has no duplicates -> match only happens once
            if("${tgt}" STREQUAL "${lib}")
                #message(STATUS "Replacing ${repl} with ${target_expanded_${lib}}")
                set(repl ${target_expanded_${lib}})
            endif()
        endforeach()
        list(APPEND target_names_expanded ${repl})
    endforeach()
    #message(STATUS "target_names_expanded: ${target_names_expanded} \n")
    list(LENGTH target_names_expanded count)
    if(count GREATER 500)
        message(FATAL_ERROR "Length of list exeeds 500")
        exit(0)
    endif()

    # Now we do the recursion.
    # If there are any targets left in target_names_expanded, we call expand_target_libs again
    # Note that nothing actually happens in the following two "foreach" if there are no targets left
    if("Threads::Threads_core" IN_LIST target_names_expanded)
        message(FATAL_ERROR "1")
    endif()
    foreach(lib ${target_names_expanded})
        if(TARGET ${lib})
            expand_target_libs(${lib} recursed_libs)
            set(target_recursed_${lib} ${recursed_libs})
            list(APPEND target_recursed ${lib})
        endif()
    endforeach()

    foreach(lib ${target_recursed})
        list(TRANSFORM target_names_expanded REPLACE "${lib}" "${target_recursed_${lib}}")
    endforeach()

    # Remove duplicates in a way that retains linking order, i.e. keep last occurrence
    list(REVERSE "target_names_expanded")
    list(REMOVE_DUPLICATES "target_names_expanded")
    list(REVERSE "target_names_expanded")
    # Return the expanded targets
    set(${expanded_list} ${target_names_expanded} PARENT_SCOPE)
endfunction()



function(expand_target_libs_interface_only target_names expanded_list)
    foreach(lib ${target_names})
        #        message("Checking if ${lib} is a target")
        if(TARGET ${lib})
            unset(interface_libs)
            unset(imported_lib)
            #            message("Expanding target ${lib}")
            get_target_property(interface_libs  ${lib} INTERFACE_LINK_LIBRARIES)
            # Now we have all the libs in the target "lib".
            # interface_libs may contain many other targets. That's ok, for now we only do one level of expansion.
            if(interface_libs)
                list(APPEND target_expanded_${lib}  ${interface_libs})
            endif()
            list(APPEND target_expanded ${lib})
            #            message("Expanded target ${lib} into ${target_expanded_${lib}}")
        endif()
    endforeach()

    set(target_names_expanded ${target_names})
    foreach(lib ${target_expanded})
        list(TRANSFORM target_names_expanded REPLACE "${lib}" "${target_expanded_${lib}}")
    endforeach()

    # Now we do the recursion.
    # If there are any targets left in target_names_expanded, we call expand_target_libs again
    # Note that nothing actually happens in the following two "foreach" if there are no targets left
    foreach(lib ${target_names_expanded})
        if(TARGET ${lib})
            expand_target_libs(${lib} recursed_libs)
            set(target_recursed_${lib} ${recursed_libs})
            list(APPEND target_recursed ${lib})
        endif()
    endforeach()

    foreach(lib ${target_recursed})
        list(TRANSFORM target_names_expanded REPLACE "${lib}" "${target_recursed_${lib}}")
    endforeach()

    # Remove duplicates in a way that retains linking order, i.e. keep last occurrence
    list(REVERSE "target_names_expanded")
    list(REMOVE_DUPLICATES "target_names_expanded")
    list(REVERSE "target_names_expanded")
    # Return the expanded targets
    set(${expanded_list} ${target_names_expanded} PARENT_SCOPE)
endfunction()

function(expand_target_one_level target_names expanded_list)
    foreach(target_name ${target_names})
        if(TARGET ${target_name})
            list(APPEND target_names_expanded ${target_name})
            unset(interface_libs)
            unset(imported_lib)
            get_target_property(interface_libs  ${target_name} INTERFACE_LINK_LIBRARIES)
            # Now we have all the libs in the target "lib".
            # interface_libs may contain many other targets. That's ok, for now we only do one level of expansion.
            foreach(elem ${interface_libs})
                if(TARGET ${elem})
                    list(APPEND target_names_expanded ${elem})
                endif()
            endforeach()
        endif()
    endforeach()
    # Remove duplicates in a way that retains linking order, i.e. keep last occurrence
    list(REVERSE "target_names_expanded")
    list(REMOVE_DUPLICATES "target_names_expanded")
    list(REVERSE "target_names_expanded")
    # Return the expanded targets
    set(${expanded_list} ${target_names_expanded} PARENT_SCOPE)
endfunction()



function(expand_target_all_targets target_names expanded_list)
    foreach(target_name ${target_names})
        if(TARGET ${target_name} AND NOT ${target_name} IN_LIST ${expanded_list})
            list(APPEND target_names_expanded ${target_name})
            unset(interface_libs)
            get_target_property(interface_libs  ${target_name} INTERFACE_LINK_LIBRARIES)
            foreach(elem ${interface_libs})
                if(TARGET ${elem} AND NOT ${elem} IN_LIST ${expanded_list} AND NOT ${elem} IN_LIST target_names AND NOT ${elem} IN_LIST target_names_expanded)
                    expand_target_all_targets(${elem} recursed_list)
                    foreach(rec ${recursed_list})
                        if(TARGET ${rec} AND NOT ${rec} IN_LIST target_names_expanded AND NOT ${rec} IN_LIST ${expanded_list})
                            list(APPEND target_names_expanded ${rec})
                        endif()
                    endforeach()
                endif()
            endforeach()
        endif()
    endforeach()

    # Remove duplicates in a way that retains linking order, i.e. keep last occurrence
    if(target_names_expanded)
        list(REVERSE target_names_expanded)
        list(REMOVE_DUPLICATES "target_names_expanded")
        list(REVERSE "target_names_expanded")
        list(APPEND ${target_names_expanded} "${${expanded_list}}")
    endif()
    list(APPEND ${expanded_list} "${target_names_expanded}")
    set(${expanded_list} "${${expanded_list}}" PARENT_SCOPE)
endfunction()


function(expand_target_incs target_names expanded_list)
    foreach(target_name ${target_names})
        if(TARGET ${target_name})
            get_target_property(target_libs  ${target_name} INTERFACE_LINK_LIBRARIES)
            get_target_property(found_incs   ${target_name} INTERFACE_INCLUDE_DIRECTORIES)
            foreach(lib ${target_libs})
                if(TARGET ${lib})
                    expand_target_incs(${lib} ${expanded_list})
                    get_target_property(incs ${lib} INTERFACE_INCLUDE_DIRECTORIES)
                    foreach(inc ${incs})
                        if(inc)
                            list(APPEND found_incs ${inc})
                        endif()
                    endforeach()
                endif()
            endforeach()

            #    set(target_incs)
            foreach(inc ${found_incs})
                if(inc)
                    list(APPEND target_incs ${inc})
                endif()
            endforeach()

            #    message("Current target_incs: ${target_incs}")
            #    message("Current ${expanded_list}: ${${expanded_list}}")
            #    message("Returning:  ${${expanded_list}};${target_incs}")
            if(NOT ${expanded_list})
                set(${expanded_list} "${target_incs}" PARENT_SCOPE)
            else()
                set(${expanded_list} "${${expanded_list}};${target_incs}" PARENT_SCOPE)
            endif()
        endif()
    endforeach()
endfunction()




function(expand_target_opts target_names expanded_list)
    foreach(target_name ${target_names})
        if(TARGET ${target_name})
            get_target_property(target_libs  ${target_name} INTERFACE_LINK_LIBRARIES)
            get_target_property(found_opts  ${target_name} INTERFACE_COMPILE_OPTIONS)
            foreach(lib ${target_libs})
                if(TARGET ${lib})
                    expand_target_opts(${lib} ${expanded_list})
                    get_target_property(opts ${lib} INTERFACE_COMPILE_OPTIONS)
                    foreach(opt ${opts})
                        if(opt)
                            list(APPEND found_opts ${opt})
                        endif()
                    endforeach()
                endif()
            endforeach()
            foreach(opt ${found_opts})
                if(opt)
                    list(APPEND target_opts ${opt})
                endif()
            endforeach()
            #    message("Current target_opts: ${target_opts}")
            #    message("Current ${expanded_list}: ${${expanded_list}}")
            #    message("Returning:  ${${expanded_list}};${target_opts}")
            if(NOT ${expanded_list})
                set(${expanded_list} "${target_opts}" PARENT_SCOPE)
            else()
                set(${expanded_list} "${${expanded_list}};${target_opts}" PARENT_SCOPE)
            endif()
        endif()
    endforeach()
endfunction()

function(expand_target_defs target_names expanded_list)
    foreach(target_name ${target_names})
        if(TARGET ${target_name})
            get_target_property(target_libs  ${target_name} INTERFACE_LINK_LIBRARIES)
            get_target_property(defs  ${target_name} INTERFACE_COMPILE_DEFINITIONS)
            list(APPEND found_defs "${defs}")
            foreach(lib ${target_libs})
                if(TARGET ${lib})
                    expand_target_opts(${lib} ${expanded_list})
                    get_target_property(defs ${lib} INTERFACE_COMPILE_DEFINITIONS)
                    list(APPEND found_defs "${defs}")
                endif()
            endforeach()
            foreach(def ${found_defs})
                if(def)
                    list(APPEND target_defs ${def})
                endif()
            endforeach()
            if(NOT ${expanded_list})
                set(${expanded_list} "${target_defs}" PARENT_SCOPE)
            else()
                set(${expanded_list} "${${expanded_list}};${target_defs}" PARENT_SCOPE)
            endif()
        endif()
    endforeach()
endfunction()