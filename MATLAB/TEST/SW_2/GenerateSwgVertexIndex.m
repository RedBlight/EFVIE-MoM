function [ swgVertexIndex ] = GenerateSwgVertexIndex( faceCount, tetraVertexIndex, faceVertexIndex, faceTetraIndex )
		
	swgVertexIndex = zeros( 2, faceCount );
	
	for k = 1 : faceCount
		
		tetraIdxP = faceTetraIndex( 1, k );
		tetraIdxM = faceTetraIndex( 2, k );
		
		swgVertexIndex( 1, k ) = M.GetSwgVertex( faceVertexIndex( :, k ), tetraVertexIndex( :, tetraIdxP ) );
		
		if( tetraIdxM == -1 )
			swgVertexIndex( 2, k ) = -1;
		else
			swgVertexIndex( 2, k ) = M.GetSwgVertex( faceVertexIndex( :, k ), tetraVertexIndex( :, tetraIdxM ) );
		end
		
	end
	
end

