#!/usr/bin/perl
#
# usage:
#    perl ./gen_oclcall_hook.pl 

#if ($#ARGV > -1) {
#	$CALL_VECTOR_SYM = $ARGV[0];
#} else {
#	$CALL_VECTOR_SYM = "__ocl_call_vector";
#}
#
#if ($#ARGV > 0) {
#	$CALL_SYM_PREFIX = $ARGV[1];
#} else {
#	$CALL_SYM_PREFIX = "";
#}

$master = '../libocl/oclcall.master';
open(MASTER, $master);
@lines = <MASTER>;
close(MASTER);
#@lines = <STDIN>;

open($out, ">oclcall_hook.c");
#$out = *STDOUT;

### 
### generate oclcall_hook.c
###

printf $out "/* oclcall_hook.c\n";
printf $out " * This file was auto-generated by gen_oclcall_hook.pl\n";
printf $out " */\n\n";

printf $out "/* $CALL_VECTOR_SYM $CALL_SYM_PREFIX */\n\n";

printf $out "#include \"CL/cl.h\"\n\n";
printf $out "#include \"CL/cl_gl.h\"\n\n";

printf $out "#include \"printcl.h\"\n\n";

printf $out "\ntypedef void (*cl_pfn_notify_t)(const char*, const void*, size_t, void*);\n";
printf $out "typedef void (*cl_pfn_notify2_t)(cl_program , void* );\n";
printf $out "typedef void (*cl_user_func_t)(void*);\n\n";

#######################################
#
foreach $l (@lines) {
	if (!($l =~ /^[ \t]*#.*/)) {
		@fields = split(' ', $l);
		$name = $fields[0];
		if ($name =~ /^[a-zA-Z].*/) {
			$type = $fields[1];
			$retype = $fields[2];
			$tlist = $fields[3];
			$tlist =~ s/~/ /g;
			@args = split(',',$tlist);
			$atlist = "";
			$alist = "";
			$j=0;
			foreach $a (@args) {
				if ($j > 0) { $atlist .=",$a a$j"; }
				else { $atlist .= "$a a$j"; }
				if ($j > 0) { $alist .=",a$j"; }
				else { $alist .= "a$j"; }
				$j += 1;
			}
			if ($atlist eq "void a0") {
				$atlist = "void";
				$alist = "";
			}
			if (!($name =~ /^reserved/)) { 
				printf $out "void __oclcall_prehook_$name($tlist);\n\n";
				printf $out "void __oclcall_prehook_$name($atlist) {\n"; 
				printf $out "\tprintcl( CL_DEBUG \" __oclcall_prehook_$name \" );\n";
   			printf $out "\tprintf(\">>> stopped @ pre $name ( [c]ontinue )\\n\");\n";
   			printf $out "\tchar key = 0; while ( (key = getchar()) != 'c' );\n";
				printf $out "}\n\n";
				printf $out "void __oclcall_posthook_$name($tlist);\n\n";
				printf $out "void __oclcall_posthook_$name($atlist) {\n";
				printf $out "\tprintcl( CL_DEBUG \" __oclcall_posthook_$name \" );\n";
   			printf $out "\tprintf(\">>> stopped @ post $name ( [c]ontinue )\\n\");\n";
   			printf $out "\tchar key = 0; while ( (key = getchar()) != 'c' );\n";
				printf $out "} \n\n";

			}
		}
	}
}
printf $out "\n";

printf $out "\nstatic void* _cvec0[] = { \\\n";
$i=0;
foreach $l (@lines) {
	if (!($l =~ /^[ \t]*#.*/)) {
		@fields = split(' ', $l);
		$name = $fields[0];
		if ($name =~ /^[a-zA-Z].*/) {
			unless ($name =~ /reserved/) {
				$tlist = $fields[3];
				@args = split(',',$tlist);
				if ($tlist =~ /^void$/) { $narg = 0; }
				else { $narg = $#args+1; }
				printf $out "\t__oclcall_prehook_$name, \\\n";
			}
		}
	$i = $i + 1;
	}
}
printf $out "\t};\n";

printf $out "\nvoid** __oclcall_prehook_call_vector = (void**)_cvec0;\n";

printf $out "\nstatic void* _cvec1[] = { \\\n";
$i=0;
foreach $l (@lines) {
	if (!($l =~ /^[ \t]*#.*/)) {
		@fields = split(' ', $l);
		$name = $fields[0];
		if ($name =~ /^[a-zA-Z].*/) {
			unless ($name =~ /reserved/) {
				$tlist = $fields[3];
				@args = split(',',$tlist);
				if ($tlist =~ /^void$/) { $narg = 0; }
				else { $narg = $#args+1; }
				printf $out "\t__oclcall_posthook_$name, \\\n";
			}
		}
	$i = $i + 1;
	}
}
printf $out "\t};\n";

printf $out "\nvoid** __oclcall_posthook_call_vector = (void**)_cvec1;\n";


#close($out);

