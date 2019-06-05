#!/usr/bin/perl

use strict;
use warnings;

sub Exit {
	print shift;
	exit(1);
}

our %spec;

if ($#ARGV + 1) {
	%spec = Spec($ARGV[0]);
	unless (%spec) {Exit "Неверные заданы параметры\n";}
	Header();
	Makefile();
	system "make clean > /dev/null";
	system "mkdir build -p";
}
else {
	Exit "Неверное количество параметров\n";
}

sub Spec {
	my $mode = shift;
	my %spec;
	if ($mode eq "x64") {
		$spec{"arch"} = "x64";
		$spec{"digit_t"} = "uint64_t";
		$spec{"str"} = 19;
	}
	else {
		my ($d, $p);
		my %sz_table = ("8" => 2, "16" => 4, "32" => 9);
		$spec{"arch"} = "any";
		if ($mode eq "c16") {$d = "8"; $p = "16";}
		else {if ($mode eq "c32") {$d = "16"; $p = "32";}
		else {if ($mode eq "c64") {$d = "32"; $p = "64";}
		else {return;}}}
		$spec{"digit_t"} = "uint${d}_t";
		$spec{"proc_t"} = "uint${p}_t";
		$spec{"len"} = $d;
		$spec{"str"} = $sz_table{$d};
	}
	return %spec;
}

sub Header {
	open (my $file, '>', 'configure.h');
	my $c = not ($spec{"arch"} eq "x64");
	my $dt = $spec{"digit_t"};
	my $pt = $spec{"proc_t"};
	my $len = $spec{"len"};
	my $str = $spec{"str"};
	print $file "#ifndef CONFIGURE_H\n";
	print $file "#define CONFIGURE_H\n\n";
	print $file "#include <ctype.h>\n\n";
	print $file "typedef $dt digit_t;\n";
	if ($c) {print $file "typedef $pt proc_t;\n";}
	print $file "typedef uint8_t dec_t;\n\n";
	if ($c) {
		print $file "#define DGT_LEN ${len}\n";
		print $file "#define DGT_LOW ((((proc_t)1u) << DGT_LEN)-1)\n";
		print $file "#define DGT_HIGH (DGT_LOW << DGT_LEN)\n\n";
	};
	print $file "#define STR_FACTOR ${str}\n\n";
	print $file "#endif\n";
}

sub Makefile {
	open (my $file, '>', 'Makefile');
	my $dir = "build";
	my $debug = "-g";

	my $digit = $dir . "/digit.o";
	my $number = $dir . "/number.o";
	my $lib = $dir . "/libnumber.so";

	print $file "${lib}: ${digit} ${number}\n";
	print $file "\tg++ ${debug} -shared ${digit} ${number} -o ${lib}\n\n";

	print $file "install: ${lib} configure.h\n";
	print $file "\tmkdir -p /usr/local/include/number\n";
	print $file "\tcp ${lib} /usr/local/lib\n";
	print $file "\tcp number.h configure.h /usr/local/include/number\n";
	print $file "\tldconfig\n\n";

	print $file "${number}: number.cpp number.h\n";
	print $file "\tg++ number.cpp ${debug} -c -fPIC --std=c++11 -o ${number}\n\n";

	if ($spec{"arch"} eq "x64") {
		print $file "${digit}: digit.nasm\n";
		print $file "\tnasm digit.nasm ${debug} -felf64 -o ${digit}\n\n";
	}

	else {
		print $file "${digit}: digit.c\n";
		print $file "\tgcc digit.c ${debug} -c -fPIC --std=c11 -o ${digit}\n\n";
	}

	print $file "clean:\n\trm -f ${number} ${digit} ${lib}\n"
}
