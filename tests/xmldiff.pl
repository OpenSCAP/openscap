# This is a modification of XML-SemanticDiff
# ------------------------------------------
# 
# Semantically compare two XML files for equivalence. This is done while
# ignoring whitespace variations in the text as well as several 
# permutations in the order of the tags.
#
# For original XML-SemanticDiff please see: 
#
#     Search CPAN
#         http://search.cpan.org/dist/XML-SemanticDiff
#
#     CPAN Request Tracker:
#         http://rt.cpan.org/NoAuth/Bugs.html?Dist=XML-SemanticDiff
#
#     AnnoCPAN, annotated CPAN documentation:
#         http://annocpan.org/dist/XML-SemanticDiff
#
#     CPAN Ratings:
#         http://cpanratings.perl.org/d/XML-SemanticDiff
#
# COPYRIGHT AND LICENCE
#
# Copyright (c) 2000 Kip Hampton. All rights reserved. This program is
# free software; you can redistribute it and/or modify it under the same terms
# as Perl itself.

package SemanticDiff;

use strict;
use warnings;

use XML::Parser;

sub new {
    my ($proto, %args) = @_;
    my $class = ref($proto) || $proto;
    my $self = \%args;

    bless ($self, $class);
    return $self;
}

sub _is_file
{
    my ($self, $specifier) = @_;
    return $specifier !~ /\n/g && -f $specifier;
}

sub _get_pathfinder_obj {
    my $self = shift;

    return SemanticDiff::PathFinder::Obj->new();
}

sub read_xml {
    my $self = shift;

    my ($xml_specifier) = @_;

    if (ref($xml_specifier) eq 'HASH')
    {
        return $xml_specifier;
    }
    else
    {
        $self->{path_finder_obj} = $self->_get_pathfinder_obj();

        my $p = XML::Parser->new(
            Style => 'Stream',
            Pkg   => 'SemanticDiff::PathFinder',
            'Non-Expat-Options' => $self,
            Namespaces => 1
        );

        my $ret =
            $self->_is_file($xml_specifier)
                ? $p->parsefile($xml_specifier)
                : $p->parse($xml_specifier)
                ;
        
        $self->{path_finder_obj} = undef;

        return $ret;
    }
}

sub _same_namespace
{
    my ($self, $to, $from) = @_;
    
    my $t_e = exists($to->{NamespaceURI});
    my $f_e = exists($from->{NamespaceURI});
    if (!$t_e && !$f_e)
    {
        return 1;
    }
    elsif ($t_e && $f_e)
    {
        return ($to->{NamespaceURI} eq $from->{NamespaceURI});
    }
    else
    {
        return 0;
    }
}

sub _match_xpath {
    my $self = shift;
    my ($xpath, $flat_name) = @_;
    my @x_way = split /\//, $xpath;
    my @f_way = split /\//, $flat_name;
    for my $i (0..$#x_way) {
        $x_way[$i]=~s/.*?://g;
    }           
    for my $i (0..$#f_way) {
        $f_way[$i]=~s/\[.*?\]$//g;
    }   
    return 0 if $#x_way > $#f_way;
    for my $i (0..$#x_way) {
        if ($x_way[$i] ne $f_way[$i]) {
            return 0;
        }
    }
    return 1;
}

sub compare {
    my $self = shift;
    my ($from_xml, $to_xml) = @_;

    my $from_doc = $self->read_xml($from_xml);
    my $to_doc = $self->read_xml($to_xml);

    my @warnings = ();

    my $handler = $self->{diffhandler} || SemanticDiff::BasicHandler->new(%$self);

    if (defined $self->{ignorexpath}) {
        my $ignore = $self->{ignorexpath};
        for my $path (@$ignore) {
            for my $ref ($from_doc, $to_doc) {
                for my $key (keys %$ref) {
                    if ($self->_match_xpath($path, $key)) {
                        delete $ref->{$key};
                    }
                }
            }
        }
    }

    push (@warnings, $handler->init($self)) if $handler->can('init');

    foreach my $element (sort keys (%$from_doc)) {

        if (defined $to_doc->{$element}) {

            unless ($from_doc->{$element}->{TextChecksum} eq $to_doc->{$element}->{TextChecksum}) {
                push (@warnings, $handler->element_value($element, 
                                                         $to_doc->{$element}, 
                                                         $from_doc->{$element}))
                          if $handler->can('element_value');
            }
        
            unless ($self->_same_namespace($from_doc->{$element},$to_doc->{$element})) {
                push (@warnings, $handler->namespace_uri($element, 
                                                         $to_doc->{$element}, 
                                                         $from_doc->{$element}))
                           if $handler->can('namespace_uri');
            }
    
            foreach my $attr (keys(%{$from_doc->{$element}->{Attributes}})) {
 
                 if (defined ($to_doc->{$element}->{Attributes}->{$attr})) {

                     if ($to_doc->{$element}->{Attributes}->{$attr} ne $from_doc->{$element}->{Attributes}->{$attr}){
                        push (@warnings, $handler->attribute_value($attr, 
                                                                   $element, 
                                                                   $to_doc->{$element},
                                                                   $from_doc->{$element}))
                              if $handler->can('attribute_value');
                     }
                     delete $to_doc->{$element}->{Attributes}->{$attr};
                 }
                 else {
                     push (@warnings, $handler->missing_attribute($attr, 
                                                                  $element, 
                                                                  $to_doc->{$element}, 
                                                                  $from_doc->{$element}))
                           if $handler->can('missing_attribute'); 
                 }                
            }

            foreach my $leftover (keys(%{$to_doc->{$element}->{Attributes}})) {
                push (@warnings, $handler->rogue_attribute($leftover, 
                                                           $element, 
                                                           $to_doc->{$element}, 
                                                           $from_doc->{$element}))
                     if $handler->can('rogue_attribute');
            }
            
            delete $to_doc->{$element};
        }
        else {  
            push (@warnings, $handler->missing_element($element, $from_doc->{$element}))
                      if $handler->can('missing_element');          
        }
    }

    foreach my $leftover ( keys (%$to_doc) ) {
        push (@warnings, $handler->rogue_element($leftover, $to_doc->{$leftover})) 
             if $handler->can('rogue_element');
    }                 

    push (@warnings, $handler->final($self)) if $handler->can('final');
            
    return @warnings;
}

package SemanticDiff::BasicHandler;

use strict;
use warnings;

sub new {       
    my ($proto, %args) = @_;
    my $class = ref($proto) || $proto;
    my $self  = \%args;
    bless ($self, $class);
    return $self;
}

sub rogue_element {
    my $self = shift;
    my ($element, $properties) = @_;
    my ($element_name, $parent) = parent_and_name($element);
    my $info = {context => $parent,
                message => "Rogue element '$element_name' in element '$parent'."};
    
    if ($self->{keeplinenums}) {
        $info->{startline} = $properties->{TagStart};
        $info->{endline}   = $properties->{TagEnd};
    }
 
    if ($self->{keepdata}) {
        $info->{new_value} = $properties->{CData};
    }    
    return $info;
}

sub missing_element {
    my $self = shift;
    my ($element, $properties) = @_;
    my ($element_name, $parent) = parent_and_name($element);
    my $info = {context => $parent,
                message => "Child element '$element_name' missing from element '$parent'."};
                 
    if ($self->{keeplinenums}) {
        $info->{startline} = $properties->{TagStart};
        $info->{endline}   = $properties->{TagEnd};
    }
    if ($self->{keepdata}) {
        $info->{old_value} = $properties->{CData};
    }
    return $info;
}

sub element_value {
    my $self = shift;
    my ($element, $new_properties, $old_properties) = @_;
    my ($element_name, $parent) = parent_and_name($element);

    my $info = {context => $element,
                message => "Character differences in element '$element_name'."};
                       
    if ($self->{keeplinenums}) {
        $info->{startline} = $new_properties->{TagStart};
        $info->{endline}   = $new_properties->{TagEnd};
    }
                  
    if ($self->{keepdata}) {
        $info->{old_value} = $old_properties->{CData};
        $info->{new_value} = $new_properties->{CData};
    }

    return $info;
}

sub rogue_attribute {
    my $self = shift;    
    my ($attr, $element, $properties) = @_;
    my ($element_name, $parent) = parent_and_name($element);
    my $info = {context  => $element,
                message  => "Rogue attribute '$attr' in element '$element_name'."};
        
    if ($self->{keeplinenums}) {
        $info->{startline} = $properties->{TagStart};
        $info->{endline}   = $properties->{TagEnd};
    }

    if ($self->{keepdata}) {
        $info->{new_value} = $properties->{Attributes}->{$attr};
    }
    return $info;
}

sub missing_attribute {
    my $self = shift;
    my ($attr, $element, $new_properties, $old_properties) = @_;
    my ($element_name, $parent) = parent_and_name($element);
    my $info = {context  => $element,
                message  => "Attribute '$attr' missing from element '$element_name'."};
         
    if ($self->{keeplinenums}) {
        $info->{startline} = $new_properties->{TagStart};
        $info->{endline}   = $new_properties->{TagEnd};
    }

    if ($self->{keepdata}) {
        $info->{old_value} = $old_properties->{Attributes}->{$attr};
    }
    return $info;
}

sub attribute_value {
    my $self = shift;
    my ($attr, $element, $new_properties, $old_properties) = @_;
    my ($element_name, $parent) = parent_and_name($element);
    my $info = {context  => $element,
                message  => "Attribute '$attr' has different value in element '$element_name'."};
                  
    if ($self->{keeplinenums}) {
        $info->{startline} = $new_properties->{TagStart};
        $info->{endline}   = $new_properties->{TagEnd};
    }
        
    if ($self->{keepdata}) {
        $info->{old_value} = $old_properties->{Attributes}->{$attr};
        $info->{new_value} = $new_properties->{Attributes}->{$attr};
    }
    return $info;
}

sub namespace_uri {
    my $self = shift;
    my ($element, $new_properties, $old_properties) = @_;
    my ($element_name, $parent) = parent_and_name($element);
    my $info = {context  => $element,
                message  => "Element '$element_name' within different namespace."};
            
    if ($self->{keeplinenums}) {
        $info->{startline} = $new_properties->{TagStart};
        $info->{endline}   = $new_properties->{TagEnd};
    }
                            
    if ($self->{keepdata}) {
        $info->{old_value} = $old_properties->{NamspaceURI};
        $info->{new_value} = $new_properties->{NamspaceURI};
    }
    return $info;
}

sub parent_and_name {
    my $element = shift;
    my @steps = split('/', $element);   
    my $element_name = pop (@steps);
    my $parent = join '/', @steps;
    $element_name =~ s/\[\d+\]$//;
    return ($element_name, $parent);
}

package SemanticDiff::PathFinder;

foreach my $func (qw(StartTag EndTag Text StartDocument EndDocument PI))
{
    no strict 'refs';
    *{__PACKAGE__.'::'.$func} = sub {
        my $expat = shift;
        return $expat->{'Non-Expat-Options'}->{path_finder_obj}->$func(
            $expat, @_
        );
    };
}

package SemanticDiff::PathFinder::Obj;

use strict;

use Digest::MD5  qw(md5_base64);

use Encode qw(encode_utf8);

foreach my $accessor (qw(descendents char_accumulator doc
    opts xml_context PI_position_index))
{
    no strict 'refs';
    *{__PACKAGE__.'::'.$accessor} = sub {
        my $self = shift;

        if (@_)
        {
            $self->{$accessor} = shift;
        }
        return $self->{$accessor};
    };
}

sub new {
    my $class = shift;

    my $self = {};
    bless $self, $class;

    $self->_init(@_);

    return $self;
}

sub _init {
    return 0;
}

sub StartTag {
    my ($self, $expat, $element) = @_;


    my %attrs = %_;
            
    my @context = $expat->context;
    my $context_length = scalar (@context);
    my $parent = $context[$context_length -1];
    push (@{$self->descendents()->{$parent}}, $element) if $parent;

    my $last_ctx_elem = $self->xml_context()->[-1] || { position_index => {}};

    push @{$self->xml_context()}, 
        {
            element => "$element", 
            'index' => ++$last_ctx_elem->{position_index}->{"$element"},
            position_index => {},
        };

    my $test_context;
 
    $test_context = $self->_calc_test_context();

    $self->doc()->{$test_context} = 
    {
        NamespaceURI => ($expat->namespace($element) || ""),
        Attributes   => \%attrs,
        ($self->opts()->{keeplinenums}
            ? ( TagStart => $expat->current_line) 
            : ()
        ),
    };
}

sub _calc_test_context
{
    my $self = shift;

    return 
        join("", 
            map { "/". $_->{'element'} . "[" . $_->{'index'} . "]" } 
            @{$self->xml_context()}
        );
}

sub EndTag {
    my ($self, $expat, $element) = @_;
    
    my @context = $expat->context;

    my $test_context = $self->_calc_test_context();

    my $text;
    if ( defined( $self->char_accumulator()->{$element} )) { 
        $text = $self->char_accumulator()->{$element};
        delete $self->char_accumulator()->{$element};
    }
    $text ||= 'o';
    
    $self->doc()->{"$test_context"}->{TextChecksum} = md5_base64(encode_utf8("$text"));
    if ($self->opts()->{keepdata}) {
        $self->doc()->{"$test_context"}->{CData} = $text;
    }
    
    
    if (defined ( $self->descendents()->{$element})) {
        my $seen = {};
        foreach my $child (@{$self->descendents()->{$element}}) {
            next if $seen->{$child};
            $seen->{$child}++;
        }
    }
    
    $self->doc()->{"$test_context"}->{TagEnd} = $expat->current_line if $self->opts()->{keeplinenums};

    pop(@{$self->xml_context()});
}

sub Text {
    my $self = shift;
    my $expat = shift;
    
    my $element = $expat->current_element;
    my $char = $_;
    
    $char =~ s/^\s*//;
    $char =~ s/\s*$//;
    $char =~ s/\s+/ /g;
    $self->char_accumulator()->{$element} .= $char if $char;
    
}
        
sub StartDocument {
    my $self = shift;
    my $expat = shift;
    $self->doc({});
    $self->descendents({});
    $self->char_accumulator({});
    $self->opts($expat->{'Non-Expat-Options'});
    $self->xml_context([]);
    $self->PI_position_index({});
}
        
sub EndDocument {
    my $self = shift;

    return $self->doc();
}


sub PI {
    my ($self, $expat, $target, $data) = @_;
    my $attrs = {};
    $self->PI_position_index()->{$target}++;

    foreach my $pair (split /\s+/, $data) {
        $attrs->{$1} = $2 if $pair =~ /^(.+?)=["'](.+?)["']$/;
    }

    my $slug = '?' . $target . '[' . $self->PI_position_index()->{$target} . ']';

    $self->doc()->{$slug} =
        {
            Attributes => ($attrs || {}),
            TextChecksum => "1",
            NamespaceURI => "", 
            ( $self->opts()->{keeplinenums} 
            ? (
                TagStart => $expat->current_line(),
                TagEnd => $expat->current_line(),
              )
            : ()
            ),
        };
}   

1;

sub usage {
   die "usage: $0 one.xml two.xml \n";
}

use strict;

my $diff = SemanticDiff->new(keeplinenums => 1);

my ($file1, $file2) = @ARGV;
usage() unless defined $file1 and defined $file2;

my $ret_val = 2;

eval { 
    my @diffs = $diff->compare($file1, $file2); 

    foreach my $change (@diffs) {
       print STDERR "$change->{message} ($change->{startline} <> $change->{endline})\n";
    }

    $ret_val = (scalar @diffs) == 0 ? 0 : 1;

}; warn $@ if $@;

exit $ret_val;
