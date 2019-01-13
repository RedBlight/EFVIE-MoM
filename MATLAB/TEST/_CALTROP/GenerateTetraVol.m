function [ tetraVol ] = GenerateTetraVol( tetraCount, vertexList, tetraVertexIndex )
	
	tetraVol = zeros( 1, tetraCount );
	
	for k = 1 : tetraCount
		tetraVol( k ) = M.TetVol( ...
			vertexList( :, tetraVertexIndex( 1, k ) ), ...
			vertexList( :, tetraVertexIndex( 2, k ) ), ...
			vertexList( :, tetraVertexIndex( 3, k ) ), ...
			vertexList( :, tetraVertexIndex( 4, k ) ) ...
		);
	end
	
end