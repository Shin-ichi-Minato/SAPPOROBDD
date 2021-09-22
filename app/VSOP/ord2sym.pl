#!/usr/bin/perl
print "symbol /t\n";
print "symbol ";
while(<STDIN>){
  while(/([0-9]+)/gi){
    print "x$1 " ;
  }
}
print "\n";

