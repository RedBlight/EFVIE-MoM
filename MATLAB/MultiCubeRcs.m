%clc;
%clear all;
%close all;

geom1 = "cube6";
geom2 = "cube78";
geom3 = "cube100";
geom4 = "cube173";

ruleN = "eps2";
waveN = "PlaneZinYpol";
obspN = "xycircular181";

ObserveRCS( geom1, ruleN, waveN, obspN );
ObserveRCS( geom2, ruleN, waveN, obspN );
ObserveRCS( geom3, ruleN, waveN, obspN );
ObserveRCS( geom4, ruleN, waveN, obspN );

rcs1 = ReadRcsResult( geom1 );
rcs2 = ReadRcsResult( geom2 );
rcs3 = ReadRcsResult( geom3 );
rcs4 = ReadRcsResult( geom4 );

T = readtable( "RCS.dat" );
rcs0 = table2array( T(:,2) );

figure(  );
plot( ...
    -180:2:180, rcs1, "red", ...
    -180:2:180, rcs2, "green", ...
    -180:2:180, rcs4, "blue", ...
    -180:2:180, rcs3, "magenta" ...
);

rcs1N = rcs1 - min( rcs1 );
rcs1N = rcs1N ./ max( rcs1N );
rcs1N = rcs1N .* ( max( rcs0 ) - min( rcs0 ) );
rcs1N = rcs1N + min( rcs0 );

rcs2N = rcs2 - min( rcs2 );
rcs2N = rcs2N ./ max( rcs2N );
rcs2N = rcs2N .* ( max( rcs0 ) - min( rcs0 ) );
rcs2N = rcs2N + min( rcs0 );


rcs3N = rcs3 - min( rcs3 );
rcs3N = rcs3N ./ max( rcs3N );
rcs3N = rcs3N .* ( max( rcs0 ) - min( rcs0 ) );
rcs3N = rcs3N + min( rcs0 );


rcs4N = rcs4 - min( rcs4 );
rcs4N = rcs4N ./ max( rcs4N );
rcs4N = rcs4N .* ( max( rcs0 ) - min( rcs0 ) );
rcs4N = rcs4N + min( rcs0 );

%rcs3N = max( rcs3N ) - rcs3N;

figure(  );
plot( ...
    0:2:360, rcs0, "black", ...
    0:2:360, rcs1N, "red", ...
    0:2:360, rcs2N, "green", ...
    0:2:360, rcs3N, "blue", ...
    0:2:360, rcs4N, "magenta" ...
);