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
			unit_ = vec_ ./ VecLength( vec_ );
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

