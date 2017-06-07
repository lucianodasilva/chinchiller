
macro (glob_code source_path file_list) 

	if (EXISTS "${source_path}")

		file ( GLOB ${file_list}
    		${source_path}/*.h
			${source_path}/*.hpp
			${source_path}/*.c
			${source_path}/*.cpp
			${source_path}/*.m
			${source_path}/*.mm
		)

	else ()
		message ( WARNING "Source code glob path '${source_path}' does not exist!" )
	endif ()

endmacro ()