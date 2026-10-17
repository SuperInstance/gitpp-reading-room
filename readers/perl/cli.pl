#!/usr/bin/env perl
use strict; use warnings;
use JSON::PP ();
my $t = do { local $/; <STDIN> };
require "./fold.pl";
my $d = JSON::PP->new->decode($t);
print JSON::PP->new->canonical->encode(derive($d)), "\n";
