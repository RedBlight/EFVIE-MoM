clc;
clear all;
close all;
format long;

%%% errMax is expected to be around 1E-1
%%% and it is expected to get smaller as adaptive integral goes deeper

ErrRel = @( A_, N_ ) abs( ( A_ - N_ ) / N_ );
IsValid = @( n_ ) ~isnan( n_ ) && ~isinf( n_ );


markStr = '..............................';

v1 = [ 1; 0; 0 ];
v2 = [ 0; 1; 0 ];
v3 = [ 0; 0; 1 ];

v = [ v1, v2, v3 ];

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% ON CORNERS & VERTEX PERMUTATIONS
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

fprintf( 'ON CORNERS & VERTEX PERMUTATIONS\n' );

errMax = 0;
errMean = 0;
errMin = inf;

vp1 = [ v1, v1, v2, v2, v3, v3 ];
vp2 = [ v2, v3, v1, v3, v1, v2 ];
vp3 = [ v3, v2, v3, v1, v2, v1 ];

T = 3 * 6;

for idr = 1 : 3
	for idp = 1 : 6
		valA = SingIntSur( v( :, idr ), vp1( :, idp ), vp2( :, idp ), vp3( :, idp ) );
		valN = AdaptiveSingIntSur( v( :, idr ), v1, v2, v3, 0, 0 );
		err = ErrRel( valA, valN );
		errMax = max( err, errMax );
		errMean = errMean + err;
		errMin = min( err, errMin );
	end
end

errMean = errMean / T;

fprintf( 'errMax: %d\n', errMax );
fprintf( 'errMid: %d\n', errMean );
fprintf( 'errMin: %d\n\n', errMin );

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% ON EDGES
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

fprintf( 'ON EDGES\n' );

errMax = 0;
errMean = 0;
errMin = inf;

N = 2^10;
T = N*3;

for ide = 1 : 3
	
	vm = v( :, ide );
	vp = v( :, mod( ide, 3 ) + 1 );
	vecL = vp - vm;
	f = rand( 1, N ) .* 3 - 1;
	
	markN = int64( linspace( 1, N, 30 ) );
	idMark = 1;

	fprintf( markStr );
	fprintf( '\n' );
	
	for idr = 1 : N
		
		if idr == markN( idMark )
			fprintf( '.' );
			idMark = idMark + 1;
		end
		
		vr = vm + vecL .* f( idr );
		valA = SingIntSur( vr, v1, v2, v3 );
		valN = AdaptiveSingIntSur( vr, v1, v2, v3, 0, 0 );
		err = ErrRel( valA, valN );
		errMax = max( err, errMax );
		errMean = errMean + err;
		errMin = min( err, errMin );
		
	end
	
	fprintf( '\n' );
	
end

errMean = errMean / T;

fprintf( 'errMax: %d\n', errMax );
fprintf( 'errMid: %d\n', errMean );
fprintf( 'errMin: %d\n\n', errMin );

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% ON FACE
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

fprintf( 'ON FACE\n' );

N = 2^10;
T = N;

errMax = 0;
errMean = 0;
errMin = inf;

vecOr = M.TriMid( v1, v2, v3 );

dirUp = M.VecUnit( v3 - vecOr );
dirOut = M.VecUnit( M.VecCross( dirUp, v2 - vecOr ) );
dirRight = M.VecUnit( M.VecCross( dirUp, dirOut ) );

radiusMin = min( [ M.VecLength( v1 - vecOr ), M.VecLength( v2 - vecOr ), M.VecLength( v3 - vecOr ), ] );
radius = radiusMin * 2;

magUp = ( rand( 1, N ) .* 2 - 1 ) .* radius;
magRight = ( rand( 1, N ) .* 2 - 1 ) .* radius;

markN = int64( linspace( 1, N, 30 ) );
idMark = 1;

fprintf( markStr );
fprintf( '\n' );

for k = 1 : N
	
	if k == markN( idMark )
		fprintf( '.' );
		idMark = idMark + 1;
	end
	
	vr = vecOr + dirUp .* magUp( k ) + dirRight .* magRight( k );
	valA = SingIntSur( vr, v1, v2, v3 );
	valN = AdaptiveSingIntSur( vr, v1, v2, v3, 0, 0 );
	err = ErrRel( valA, valN );
	errMax = max( err, errMax );
	errMean = errMean + err;
	errMin = min( err, errMin );
	
end

errMean = errMean / T;

fprintf( '\n' );
fprintf( 'errMax: %d\n', errMax );
fprintf( 'errMid: %d\n', errMean );
fprintf( 'errMin: %d\n\n', errMin );

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% ON VOLUME
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

fprintf( 'ON VOLUME\n' );

N = 2^10;
T = N;

errMax = 0;
errMean = 0;
errMin = inf;

vecOr = M.TriMid( v1, v2, v3 );

dirUp = M.VecUnit( v3 - vecOr );
dirOut = M.VecUnit( M.VecCross( dirUp, v2 - vecOr ) );
dirRight = M.VecUnit( M.VecCross( dirUp, dirOut ) );

radiusMin = min( [ M.VecLength( v1 - vecOr ), M.VecLength( v2 - vecOr ), M.VecLength( v3 - vecOr ), ] );
radius = radiusMin * 2;

magUp = ( rand( 1, N ) .* 2 - 1 ) .* radius;
magRight = ( rand( 1, N ) .* 2 - 1 ) .* radius;
magOut = ( rand( 1, N ) .* 2 - 1 ) .* radius;

markN = int64( linspace( 1, N, 30 ) );
idMark = 1;

fprintf( markStr );
fprintf( '\n' );

for k = 1 : N
	
	if k == markN( idMark )
		fprintf( '.' );
		idMark = idMark + 1;
	end
	
	vr = vecOr + dirUp .* magUp( k ) + dirRight .* magRight( k ) + dirOut .* magOut( k );
	valA = SingIntSur( vr, v1, v2, v3 );
	valN = AdaptiveSingIntSur( vr, v1, v2, v3, 0, 0 );
	err = ErrRel( valA, valN );
	errMax = max( err, errMax );
	errMean = errMean + err;
	errMin = min( err, errMin );
	
end

errMean = errMean / T;

fprintf( '\n' );
fprintf( 'errMax: %d\n', errMax );
fprintf( 'errMid: %d\n', errMean );
fprintf( 'errMin: %d\n\n', errMin );

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

















% %%%
% vecL = v2 - v1;
% magL = M.VecLength( vecL );
% dirL = M.VecUnit( vecL );
% errCount = 0;
% for dt = linspace( 0, 1, 255 )
% 	%%%disp( dt * 255 );
% 	vr = v1 + dt .* magL .* dirL + [0;0;0];
% 	valA = SingIntSur( vr, v1, v2, v3 );
% 	valN = AdaptiveSingIntSur( vr, v1, v2, v3, 0, 0 );
% 	if( ErrRel( valA, valN ) > 1E-1 || ~IsValid( valA ) || ~IsValid( valN ) )
% 	%if( ~IsValid( valA ) )
% 		errCount = errCount + 1;
% 		disp( "!!!Error" );
% 		disp( vr );
% 		disp( valA );
% 		disp( valN );
% 		
% 		SingIntSurDebug( vr, v1, v2, v3 );
% 		pause;
% 	end
% end
% disp( "v1-v2 error Count: " + string( errCount ) 










% 
% 
% valA = SingIntSur( vr, v1, v2, v3 );
% valN = AdaptiveSingIntSur( vr, v1, v2, v3, 0, 0 );
% err = ErrRel( valA, valN );
% errMax = max( err, errMax );
% errMin = min( err, errMin );
% 
% valA = SingIntSur( vr, v1, v3, v2 );
% valN = AdaptiveSingIntSur( vr, v1, v2, v3, 0, 0 );
% err = ErrRel( valA, valN );
% errMax = max( err, errMax );
% errMin = min( err, errMin );
% 
% valA = SingIntSur( vr, v2, v1, v3 );
% valN = AdaptiveSingIntSur( vr, v1, v2, v3, 0, 0 );
% err = ErrRel( valA, valN );
% errMax = max( err, errMax );
% errMin = min( err, errMin );
% 
% valA = SingIntSur( vr, v2, v3, v1 );
% valN = AdaptiveSingIntSur( vr, v1, v2, v3, 0, 0 );
% err = ErrRel( valA, valN );
% errMax = max( err, errMax );
% errMin = min( err, errMin );
% 
% valA = SingIntSur( vr, v3, v1, v2 );
% valN = AdaptiveSingIntSur( vr, v1, v2, v3, 0, 0 );
% err = ErrRel( valA, valN );
% errMax = max( err, errMax );
% errMin = min( err, errMin );
% 
% valA = SingIntSur( vr, v3, v2, v1 );
% valN = AdaptiveSingIntSur( vr, v1, v2, v3, 0, 0 );
% err = ErrRel( valA, valN );
% errMax = max( err, errMax );
% errMin = min( err, errMin );
% 
% 
% %%%
% disp( "Case: Edges" )
% 
% %%%
% vecL = v2 - v1;
% magL = M.VecLength( vecL );
% dirL = M.VecUnit( vecL );
% errCount = 0;
% for dt = linspace( 0, 1, 255 )
% 	%%%disp( dt * 255 );
% 	vr = v1 + dt .* magL .* dirL + [0;0;0];
% 	valA = SingIntSur( vr, v1, v2, v3 );
% 	valN = AdaptiveSingIntSur( vr, v1, v2, v3, 0, 0 );
% 	if( ErrRel( valA, valN ) > 1E-1 || ~IsValid( valA ) || ~IsValid( valN ) )
% 	%if( ~IsValid( valA ) )
% 		errCount = errCount + 1;
% 		disp( "!!!Error" );
% 		disp( vr );
% 		disp( valA );
% 		disp( valN );
% 		
% 		SingIntSurDebug( vr, v1, v2, v3 );
% 		pause;
% 	end
% end
% disp( "v1-v2 error Count: " + string( errCount ) )
% 
% %%%
% vecL = v3 - v2;
% magL = M.VecLength( vecL );
% dirL = M.VecUnit( vecL );
% errCount = 0;
% for dt = linspace( 0, 1, 255 )
% 	%%%disp( dt * 255 );
% 	vr = v2 + dt .* magL .* dirL + [0;0;0];
% 	valA = SingIntSur( vr, v1, v2, v3 );
% 	valN = AdaptiveSingIntSur( vr, v1, v2, v3, 0, 0 );
% 	if( ErrRel( valA, valN ) > 1E-1 || ~IsValid( valA ) || ~IsValid( valN ) )
% 	%if( ~IsValid( valA ) )
% 		errCount = errCount + 1;
% 		disp( "!!!Error" );
% 		disp( vr );
% 		disp( valA );
% 		disp( valN );
% 		
% 		SingIntSurDebug( vr, v1, v2, v3 );
% 		pause;
% 	end
% end
% disp( "v2-v3 error Count: " + string( errCount ) )
% 
% %%%
% vecL = v1 - v3;
% magL = M.VecLength( vecL );
% dirL = M.VecUnit( vecL );
% errCount = 0;
% for dt = linspace( 0, 1, 255 )
% 	%%%disp( dt * 255 );
% 	vr = v3 + dt .* magL .* dirL + [0;0;0];
% 	valA = SingIntSur( vr, v1, v2, v3 );
% 	valN = AdaptiveSingIntSur( vr, v1, v2, v3, 0, 0 );
% 	if( ErrRel( valA, valN ) > 1E-1 || ~IsValid( valA ) || ~IsValid( valN ) )
% 	%if( ~IsValid( valA ) )
% 		errCount = errCount + 1;
% 		disp( "!!!Error" );
% 		disp( vr );
% 		disp( valA );
% 		disp( valN );
% 		
% 		SingIntSurDebug( vr, v1, v2, v3 );
% 		pause;
% 	end
% end
% disp( "v3-v1 error Count: " + string( errCount ) )
% 
% 
% %%%
% disp( "Case: OnPlane" )
% 
% vecOr = vm;
% dirUp = M.VecUnit( v3 - vm );
% dirOut = M.VecUnit( M.VecCross( dirUp, v2 - vm ) );
% dirRight = M.VecUnit( M.VecCross( dirUp, dirOut ) );
% radius = 3;
% N = 1000;
% magUp = rand( 1, N ) .* 2 .* radius - radius;
% magRight = rand( 1, N ) .* 2 .* radius - radius;
% for k = 1 : N
% 	disp(k);
% 	vr = vecOr + dirUp .* magUp( k ) + dirRight .* magRight( k );
% 	valA = SingIntSur( vr, v1, v2, v3 );
% 	valN = AdaptiveSingIntSur( vr, v1, v2, v3, 0, 0 );
% 	if( ErrRel( valA, valN ) > 1E-1 || ~IsValid( valA ) || ~IsValid( valN ) )
% 	%if( ~IsValid( valA ) )
% 		errCount = errCount + 1;
% 		disp( "!!!Error" );
% 		disp( vr );
% 		disp( valA );
% 		disp( valN );
% 		
% 		SingIntSurDebug( vr, v1, v2, v3 );
% 		pause;
% 	end
% end
% 
% % s = repmat( vecOr, 1, N ) + repmat( dirUp, 1, N ) .* magUp + repmat( dirRight, 1, N ) .* magRight;
% % 
% % figure();
% % scatter3(s(1,:),s(2,:),s(3,:));
