function [ Es ] = GetEs( ...
	r, freq, ...
	faceCount, faceArea, tetraVol, ...
	vertexList, swgVertexIndex, faceTetraIndex, ...
	kontrast, kontrastDiff, coefVector, ...
	faceQuadW, faceQuadR, tetraQuadW, tetraQuadR ...
)
	
	Es = [ 0; 0; 0 ];
	
	angFreq = 2 * pi * freq;
	waveLen = M.c0 / freq;
	waveNum = 2 * pi / waveLen;
	
	KP = 1 / ( 4 * pi * M.eps0 );
	KA = ( 1i * angFreq * M.mu0 ) / ( 12 * pi );
	
	for idFace = 1 : faceCount
		
		gradPhi = [ 0; 0; 0 ];
		vecA = [ 0; 0; 0 ];
		
		%%%
		
		idTetra1 = faceTetraIndex( 1, idFace );
		idTetra2 = faceTetraIndex( 2, idFace );
		
		faceAr = faceArea( idFace );
		coef = coefVector( idFace );
		konDiff = kontrastDiff( idFace );
		
		%%% For 0
		
		qW0 = squeeze( faceQuadW( 1, idFace, : ) );
		qX0 = squeeze( faceQuadR( 1, idFace, : ) );
		qY0 = squeeze( faceQuadR( 2, idFace, : ) );
		qZ0 = squeeze( faceQuadR( 3, idFace, : ) );
		
		IP0 = M.GreenInt_Phi( waveNum, r, qW0, qX0, qY0, qZ0 );
		
		gradPhi = gradPhi - konDiff .* IP0;
		
		%%%
		
		%%% For +
		
		qW1 = squeeze( tetraQuadW( 1, idTetra1, : ) );
		qX1 = squeeze( tetraQuadR( 1, idTetra1, : ) );
		qY1 = squeeze( tetraQuadR( 2, idTetra1, : ) );
		qZ1 = squeeze( tetraQuadR( 3, idTetra1, : ) );
		
		kon1 = kontrast( idTetra1 );
		tetVol1 = tetraVol( idTetra1 );
		swgFactor1 = ( kon1 .* faceAr ./ tetVol1 );
		swgVertex1 = vertexList( :, swgVertexIndex( 1, idFace ) );
		
		IP1 = M.GreenInt_Phi( waveNum, r, qW1, qX1, qY1, qZ1 );
		IA1 = M.GreenInt_A( waveNum, r, swgVertex1, qW1, qX1, qY1, qZ1 );
		
		gradPhi = gradPhi + swgFactor1 .* IP1;
		vecA = vecA + swgFactor1 .* IA1;
		
		%%%
		
		%%% For -
		
		if idTetra2 ~= -1
			
			qW2 = squeeze( tetraQuadW( 1, idTetra2, : ) );
			qX2 = squeeze( tetraQuadR( 1, idTetra2, : ) );
			qY2 = squeeze( tetraQuadR( 2, idTetra2, : ) );
			qZ2 = squeeze( tetraQuadR( 3, idTetra2, : ) );
			
			kon2 = kontrast( idTetra2 );
			tetVol2 = tetraVol( idTetra2 );
			swgFactor2 = ( kon2 .* faceAr ./ tetVol2 );
			swgVertex2 = vertexList( :, swgVertexIndex( 2, idFace ) );
			
			IP2 = M.GreenInt_Phi( waveNum, r, qW2, qX2, qY2, qZ2 );
			IA2 = M.GreenInt_A( waveNum, r, swgVertex2, qW2, qX2, qY2, qZ2 );

			gradPhi = gradPhi - swgFactor2 .* IP2;
			vecA = vecA - swgFactor2 .* IA2;
			
		end
		
		%%%
		
		gradPhi = KP .* coef .* gradPhi;
		vecA = KA .* coef .* vecA;
		
		Es = Es - 1i .* angFreq .* vecA - gradPhi;
		
	end
	
end

