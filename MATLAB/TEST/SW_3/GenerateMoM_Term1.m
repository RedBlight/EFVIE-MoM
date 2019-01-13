function [ momTerm1 ] = GenerateMoM_Term1( ...
	faceCount, vertexList, tetraVertexIndex, ...
	faceTetraIndex, swgVertexIndex, ...
	epsRel, faceArea, tetraVol, ...
	tetraMidPoint ...
)
	% m = which row, n = which col
	
	momTerm1 = zeros( faceCount, faceCount );
	
	parfor m = 1 : faceCount
		for n = 1 : faceCount
			faceIdxM = m;
			faceIdxN = n;
			for a = 1 : 2
				for b = 1 : 2
					tetraIdxA = faceTetraIndex( a, faceIdxM );
					tetraIdxB = faceTetraIndex( b, faceIdxN );
					if( tetraIdxA == tetraIdxB && tetraIdxA ~= -1 && tetraIdxB ~= -1 )
						
						epsFactor = 1 / ( M.eps0 * epsRel( tetraIdxB ) );
						geomFactor = faceArea( faceIdxM ) * faceArea( faceIdxN ) / ( 9 * tetraVol( tetraIdxA ) );
						v1 = vertexList( :, tetraVertexIndex( 1, tetraIdxA ) );
						v2 = vertexList( :, tetraVertexIndex( 2, tetraIdxA ) );
						v3 = vertexList( :, tetraVertexIndex( 3, tetraIdxA ) );
						v4 = vertexList( :, tetraVertexIndex( 4, tetraIdxA ) );
						vm = vertexList( :, swgVertexIndex( a, faceIdxM ) );
						vn = vertexList( :, swgVertexIndex( b, faceIdxN ) );
						vc = tetraMidPoint( :, tetraIdxA );
						
						gmA = double( a == 1 ) * 1 + double( a == 2 ) * (-1);
						gmB = double( b == 1 ) * 1 + double( b == 2 ) * (-1);
						gm = gmA * gmB;
						
						%%%%
						%gm = 1;
						%%%%
						
						momTerm1( m, n ) = momTerm1( m, n ) + gm * epsFactor * geomFactor * ( ...
							(8/5) * M.VecLength( vc )^2 - M.VecDot( vm + vn, vc ) + M.VecDot( vm, vn ) ...
							- (1/10) * ( M.VecDot( v1, v2 ) + M.VecDot( v1, v3 ) + M.VecDot( v1, v4 ) ...
							+ M.VecDot( v2, v3 ) + M.VecDot( v2, v4 ) + M.VecDot( v3, v4 ) ) ...
						);
						
					end
				end
			end
		end
	end
	
end

