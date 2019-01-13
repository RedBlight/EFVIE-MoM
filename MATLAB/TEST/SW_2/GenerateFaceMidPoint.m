function [ faceMidPoint ] = GenerateFaceMidPoint( faceCount, vertexList, faceVertexIndex )
	
	faceMidPoint = zeros( 3, faceCount );
	
	parfor k = 1 : faceCount
		faceMidPoint( :, k ) = M.TriMid( ...
			vertexList( :, faceVertexIndex( 1, k ) ), ...
			vertexList( :, faceVertexIndex( 2, k ) ), ...
			vertexList( :, faceVertexIndex( 3, k ) ) ...
		);
	end
	
end