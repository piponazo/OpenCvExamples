set(EXTRA_C_FLAGS "")

if (UNIX)
    set (CMAKE_CXX_FLAGS                "")
    #set (CMAKE_CXX_FLAGS_DEBUG          "-g -O0 -DDEBUG")
    set (CMAKE_CXX_FLAGS_DEBUG          "-g3 -gstrict-dwarf -O0")
endif()

if (UNIX) # Assuming gcc or clang
   if(WARNINGS_ANSI_ISO)
      set(EXTRA_C_FLAGS "${EXTRA_C_FLAGS} -Wextra")
   endif()

   if(WARNINGS_ARE_ERRORS)
      set(EXTRA_C_FLAGS "${EXTRA_C_FLAGS} -Werror")
   endif()

   set(EXTRA_C_FLAGS "${EXTRA_C_FLAGS} -Wall -Werror=return-type")
endif()

IF (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")

    set(EXTRA_C_FLAGS "${EXTRA_C_FLAGS} -fvisibility=hidden") # All the symbols will be hidden by default.
    set(EXTRA_C_FLAGS "${EXTRA_C_FLAGS} -Wreturn-type")
    set(EXTRA_C_FLAGS "${EXTRA_C_FLAGS} -Wno-long-long")
    set(EXTRA_C_FLAGS "${EXTRA_C_FLAGS} -Wno-maybe-uninitialized")
    set(EXTRA_C_FLAGS "${EXTRA_C_FLAGS} -Wno-unused-function")

    if(WARNINGS_ANSI_ISO)
        set(EXTRA_C_FLAGS "${EXTRA_C_FLAGS} -Wstrict-aliasing=3")
    else()
        set(EXTRA_C_FLAGS "${EXTRA_C_FLAGS} -Wno-narrowing")
        set(EXTRA_C_FLAGS "${EXTRA_C_FLAGS} -Wno-delete-non-virtual-dtor")
        set(EXTRA_C_FLAGS "${EXTRA_C_FLAGS} -Wno-unnamed-type-template-args")
    endif()

endif()

# Add user supplied extra options (optimization, etc...)
# ==========================================================
set(EXTRA_C_FLAGS "${EXTRA_C_FLAGS}" CACHE INTERNAL "Extra compiler options")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_C_FLAGS}")
