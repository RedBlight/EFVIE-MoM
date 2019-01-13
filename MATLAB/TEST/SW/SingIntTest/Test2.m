clc;
clear all;
close all;
format long;

v1 = [ 1; 0; 0 ];
v2 = [ 0; 1; 0 ];
v3 = [ 0; 0; 1 ];
vr = [ 0; 0; 0 ];

vm = (v1+v2+v3)./3;

ErrRel = @( A_, N_ ) abs( ( A_ - N_ ) / N_ );
IsValid = @( n_ ) ~isnan( n_ ) && ~isinf( n_ );


valA = SingIntSur( vr, v1, v2, v3 );
valN = AdaptiveSingIntSur( vr, v1, v2, v3, 0, 0 );

ErrRel( valA, valN )