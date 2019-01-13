function [ faceQuadCount, faceQuadW, faceQuadR ] = GenerateFaceQuad( faceCount, vertexList, faceVertexIndex, qLevel )
	
	% faceQuadW( w, faceId, quadId )
	% faceQuadR( xyz, faceId, quadId )
	
% 	faceQuadCount = 7;
% 	faceQuadW = zeros( 1, faceCount, faceQuadCount );
% 	faceQuadR = zeros( 3, faceCount, faceQuadCount );
% 	
% 	bf1 = 1.0 / 3.0;
% 	bf2 = ( 6.0 - sqrt( 15.0 ) ) / 21.0;
% 	bf3 = ( 9.0 + 2.0 * sqrt( 15.0 ) ) / 21.0;
% 	bf4 = ( 6.0 + sqrt( 15.0 ) ) / 21.0;
% 	bf5 = ( 9.0 - 2.0 * sqrt( 15.0 ) ) / 21.0;
% 	
% 	wf1 = 9.0 / 40.0;
% 	wf2 = ( 155.0 - sqrt( 15.0 ) ) / 1200.0;
% 	wf3 = ( 155.0 + sqrt( 15.0 ) ) / 1200.0;
% 	
% 	for idFace = 1 : faceCount
% 		vIndex = faceVertexIndex( :, idFace );
% 		v1 = vertexList( :, vIndex( 1 ) );
% 		v2 = vertexList( :, vIndex( 2 ) );
% 		v3 = vertexList( :, vIndex( 3 ) );
% 		
% 		ar = M.TriArea( v1, v2, v3 );
% 		
% 		faceQuadW( 1, idFace, : ) = ar .* [ wf1, wf2, wf2, wf2, wf3, wf3, wf3 ];
% 		
% 		faceQuadR( :, idFace, 1 ) = bf1 .* ( v1 + v2 + v3 );
% 		
% 		faceQuadR( :, idFace, 2 ) = bf2 .* v1 + bf2 .* v2 + bf3 .* v3;
% 		faceQuadR( :, idFace, 3 ) = bf2 .* v1 + bf3 .* v2 + bf2 .* v3;
% 		faceQuadR( :, idFace, 4 ) = bf3 .* v1 + bf2 .* v2 + bf2 .* v3;
% 		
% 		faceQuadR( :, idFace, 5 ) = bf4 .* v1 + bf4 .* v2 + bf5 .* v3;
% 		faceQuadR( :, idFace, 6 ) = bf4 .* v1 + bf5 .* v2 + bf4 .* v3;
% 		faceQuadR( :, idFace, 7 ) = bf5 .* v1 + bf4 .* v2 + bf4 .* v3;
% 	end

	faceQuadCount = 4 ^ qLevel;
	
	faceQuadW = zeros( 1, faceCount, faceQuadCount );
	faceQuadR = zeros( 3, faceCount, faceQuadCount );

	for idFace = 1 : faceCount
		vIndex = faceVertexIndex( :, idFace );
		v1 = vertexList( :, vIndex( 1 ) );
		v2 = vertexList( :, vIndex( 2 ) );
		v3 = vertexList( :, vIndex( 3 ) );
		
		for k = 1 : qLevel
			[ v1, v2, v3 ] = M.SubdivideTri( v1, v2, v3, 4^(k-1) );
		end
		
		vm = ( v1 + v2 + v3 ) ./ 3;
		
		v12 = v2 - v1;
		v13 = v3 - v1;
		
		vc = [ ...
			v12(2,:) .* v13(3,:) - v12(3,:) .* v13(2,:) ; ...
			v12(3,:) .* v13(1,:) - v12(1,:) .* v13(3,:) ; ...
			v12(1,:) .* v13(2,:) - v12(2,:) .* v13(1,:) ...
		];
		
		ar = sqrt( vc(1,:).^2 + vc(2,:).^2 + vc(3,:).^2 ) ./ 2;
		faceQuadW( 1, idFace, : ) = ar;
		faceQuadR( 1, idFace, : ) = vm( 1, : );
		faceQuadR( 2, idFace, : ) = vm( 2, : );
		faceQuadR( 3, idFace, : ) = vm( 3, : );
		
	end
	
end