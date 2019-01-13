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
		
		%%% OTHER
		
		function swgVertex_ = GetSwgVertex( faceVertexIndex, tetraVertexIndex )
			swgVertex_ = tetraVertexIndex;
			swgVertex_ = swgVertex_( swgVertex_ ~= faceVertexIndex( 1 ) );
			swgVertex_ = swgVertex_( swgVertex_ ~= faceVertexIndex( 2 ) );
			swgVertex_ = swgVertex_( swgVertex_ ~= faceVertexIndex( 3 ) );
		end
		
		
	end
end

