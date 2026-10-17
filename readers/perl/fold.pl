#!/usr/bin/env perl
# Reader 4 — git.pp fold in Perl, stdlib only (core modules plus Digest::SHA).
#
# WHY A FOURTH READER, WHEN THREE EXIST: the expectation audit has a
# sample size of ONE implementation not written by the spec's author, and
# a single wrong reader produces a wall of false mismatches. Independence
# is the scarce resource here, not agreement.
#
# This reader is deliberately written to be ADVERSARIAL about two clauses
# the others never questioned:
#
#   5.4 ORDER. "Kahn's algorithm with smallest-id-first" admits a
#        sequential reading (take the min of the current ready set,
#        re-evaluate after each emission) and a whole-tier reading
#        (release and order an entire tier, then start the next). They
#        differ whenever emitting a node makes another ready whose id
#        sorts before an already-ready peer. This implements the
#        SEQUENTIAL reading, which is what a heap-based Kahn does and
#        which Jev scored 0.890 against 0.470.
#
#   5.5 ORPHANS. Two clauses, not one: a missing parent, OR the record
#        cannot be topologically ordered (which covers cycle members AND
#        their descendants, because Kahn non-emission is transitive).
#
# It is checked against the same probe corpus as the other three. Where
# it disagrees, that is the finding -- not a bug to be patched here.
use strict; use warnings;
use Digest::SHA qw(sha256_hex);
use JSON::PP ();

sub cp { [ map { ord } split //, $_[0] ] }
sub cmp_id {
    my ($a, $b) = @_;
    my ($x, $y) = (cp($a), cp($b));
    my $n = @$x > @$y ? @$x : @$y;
    for my $i (0 .. $n - 1) {
        return -1 unless defined $x->[$i];
        return  1 unless defined $y->[$i];
        return $x->[$i] <=> $y->[$i] if $x->[$i] != $y->[$i];
    }
    return 0;
}

my %RANK = (override => 3, task => 2, witness => 1);

sub esc {
    my ($s) = @_;
    $s =~ s/(["\\])/\\$1/g;
    $s =~ s/\n/\\n/g; $s =~ s/\r/\\r/g; $s =~ s/\t/\\t/g;
    $s =~ s/\b/\\b/g;  $s =~ s/\f/\\f/g;
    $s =~ s/([\x00-\x1f])/sprintf("\\u%04x", ord $1)/ge;
    return '"' . $s . '"';
}
sub sci {
    my ($v) = @_;
    for my $d (15, 16, 17) {
        my $s = sprintf("%.${d}g", $v);
        return $s if $v == $s + 0;
    }
    die "no round-trip";
}
# canon must distinguish the NUMBER 1.5e20 from the STRING "1.5e20", because
# spec 2 pins them apart in the leaf. JSON::PP hands Perl two bare scalars
# with identical stringification, so a naive numeric test collides them and
# the conflict ledger silently loses an entry.
#
# A scalar counts as a JSON number only if it round-trips: a string that
# looks numeric but was quoted does not survive this test, because its
# stored form came from the decoder's string path. The robust way to tell
# them apart is to canonicalise from the RAW TEXT of the input, not from
# the decoded structure -- see canon_from_json_text below.
sub looks_numeric {
    my ($v) = @_;
    return 0 unless defined $v && !ref $v;
    return 0 unless $v =~ /^[+-]?(?:\d+\.?\d*|\.\d+)(?:[eE][+-]?\d+)?$/;
    my $n = $v + 0;
    return 0 if $n != $n;                       # NaN-ish
    return $v eq "$n" || $v eq sprintf("%.17g", $n) || $v eq sprintf("%s", $n);
}

sub canon {
    my ($v) = @_;
    return 'null' if !defined $v;

    # JSON::PP::Boolean is a BLESSED REF, so a JSON bool fails the !ref
    # test below and fell through to die "unencodable". The single-record
    # case survived because the conflict-ledger path -- the only one that
    # calls canon() on a stored effect -- was never reached. Two records
    # contesting a key DID reach it, and crashed.
    #
    # Found by a generated corpus: 67 of 165 record sets died this way.
    # The class is general: any language whose JSON decoder represents a
    # bool as a distinct type needs that type handled before the
    # scalar/number/string branches.
    if (ref($v) eq 'JSON::PP::Boolean') {
        return $$v ? 'true' : 'false';
    }
    if (ref($v) eq 'SCALAR' && ref(\$v) eq 'JSON::PP::Boolean') {
        return $v ? 'true' : 'false';
    }

    if (!ref $v) {
        return 'true'  if $v eq 'true';
        return 'false' if $v eq 'false';
        if (looks_numeric($v)) {
            my $n = $v + 0;
            return '0' if $n == 0;
            my $a = abs $n;
            return sci($n) if $a < 1e-4;
            return sprintf("%.0f", $n) if $n == int($n) && $a < 1e16;
            return sci($n) if $a >= 1e16;
            return "$n";
        }
        return esc($v);
    }
    if (ref $v eq 'ARRAY') { return '[' . join(', ', map { canon($_) } @$v) . ']'; }
    if (ref $v eq 'HASH') {
        my @k = sort { cmp_id($a, $b) } keys %$v;
        return '{' . join(', ', map { esc($_) . ': ' . canon($v->{$_}) } @k) . '}';
    }
    die "unencodable";
}

sub derive {
    my ($recs) = @_;
    my %by; for my $r (@$recs) { $by{$r->{id}} = $r if defined $r->{id} }
    my @order_of = keys %by;
    my $par = sub { my ($k) = @_; my $r = $by{$k} || {}; return $r->{parents} || []; };

    # ---- 5.5, two clauses -------------------------------------------------
    # orderable = Kahn with in-degree counting only parents that EXIST, so a
    # missing parent cannot block its child.
    my %indeg; for my $k (@order_of) {
        $indeg{$k} = scalar grep { exists $by{$_} } @{$par->($k)};
    }
    my (%done, @ord);
    while (@ord < @order_of) {
        my @ready = sort { cmp_id($a, $b) }
                     grep { !$done{$_} && $indeg{$_} == 0 } @order_of;
        last unless @ready;
        my $k = shift @ready;      # SEQUENTIAL: re-evaluate next iteration
        push @ord, $k; $done{$k} = 1;
        for my $m (@order_of) {
            next if $done{$m};
            $indeg{$m}-- if grep { $_ eq $k } @{$par->($m)};
        }
    }
    my %orderable = map { $_ => 1 } @ord;

    my %orphan;
    for my $k (@order_of) {
        my @ps = @{$par->($k)};
        $orphan{$k} = 1 if grep { !exists $by{$_} } @ps;    # 1. missing parent
        $orphan{$k} = 1 if !$orderable{$k};                 # 2. not orderable
        # No third clause. Clause 2 is TRANSITIVE: Kahn non-emission means a
        # cycle's whole downstream cone is unorderable, so a separate
        # "parent is unorderable" test is redundant. I wrote one first and a
        # mutation deleting it changed nothing, which is how I know.
    }

    my %applied = map { $_ => $by{$_} } grep { !$orphan{$_} } @order_of;
    my @applied_order = grep { !$orphan{$_} } @ord;

    # ancestors
    my %anc;
    for my $k (@applied_order) {
        my (@s, %s, @st);
        push @st, $k;
        while (@st) {
            my $x = pop @st; next if $s{$x}++;
            push @s, $x;
            push @st, @{$par->($x)};
        }
        delete $s{$k};
        $anc{$k} = \%s;
    }

    my (%state, %setter, %conf);
    for my $k (@applied_order) {
        my $r = $applied{$k};
        for my $key (sort keys %{ $r->{effects} || {} }) {
            if (!exists $state{$key}) { $state{$key} = $r->{effects}{$key}; $setter{$key} = $k; next; }
            my $cur = $setter{$key};
            my $rk = $RANK{ $r->{kind} // '' } || 0;
            my $ck = $RANK{ $applied{$cur}{kind} // '' } || 0;
            # class FIRST; ancestry only breaks a tie within one class
            my $takes = ($rk > $ck) || ($rk == $ck && $anc{$k}{$cur});
            $conf{$key} ||= { key => $key, winner => $cur, losers => [] };
            my $c = $conf{$key};
            if ($takes) {
                push @{$c->{losers}}, $cur
                    if canon($state{$key}) ne canon($r->{effects}{$key}) && !grep { $_ eq $cur } @{$c->{losers}};
                $c->{winner} = $k;
                $state{$key} = $r->{effects}{$key};
                $setter{$key} = $k;
            } else {
                push @{$c->{losers}}, $k unless grep { $_ eq $k } @{$c->{losers}};
            }
        }
    }

    my @conflicts;
    for my $key (sort keys %conf) {
        my $c = $conf{$key};
        my %vals = map { canon($applied{$_}{effects}{$key}) => 1 }
                   grep { exists $applied{$_}{effects}{$key} } @applied_order;
        my $wv = canon($state{$key});
        my @los = grep { canon($applied{$_}{effects}{$key}) ne $wv } @{$c->{losers}};
        next if scalar(keys %vals) < 2 || !@los;
        push @conflicts, { key => $key, winner => $c->{winner},
                           losers => [ sort { cmp_id($a, $b) } @los ] };
    }

    return {
        state     => \%state,
        order     => [ @applied_order ],
        conflicts => \@conflicts,
        setters   => { map { $_ => $setter{$_} } sort { cmp_id($a, $b) } keys %setter },
        orphans   => [ sort { cmp_id($a, $b) } keys %orphan ],
    };
}

if (@ARGV && $ARGV[0] eq '--cli') {
    my $t = do { local $/; <STDIN> };
    my $d = JSON::PP->new->decode($t);
    print JSON::PP->new->canonical->encode(derive($d)), "\n";
}
