function [ Es ] = GetEs( ...
	r, freq, ...
	faceCount, faceArea, tetraVol, ...
	vertexList, swgVertexIndex, faceTetraIndex, ...
	kontrast, kontrastDiff, coefVector, ...
	faceMidPoint, tetraMidPoint ...
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
		
		faceMid = faceMidPoint( :, idFace );
		
		IP0 = M.GreenInt_Phi( waveNum, faceAr, r, faceMid );
		
		gradPhi = gradPhi - konDiff .* IP0;
		
		%%%
		
		%%% For +
		
		tetMid1 = tetraMidPoint( :, idTetra1 );
		kon1 = kontrast( idTetra1 );
		tetVol1 = tetraVol( idTetra1 );
		swgFactor1 = ( kon1 .* faceAr ./ tetVol1 );
		swgVertex1 = vertexList( :, swgVertexIndex( 1, idFace ) );
		
		IP1 = M.GreenInt_Phi( waveNum, tetVol1, r, tetMid1 );
		IA1 = M.GreenInt_A( waveNum, tetVol1, swgVertex1, r, tetMid1 );
		
		gradPhi = gradPhi + swgFactor1 .* IP1;
		vecA = vecA + swgFactor1 .* IA1;
		
		%%%
		
		%%% For -
		
		if idTetra2 ~= -1
		
			tetMid2 = tetraMidPoint( :, idTetra2 );
			kon2 = kontrast( idTetra2 );
			tetVol2 = tetraVol( idTetra2 );
			swgFactor2 = ( kon2 .* faceAr ./ tetVol2 );
			swgVertex2 = vertexList( :, swgVertexIndex( 2, idFace ) );

			IP2 = M.GreenInt_Phi( waveNum, tetVol2, r, tetMid2 );
			IA2 = M.GreenInt_A( waveNum, tetVol2, swgVertex2, r, tetMid2 );

			gradPhi = gradPhi - swgFactor2 .* IP2;
			vecA = vecA - swgFactor2 .* IA2;
			
		end
		
		%%%
		
		gradPhi = KP .* coef .* gradPhi;
		vecA = KA .* coef .* vecA;
		
		Es = Es - 1i .* angFreq .* vecA - gradPhi;
		
	end
	
end

