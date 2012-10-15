#!/usr/bin/perl -w

eval 'exec /usr/bin/perl -w -S $0 ${1+"$@"}'
    if 0; # not running under some shell
use strict;

$| = 1;

unless (@ARGV >= 1) {
	print STDERR qq(Usage:
$0 [filename] query
				
	If no filename is given, supply XML on STDIN.
);
	exit;
}

use XML::XPath;

my $xpath;

my $pipeline;

if ($ARGV[0] eq '-p') {
	# pipeline mode
	$pipeline = 1;
	shift @ARGV;
}
if (@ARGV >= 2) {
	$xpath = XML::XPath->new(filename => shift(@ARGV));
}
else {
	$xpath = XML::XPath->new(ioref => \*STDIN);
}

my $nodes = $xpath->find(shift @ARGV);

unless ($nodes->isa('XML::XPath::NodeSet')) {
NOTNODES:
	print STDERR "Query didn't return a nodeset. Value: ";
	print $nodes->value, "\n";
	exit;
}

if ($pipeline) {
	$nodes = find_more($nodes);
	goto NOTNODES unless $nodes->isa('XML::XPath::NodeSet');
}

if ($nodes->size) {
	print STDERR "Found ", $nodes->size, " nodes:\n";
	foreach my $node ($nodes->get_nodelist) {
		print STDERR "-- NODE --\n";
		print $node->toString;
	}
}
else {
	print STDERR "No nodes found";
}

print STDERR "\n";

exit;

sub find_more {
	my ($nodes) = @_;
	if (!@ARGV) {
		return $nodes;
	}
	
	my $newnodes = XML::XPath::NodeSet->new;
	
	my $find = shift @ARGV;
	
	foreach my $node ($nodes->get_nodelist) {
		my $new = $xpath->find($find, $node);
		if ($new->isa('XML::XPath::NodeSet')) {
			$newnodes->append($new);
		}
		else {
			warn "Not a nodeset: ", $new->value, "\n";
		}
	}
	
	return find_more($newnodes);
}
