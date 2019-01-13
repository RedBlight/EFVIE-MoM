classdef M
	%MATH
	
	properties( Constant )
		
		eps0 = 8.854187817E-12;
		mu0 = 12.566370614E-7
		c0 = 299792458;
		
	end
	
	methods( Static )
		
		%%% VECTOR OPS
		
		function len_ = VecLength( vec_ )
			len_ = sqrt( vec_( 1 ) .^ 2 + vec_( 2 ) .^ 2 + vec_( 3 ) .^ 2 );
		end
		
		function unit_ = VecUnit( vec_ )
			unit_ = vec_ ./ M.VecLength( vec_ );
		end
		
		function dot_ = VecDot( v1_, v2_ )
			dot_ = sum( v1_ .* v2_ );
		end
		
		function cross_ = VecCross( v1_, v2_ )
			cross_ = [ ...
				v1_(2) * v2_(3) - v1_(3) * v2_(2); ...
				v1_(3) * v2_(1) - v1_(1) * v2_(3); ...
				v1_(1) * v2_(2) - v1_(2) * v2_(1) ...
			];
		end
		
		%%% Basis change
		
		function [ rhoF, phiF, zF ] = CartToPolBasis( x, y, z, xF, yF, zF )
			
			xBasis = [ 1; 0; 0 ];
			yBasis = [ 0; 1; 0 ];
			zBasis = [ 0; 0; 1 ];
			
			rhoBasis = M.VecUnit( [ x; y; 0 ] );
			phiBasis = M.VecUnit( [ -y; x; 0 ] );
			
			cartVec = xBasis .* xF + yBasis .* yF + zBasis .* zF;
			
			rhoF = M.VecDot( rhoBasis, cartVec );
			phiF = M.VecDot( phiBasis, cartVec );
			zF = M.VecDot( zBasis, cartVec );
			
		end

		%%% GGG
		function p_ = ProjPlane( r_, n_, v1_ )
			p_ = r_ - n_ .* M.VecDot( r_ - v1_, n_ );
		end
		
		function n_ = PlaneNormal( v1_, v2_, v3_ )
			n_ = M.VecUnit( M.VecCross( v2_ - v1_, v3_ - v1_ ) );
		end
		
		function n_ = PlaneNormalP( vp_, v1_, v2_, v3_ )
			n_ = M.VecUnit( M.VecCross( v2_ - v1_, v3_ - v1_ ) );
			p = M.ProjPlane( vp_, n_, v1_ );
			n_ = sign( M.VecDot( vp_ - p, n_ ) ) .* n_;
		end
		
		function n_ = LineNormalP( v1_, v2_, vp_ )
			l_ = M.VecUnit( v2_ - v1_ );
			v1p = vp_ - v1_;
			v1p_l = l_ .* M.VecDot( v1p, l_ );
			n_ = M.VecUnit( v1p - v1p_l );
		end
		
		function p_ = ProjLine( v1_, v2_, r_ )
			l_ = M.VecUnit( v2_ - v1_ );
			v1r = r_ - v1_;
			v1r_l = l_ .* M.VecDot( v1r, l_ );
			p_ = v1_ + v1r_l;
		end
		
		%%% GEOMETRY OPS
		
		function vm_ = TriMid( v1_, v2_, v3_ )
			vm_ = ( v1_ + v2_ + v3_ ) ./ 3;
		end
		
		function vm_ = TetMid( v1_, v2_, v3_, v4_ )
			vm_ = ( v1_ + v2_ + v3_ + v4_ ) ./ 4;
		end
		
		function area_ = TriArea( v1_, v2_, v3_ )
			area_ = M.VecLength( M.VecCross( v2_ - v1_, v3_ - v1_ ) ) / 2;
		end
		
		function vol_ = TetVol( v1_, v2_, v3_, v4_ )
			vol_ = abs( M.VecDot( M.VecCross( v2_ - v1_, v3_ - v1_ ), v4_ - v1_ ) ) / 6;
		end
		
		function isIn = PointInTet( r, v1, v2, v3, v4 )
			pn1 = M.PlaneNormalP( r, v1, v2, v3 );
			pn2 = M.PlaneNormalP( r, v1, v2, v4 );
			pn3 = M.PlaneNormalP( r, v1, v3, v4 );
			pn4 = M.PlaneNormalP( r, v2, v3, v4 );
			
			in1 = M.PlaneNormalP( v4, v1, v2, v3 );
			in2 = M.PlaneNormalP( v3, v1, v2, v4 );
			in3 = M.PlaneNormalP( v2, v1, v3, v4 );
			in4 = M.PlaneNormalP( v1, v2, v3, v4 );
			
			isIn = ...
				M.VecDot( pn1, in1 ) > 0 && ...
				M.VecDot( pn2, in2 ) > 0 && ...
				M.VecDot( pn3, in3 ) > 0 && ...
				M.VecDot( pn4, in4 ) > 0 ...
			;
			
			
		end
		
		%%% SUBDIVISIONS
		
		function [ v1LN, v2LN, v3LN ] = SubdivideTri( v1L, v2L, v3L, triCount )
			
			v1LN = zeros( 3, triCount * 4 );
			v2LN = zeros( 3, triCount * 4 );
			v3LN = zeros( 3, triCount * 4 );
			
			for k = 1 : triCount
				
				n = 4 * ( k - 1 );
				
				v1 = v1L( :, k );
				v2 = v2L( :, k );
				v3 = v3L( :, k );
				
				v12 = ( v1L( :, k ) + v2L( :, k ) ) ./ 2;
				v13 = ( v1L( :, k ) + v3L( :, k ) ) ./ 2;
				v23 = ( v2L( :, k ) + v3L( :, k ) ) ./ 2;
				
				v1LN( :, n + 1 ) = v23;
				v2LN( :, n + 1 ) = v13;
				v3LN( :, n + 1 ) = v12;
				
				v1LN( :, n + 2 ) = v1;
				v2LN( :, n + 2 ) = v13;
				v3LN( :, n + 2 ) = v12;
				
				v1LN( :, n + 3 ) = v23;
				v2LN( :, n + 3 ) = v2;
				v3LN( :, n + 3 ) = v12;
				
				v1LN( :, n + 4 ) = v23;
				v2LN( :, n + 4 ) = v13;
				v3LN( :, n + 4 ) = v3;
				
			end
			
		end
		
% 		function [ v1LN, v2LN, v3LN, v4LN ] = SubdivideTet( v1L, v2L, v3L, v4L, tetCount )
% 			
% 			v1LN = zeros( 3, tetCount * 8 );
% 			v2LN = zeros( 3, tetCount * 8 );
% 			v3LN = zeros( 3, tetCount * 8 );
% 			v4LN = zeros( 3, tetCount * 8 );
% 			
% 			for k = 1 : tetCount
% 				
% 				n = 8 * ( k - 1 );
% 				
% 				v1 = v1L( :, k );
% 				v2 = v2L( :, k );
% 				v3 = v3L( :, k );
% 				v4 = v3L( :, k );
% 				
% 				v12 = ( v1L( :, k ) + v2L( :, k ) ) ./ 2;
% 				v13 = ( v1L( :, k ) + v3L( :, k ) ) ./ 2;
% 				v14 = ( v1L( :, k ) + v4L( :, k ) ) ./ 2;
% 				v23 = ( v2L( :, k ) + v3L( :, k ) ) ./ 2;
% 				v24 = ( v2L( :, k ) + v4L( :, k ) ) ./ 2;
% 				v34 = ( v3L( :, k ) + v4L( :, k ) ) ./ 2;
% 				
% 				v1LN( :, n + 1 ) = v23;
% 				v2LN( :, n + 1 ) = v13;
% 				v3LN( :, n + 1 ) = v12;
% 				
% 				v1LN( :, n + 2 ) = v1;
% 				v2LN( :, n + 2 ) = v13;
% 				v3LN( :, n + 2 ) = v12;
% 				
% 				v1LN( :, n + 3 ) = v23;
% 				v2LN( :, n + 3 ) = v2;
% 				v3LN( :, n + 3 ) = v12;
% 				
% 				v1LN( :, n + 4 ) = v23;
% 				v2LN( :, n + 4 ) = v13;
% 				v3LN( :, n + 4 ) = v3;
% 				
% 			end
% 			
% 		end
		
		%%% OTHER
		
		function swgVertex_ = GetSwgVertex( faceVertexIndex, tetraVertexIndex )
			swgVertex_ = tetraVertexIndex;
			swgVertex_ = swgVertex_( swgVertex_ ~= faceVertexIndex( 1 ) );
			swgVertex_ = swgVertex_( swgVertex_ ~= faceVertexIndex( 2 ) );
			swgVertex_ = swgVertex_( swgVertex_ ~= faceVertexIndex( 3 ) );
		end
		
		function g = Green( v1, v2, k )
			R = M.VecLength( v1 - v2 );
			g = exp( -1i .* k .* R ) ./ R;
		end
		
		function g = GreenReg( v1, v2, k )
			R = M.VecLength( v1 - v2 );
			if R == 0
				g = -1i .* k;
			else
				g = ( exp( -1i .* k .* R ) - 1 ) ./ R;
			end
		end
		
		function G = GreenRegInt( k, r, W, X, Y, Z  )
			dX = X - r( 1 );
			dY = Y - r( 2 );
			dZ = Z - r( 3 );
			R = sqrt( dX .^ 2 + dY .^ 2 + dZ .^ 2 );
			g = ( exp( -1i .* k .* R ) - 1 ) ./ R;
			g( R == 0 ) = -1i .* k;
			G = sum( W .* g );
		end
		
		function G = GreenRegIntVec( k, r, W, X, Y, Z  )
			
			dX = X - r( 1 );
			dY = Y - r( 2 );
			dZ = Z - r( 3 );
			
			R = sqrt( dX .^ 2 + dY .^ 2 + dZ .^ 2 );
			g = ( exp( -1i .* k .* R ) - 1 ) ./ R;
			g( R == 0 ) = -1i .* k;
			wg = W .* g;
			G = [ ...
				sum( dX .* wg ); ...
				sum( dY .* wg ); ...
				sum( dZ .* wg ) ...
			];
		end
		
		function G = GreenInt_Phi( k, r, W, X, Y, Z  )
			
			dX = r( 1 ) - X;
			dY = r( 2 ) - Y;
			dZ = r( 3 ) - Z;
			
			R = sqrt( dX .^ 2 + dY .^ 2 + dZ .^ 2 );
			R2 = R .* R;
			
			g = ( exp( -1i .* k .* R ) ) ./ R;
			
			wg = W .* ( ( 1i .* k ./ R ) + ( 1 ./ R2 ) ) .* g;
			G = [ ...
				sum( dX .* wg ); ...
				sum( dY .* wg ); ...
				sum( dZ .* wg ) ...
			];
		
% 			disp( size( W ) );
% 			disp( size( R ) );
% 			disp( size( g ) );
		
		end
		
		function G = GreenInt_A( k, r, rn, W, X, Y, Z  )
			
			dX = r( 1 ) - X;
			dY = r( 2 ) - Y;
			dZ = r( 3 ) - Z;
			
			R = sqrt( dX .^ 2 + dY .^ 2 + dZ .^ 2 );
			
			g = ( exp( -1i .* k .* R ) ) ./ R;
			
			dnX = X - rn( 1 );
			dnY = Y - rn( 2 );
			dnZ = Z - rn( 3 );
			
			wg = W .* g;
			G = [ ...
				sum( dnX .* wg ); ...
				sum( dnY .* wg ); ...
				sum( dnZ .* wg ) ...
			];
		
		end
		
		
	end
end

