function(cache_internal_init_unique VAR_NAME)
  
    if(NOT ARGN)
        message(SEND_ERROR "Error: cache_internal_init_unique() called without any elements")
        return()
    endif()
  
    set(${VAR_NAME})  
    foreach(ELEMENT ${ARGN})
        list(APPEND ${VAR_NAME} ${ELEMENT})
    endforeach()
      
    list(REMOVE_DUPLICATES ${VAR_NAME})
    set(${VAR_NAME} ${${VAR_NAME}} CACHE INTERNAL "")  

endfunction()


# This function appends items to a CACHE INTERNAL variable
# and removes duplicates
# Example usage:
# cache_internal_append_unique(MY_VARIABLE ${CMAKE_CURRENT_BINARY_DIR}) 
function(cache_internal_append_unique VAR_NAME)

    if(NOT ARGN)
        message(SEND_ERROR "Error: cache_internal_append_unique() called without any elements")
        return()
    endif()
  
    foreach(ELEMENT ${ARGN})
        list(APPEND ${VAR_NAME} ${ELEMENT})
    endforeach()
  
    list(REMOVE_DUPLICATES ${VAR_NAME})
    set(${VAR_NAME} ${${VAR_NAME}} CACHE INTERNAL "")
  
endfunction()


function(cache_internal_remove_duplicates VAR_NAME)

  list(REMOVE_DUPLICATES ${VAR_NAME})
  set(${VAR_NAME} ${${VAR_NAME}} CACHE INTERNAL "")

endfunction()

