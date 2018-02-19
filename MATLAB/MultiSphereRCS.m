% clc;
% clear all;
% close all;

N = 5;

geom(1) = "sphere1r1p6t";
geom(2) = "sphere1r2p24t";
geom(3) = "sphere1r3p105t";
geom(4) = "sphere1r4p278t";
geom(5) = "sphere1r5p591t";
geom(6) = "sphere1r6p1110t";
geom(7) = "sphere1r7p1795t";
geom(8) = "sphere1r8p2678t";

ruleN = "eps2";
waveN = "PlaneZinYpol";
obspN = "xycircular181";

rcs = zeros( 181, N );

for k = 1:N
	ObserveRCS( geom( k ), ruleN, waveN, obspN );
    rcs( :, k ) = ReadRcsResult( geom( k ) );
%	rcsRes = ReadRcsResult( geom( k ) );
%	fig = figure();
%	h = pcolor( reshape( rcsRes, [91,46] ) );
%	set(h, 'EdgeColor', 'none');
%	axis equal tight;
% 	ax = fig.CurrentAxes;
% 	ax.YDir = 'reverse';
% 	ax.XAxisLocation = 'top';
end

% rcs1 = ReadRcsResult( geom1 );
% rcs2 = ReadRcsResult( geom2 );
% rcs3 = ReadRcsResult( geom3 );
% rcs4 = ReadRcsResult( geom4 );
% rcs5 = ReadRcsResult( geom5 );

T = readtable( "RCSs.dat" );
rcs0 = table2array( T(:,2) );

figure(  );
hold on;
for k = 1:N
	plot( -180 : 2 : 180, rcs( :, k ), "Color", [ 1-((k-1)/(N-1)), ((k-1)/(N-1)), 0 ] );
end
hold off;


figure(  );
plot( -180:2:180, rcs0, "black" )
hold on;
for k = 1:N
	rcsN = rcs( :, k ) - min( rcs( :, k ) );
	rcsN = rcsN ./ max( rcsN );
	rcsN = rcsN .* ( max( rcs0 ) - min( rcs0 ) );
	rcsN = rcsN + min( rcs0 );
	plot( -180 : 2 : 180, rcsN, "Color", [ 1-((k-1)/(N-1)), ((k-1)/(N-1)), 0 ] );
end
hold off;

for k = 1:N
	rcsN = rcs( :, k ) - min( rcs( :, k ) );
	rcsN = rcsN ./ max( rcsN );
	rcsN = rcsN .* ( max( rcs0 ) - min( rcs0 ) );
	rcsN = rcsN + min( rcs0 );
	figure(  );
	hold on;
	plot( -180:2:180, rcs0, "black" );
	plot( -180 : 2 : 180, rcsN, "Color", [ 1-((k-1)/(N-1)), ((k-1)/(N-1)), 0 ] );
	hold off;
end

% plot( ...
%     -180:2:180, rcs1, "red", ...
%     -180:2:180, rcs2, "green", ...
%     -180:2:180, rcs3, "blue", ...
%     -180:2:180, rcs4, "cyan", ...
%     -180:2:180, rcs5, "magenta" ...
% );

% rcs1N = rcs1 - min( rcs1 );
% rcs1N = rcs1N ./ max( rcs1N );
% rcs1N = rcs1N .* ( max( rcs0 ) - min( rcs0 ) );
% rcs1N = rcs1N + min( rcs0 );
% 
% rcs2N = rcs2 - min( rcs2 );
% rcs2N = rcs2N ./ max( rcs2N );
% rcs2N = rcs2N .* ( max( rcs0 ) - min( rcs0 ) );
% rcs2N = rcs2N + min( rcs0 );
% 
% rcs3N = rcs3 - min( rcs3 );
% rcs3N = rcs3N ./ max( rcs3N );
% rcs3N = rcs3N .* ( max( rcs0 ) - min( rcs0 ) );
% rcs3N = rcs3N + min( rcs0 );
% 
% rcs4N = rcs4 - min( rcs4 );
% rcs4N = rcs4N ./ max( rcs4N );
% rcs4N = rcs4N .* ( max( rcs0 ) - min( rcs0 ) );
% rcs4N = rcs4N + min( rcs0 );
% 
% rcs5N = rcs5 - min( rcs5 );
% rcs5N = rcs5N ./ max( rcs5N );
% rcs5N = rcs5N .* ( max( rcs0 ) - min( rcs0 ) );
% rcs5N = rcs5N + min( rcs0 );
% 
% 
% figure(  );
% plot( ...
%     -180:2:180, rcs0, "black", ...
%     -180:2:180, rcs1N, "red", ...
%     -180:2:180, rcs2N, "green", ...
%     -180:2:180, rcs3N, "blue", ...
%     -180:2:180, rcs4N, "cyan", ...
%     -180:2:180, rcs5N, "magenta" ...
% );