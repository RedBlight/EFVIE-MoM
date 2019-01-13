% clc;
% clear all;
% close all;
% format long;
% 
% v1 = [ 1; 0; 0 ];
% v2 = [ 0; 1; 0 ];
% v3 = [ 0; 0; 1 ];
% v4 = [ 0; 0; 0 ];
% 
% ErrRel = @( A_, N_ ) max( abs( ( A_ - N_ ) ./ N_ ) );
% IsValid = @( n_ ) ~isnan( sum( n_ ) ) && ~isinf( sum( n_ ) );
% 
% 
% valA = SingIntVec( vr, v1, v2, v3, v4 );
% valN = AdaptiveSingIntVec( vr, v1, v2, v3, v4, 0, [0,0,0] );
% 
% ErrRel( valA, valN )

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

clc;
clear all;
close all;
format long;

%%% errMax is expected to be around 1E-1
%%% and it is expected to get smaller as adaptive integral goes deeper

ErrRel = @( A_, N_ ) max( abs( ( A_ - N_ ) ./ N_ ) );
IsValid = @( n_ ) ~isnan( n_ ) && ~isinf( n_ );

markStr = '..............................';

v1 = [ 1; 0; 0 ];
v2 = [ 0; 1; 0 ];
v3 = [ 0; 0; 1 ];
v4 = [ 0; 0; 0 ];

v = [ v1, v2, v3, v4 ];

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% ON CORNERS & VERTEX PERMUTATIONS
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

fprintf( 'ON CORNERS & VERTEX PERMUTATIONS\n' );

errMax = 0;
errMean = 0;
errMin = inf;

T = 4 * 1 * 2 * 3 * 4;

vp = perms( [ 1, 2, 3, 4 ] );

for idr = 1 : 4
	for idp = 1 : 24
		vecA = SingIntVec( v( :, idr ), v( :, vp( idp, 1 ) ), v( :, vp( idp, 2 ) ), v( :, vp( idp, 3 ) ), v( :, vp( idp, 4 ) ) );
		vecN = AdaptiveSingIntVec( v( :, idr ), v1, v2, v3, v4, 0, [0;0;0] );
		err = ErrRel( vecA, vecN );
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

N = 2^6;
T = N*6;

vmList = [ v1, v1, v1, v2, v2, v3 ];
vpList = [ v2, v3, v4, v3, v4, v4 ];

for ide = 1 : 6
	
	vm = vmList( :, ide );
	vp = vpList( :, ide );
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
		vecA = SingIntVec( vr, v1, v2, v3, v4 );
		vecN = AdaptiveSingIntVec( vr, v1, v2, v3, v4, 0, [0;0;0] );
		err = ErrRel( vecA, vecN );
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

N = 2^6;
T = N * 4;

errMax = 0;
errMean = 0;
errMin = inf;

vaList = [ v1, v1, v1, v2 ];
vbList = [ v2, v2, v3, v3 ];
vcList = [ v3, v4, v4, v4 ];

for m = 1 : 4
	
	va = vaList( :, m );
	vb = vbList( :, m );
	vc = vcList( :, m );
	
	vecOr = M.TriMid( va, vb, vc );

	dirUp = M.VecUnit( vc - vecOr );
	dirOut = M.VecUnit( M.VecCross( dirUp, vb - vecOr ) );
	dirRight = M.VecUnit( M.VecCross( dirUp, dirOut ) );

	radiusMin = min( [ M.VecLength( va - vecOr ), M.VecLength( vb - vecOr ), M.VecLength( vc - vecOr ), ] );
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
		vecA = SingIntVec( vr, v1, v2, v3, v4 );
		vecN = AdaptiveSingIntVec( vr, v1, v2, v3, v4, 0, [0;0;0] );
		err = ErrRel( vecA, vecN );
		errMax = max( err, errMax );
		errMean = errMean + err;
		errMin = min( err, errMin );
		
	end
	
	fprintf( '\n' );
	
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

N = 2^6;
T = N;

errMax = 0;
errMean = 0;
errMin = inf;

vecOr = M.TetMid( v1, v2, v3, v4 );

dirUp = M.VecUnit( v3 - vecOr );
dirOut = M.VecUnit( M.VecCross( dirUp, v2 - vecOr ) );
dirRight = M.VecUnit( M.VecCross( dirUp, dirOut ) );

radiusMin = min( [ M.VecLength( v1 - vecOr ), M.VecLength( v2 - vecOr ), M.VecLength( v3 - vecOr ), M.VecLength( v4 - vecOr ) ] );
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
	vecA = SingIntVec( vr, v1, v2, v3, v4 );
	vecN = AdaptiveSingIntVec( vr, v1, v2, v3, v4, 0, [0;0;0] );
	err = ErrRel( vecA, vecN );
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











