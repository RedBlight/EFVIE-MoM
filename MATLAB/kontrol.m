%clc;
%clear all;
%close all;

fileID = fopen( 'Data/sphere52r1.rcsresult' );
A = fread( fileID, 182, 'double' );

fclose( fileID );

T = readtable( "RCS.dat" );

C = table2array( T(:,2) );
R = A(2:182);

NR = R - min( R );
NR = NR ./ max( NR );
NR = NR .* ( max( C ) - min( C ) );
NR = NR + min( C );

figure(  );
plot( -180:2:180, NR, "red", -180:2:180, C, "blue" );

figure(  );
plot( -180:2:180, R, "red" );










